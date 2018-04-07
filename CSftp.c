#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "dir.h"
#include "usage.h"
#include <string.h>
#include <unistd.h>
#include "parser.h"
#include "login.h"
#include "cwdcmd.h"
#include "ftpmode.h"
#include "ftptype.h"
#include "ftpcdup.h"
#include "ftpstru.h"

// Here is an example of how to use the above function. It also shows
// one how to get the arguments passed on the command line.

#define BACKLOG 10

int makePort(char* c) {
	int port = 0;
	int i = 0;
	while (c[i] != '\0') {
		port = port * 10;
        port = port + ((int) c[i] - 48);
        i++;
    }
    return port;
}

short bindPort(int fd, struct sockaddr_in addr) {
    short port;
    while (1) {
        port = (rand() % 60000) + 5000;
        addr.sin_port = htons(port);
        if (bind(fd, (struct sockaddr *)&addr, sizeof (struct sockaddr_in)) == 0)
            return port;
    }
} 

int main(int argc, char **argv) {

    // This is some sample code feel free to delete it
    // This is the main program for the thread version of nc

    int i;

    // Check the command line arguments
    if (argc != 2) {
        usage(argv[0]);
        return -1;
    }
    // int port = 1025;
    int port = makePort(argv[1]);
    if (port < 1024 || port > 65535) {
        perror("Port error");
        usage(argv[0]);
        return -1;
    }
    printf("port is %d\n", port);

    // initialize
    int connectionfd, controlfd, datafd;
    
    struct sockaddr_in connection_addr, control_addr, data_addr;
    int len = 0;
    socklen_t sin_size;
    char buff[BUFSIZ];
    memset(&connection_addr, 0, sizeof(connection_addr)); // initialize memory
    connection_addr.sin_family = AF_INET;
    connection_addr.sin_addr.s_addr = INADDR_ANY;
    connection_addr.sin_port = htons(port); 
    
    // setup socket
    sin_size = sizeof(control_addr);
    connectionfd = socket(PF_INET, SOCK_STREAM, 0);
    if (connectionfd < 0) {
        perror("Socket setup error");
        return 1;
    }
    
    // binding
    if (bind(connectionfd, (struct sockaddr*) &connection_addr, sizeof(struct sockaddr)) < 0) {
        perror("Binding error");
        return 1;
    }
    
    // listen
    if (listen(connectionfd, BACKLOG) < 0) {
        perror("Listen error");
        return 1;
    }
    
    printf("Server: Finished setup.\n");

    // init
    int clientNum = 0;
    int logged = 0;
    char init_dir[1024];
    char curr_dir[1024];
    ftp_mode_t ftpMode;
    ftp_type_t ftpType;
    ftp_stru_t ftpStru;
    ftp_mode_t new_mode;
    ftp_type_t new_type;
    ftp_stru_t new_stru;
    FILE *file_to_send;
    int is_in_pasv = 0;
    chdir("dir");
    getcwd(init_dir, sizeof(init_dir));
    
    
    // loop to accept client 
    while (1) {
        if (clientNum == 0) {
            logged = 0;
            // initialize the working directory
            chdir(init_dir);
            // init mode
            ftpMode.mode = STREAM;
            ftpMode.name = 'S';
            // init type
            ftpType.type = ASCII;
            strcpy(ftpType.name, "ASCII");
            // init stru
            ftpStru.stru = FILE_STRU;
            strcpy(ftpStru.name, "FILE");
            is_in_pasv = 0;
            
            printf("Server: Waiting for connection.\n");
            controlfd = accept(connectionfd, (struct sockaddr*) &control_addr, &sin_size);
            if (controlfd < 0) {
                perror("Accept error");
                continue;
            }
            printf("Server: accept client.\n");
            len = send(controlfd, "220 Hello Client\n", (int) strlen("220 Hello Client\n"), 0);
            clientNum++;
        }
        len = recv(controlfd, buff, BUFSIZ, 0);
        if (len > 0) {
            buff[len] = '\0';
            printf("from client: %s", buff);

            ftp_cmd_t ftp_cmd = parse_buff(buff);

            switch (ftp_cmd.cmd) {
                case QUIT:
                    send(controlfd, "221 bye.\n", (int) strlen("221 bye.\n"), 0);
                    close(controlfd);
                    clientNum--;
                    continue;
                case USER:
                    logged = ftp_login(ftp_cmd.args);
                    if (logged) {
                        send(controlfd, "230 Login successful.\n", (int) strlen("230 Login successful.\n"), 0);
                    } else {
                        send(controlfd, "530 Fail to log in.\n", (int) strlen("530 Fail to log in.\n"), 0);
                    }
                    continue;
                case CWD:
                case CDUP:
                case TYPE:
                case MODE:
                case STRU:
                case RETR:
                case PASV:
                case NLST:
                    break;
                default:
                    send(controlfd, "500 Invalid command.\n", (int) strlen("500 Invalid command.\n"), 0);
                    continue;
            }

            if (logged) {
                switch (ftp_cmd.cmd) {
                    case USER:
                    case QUIT:
                        break;
                    case CWD:
                        if (ftp_cwd(ftp_cmd.args) == 0) {
                            // DONE: the response code should be change
                            send(controlfd, "250 Directory successfully changed.\n", (int) strlen("250 Directory successfully changed.\n"), 0);
                        } else {
                            send(controlfd, "550 Failed to change directory.\n", (int) strlen("550 Failed to change directory.\n"), 0);
                        }
                        break;
                    case CDUP:
                        // DONE: change response.
                        getcwd(curr_dir, sizeof(curr_dir));
                        if (ftp_cdup(curr_dir, init_dir) == 0) {
                            send(controlfd, "250 Directory successfully changed.\n", (int) strlen("250 Directory successfully changed.\n"), 0);
                        } else {
                            send(controlfd, "550 Failed to change directory.\n", (int) strlen("550 Failed to change directory.\n"), 0);
                        }
                        break;
                    case TYPE:
                        // TODO: finished but what to do with TYPE?
                        new_type = change_type(ftp_cmd.args);
                        if (new_type.type == INVALID_TYPE) {
                            send(controlfd, "501 bad TYPE command.\n", (int) strlen("501 bad TYPE command.\n"), 0);
                        } else {
                            ftpType = new_type;
                            char temp[255];
                            strcpy(temp, "200 TYPE set to ");
                            strcat(temp, ftpType.name);
                            strcat(temp, ".\n");
                            send(controlfd, temp, (int) strlen(temp), 0);
                        }
                        break;
                    case MODE:
                        // TODO: finished but what to do with MODE?
                        new_mode = change_mode(ftp_cmd.args);
                        if (new_mode.mode == INVALID_MODE) {
                            send(controlfd, "501 bad MODE command.\n", (int) strlen("501 bad MODE command.\n"), 0);
                        } else if (new_mode.mode == BLOCK || new_mode.mode == COMPRESSED) {
                            send(controlfd, "504 MODE not implemented.\n", (int) strlen("504 MODE not implemented.\n"), 0);
                        } else {
                            ftpMode = new_mode;
                            char temp[255];
                            strcpy(temp, "200 Mode set to X.\n");
                            temp[16] = ftpMode.name;
                            send(controlfd, temp, (int) strlen(temp), 0);
                        }
                        break;
                    case STRU:
                        new_stru = change_stru(ftp_cmd.args);
                        if (new_stru.stru == INVALID_STRU) {
                            send(controlfd, "501 bad STRU command.\n", (int) strlen("501 bad STRU command.\n"), 0);
                        } else if (new_stru.stru == PAGE_STRU || new_stru.stru == RECORD_STRU) {
                            send(controlfd, "504 STRU not implemented.\n", (int) strlen("504 STRU not implemented.\n"), 0);
                        } else {
                            ftpStru = new_stru;
                            char temp[255];
                            strcpy(temp, "200 STRU set to ");
                            strcat(temp, ftpStru.name);
                            strcat(temp, ".\n");
                            send(controlfd, temp, (int) strlen(temp), 0);
                        }
                        break;
                    case RETR:
                        // TODO: need pasv
                        if (is_in_pasv) {
                            if (ftpType.type == ASCII) {
                                file_to_send = fopen(ftp_cmd.args, "rt");
                            } else if (ftpType.type == BINARY) {
                                file_to_send = fopen(ftp_cmd.args, "rb");
                            }
                            if (file_to_send == NULL) {
                                send(controlfd, "550 Fail to open file.\n", (int) strlen("550 Fail to open file.\n"), 0);
                            } else {
                                send(controlfd, "150 Opend target file.\n", (int) strlen("150 Opend target file.\n"), 0);
                                char file_buf[BUFSIZ];
                                int file_block_counter = 0;
                                bzero(file_buf, BUFSIZ);
                                int error_flag = 0;
                                while ((file_block_counter = fread(file_buf, sizeof(char), BUFSIZ, file_to_send)) > 0) {
                                    // TODO: change clientfd to a pasv fd in send(), close pasv fd in somewhere
                                    if (send(datafd, file_buf, file_block_counter, 0) < 0) {
                                        perror("Send Error.");
                                        error_flag = 1;
                                    }
                                    bzero(file_buf, BUFSIZ);
                                }
                                fclose(file_to_send);
                                
                                if (error_flag) {
                                    send(controlfd, "150 Send ERROR.\n", (int) strlen("150 Send ERROR.\n"), 0);
                                } else {
                                    send(controlfd, "226 bingo file sent.\n", (int) strlen("226 bingo, file sent.\n"), 0);
                                }
                            }
                            close(datafd);
                            is_in_pasv = 0;
                        } else {
                            send(controlfd, "425 enter pasv first.\n", (int) strlen("425 enter pasv first.\n"), 0);
                        }
                        break;
                    case PASV:
                        // TODO:
                        if (getsockname(controlfd, (struct sockaddr *) &control_addr, &sin_size) == -1)
                            perror("getsockname");
                        else {
                            long ip = ntohl(control_addr.sin_addr.s_addr);
                            int tempfd;
                            struct sockaddr_in temp_addr;
                            temp_addr.sin_family = AF_INET;
                            temp_addr.sin_addr.s_addr = control_addr.sin_addr.s_addr;
                            tempfd = socket(PF_INET, SOCK_STREAM, 0);
                            short port = bindPort(tempfd, temp_addr);
                            if (listen(tempfd, BACKLOG) < 0) {
                                perror("Listen error");
                                break;
                            }
                            printf("%hu\n", port);
                            unsigned short a = ip >> 8 * 3;
                            unsigned short b = (ip & 0x00ff0000) >> 8 * 2;
                            unsigned short c = (ip & 0x0000ff00) >> 8;
                            unsigned short d = (ip & 0x000000ff);
                            unsigned short e = (port >> 8) & 0x00ff;
                            unsigned short f = port & 0x00ff;
                            char ipStr[50];
                            sprintf(ipStr, "227 Entering Passive Mode (%hu,%hu,%hu,%hu,%hu,%hu)\n", a, b, c, d, e, f);
                            send(controlfd, ipStr, (int) strlen(ipStr), 0);
                            fd_set set;
                            struct timeval timeout;
                            int rv;
                            FD_ZERO(&set); /* clear the set */
                            FD_SET(tempfd, &set); /* add our file descriptor to the set */
                            timeout.tv_sec = 20;
                            timeout.tv_usec = 0;
                            rv = select(tempfd + 1, &set, NULL, NULL, &timeout);
                            if(rv == -1) {
                                perror("select"); /* an error accured */
                                break;
                            }
                            else if(rv == 0) {
                                char buf[50] = "500 timeout occurred (20 second)\n";
                                send(controlfd, buf, (int) strlen(buf), 0); /* a timeout occured */
                                break;
                            } else
                                datafd = accept(tempfd, (struct sockaddr*) &data_addr, &sin_size);
                                is_in_pasv = 1;
                            if (datafd < 0) {
                                perror("Accept error");
                                is_in_pasv = 0;
                                break;
                            }
                        }
                        break;
                    case NLST:
                        if (is_in_pasv) {
                            if (strlen(ftp_cmd.args) == 0) {
                                send(controlfd, "150 show directory entries.\n", (int) strlen("150 show directory entries.\n"), 0);
                                listFiles(datafd, ".");
                                send(controlfd, "226 directory OK.\n", (int) strlen("226 directory OK.\n"), 0);
                            } else {
                                send(controlfd, "501 invalid parameter.\n", (int) strlen("501 invalid parameter.\n"), 0);
                            }
                            is_in_pasv = 0;
                            close(datafd);
                        } else {
                            send(controlfd, "425 enter pasv first.\n", (int) strlen("425 enter pasv first.\n"), 0);
                        }
                        break;
                    default:
                        break;
                }
            } else {
                send(controlfd, "530 please log in first.\n", (int) strlen("530 please log in first.\n"), 0);
            }
        }


    }
    close(connectionfd);

    // This is how to call the function in dir.c to get a listing of a directory.
    // It requires a file descriptor, so in your code you would pass in the file descriptor 
    // returned for the ftp server's data connection
    
//    printf("Printed %d directory entries\n", listFiles(1, "."));
    return 0;

}
