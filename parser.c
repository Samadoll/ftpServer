#include <stdio.h>
#include <string.h>
#include "parser.h"
#include <ctype.h>

char* trim_str(char* str, int len) {
	int i = len - 1;
	while (i >= 0) {
		if (str[i] == ' ') {
			i--;
			continue;
		}
		str[i+1] = '\0';
		break;
	}
	return str;
	
}

ftp_cmd_t parse_buff(char* buff) {
	// init
	ftp_cmd_t ftp_cmd;
	ftp_cmd.cmd = INVALID;
	ftp_cmd.args[0] = '\0';
	
	// parse buff
//	int ixj = 0;
//	printf("whats in side. ***********\n\n");
//	while (buff[ixj] != '\0') {
//		int j = (int) buff[ixj];
//		printf("ascii: %d\n", j);
//		ixj++;
//	}
//	printf("whats in side. ***********\n\n");
	
	char cmd[255] = {'\0'};
	char param[255] = {'\0'};
	int paramPart = 0;
	int i = 0;
	int cmdI = 0;
	int paramI = 0;
	while (buff[i] != '\0') {
		int buf = (int) buff[i];
		if (buf == 13 || buf == 10) {
			break;
		}
		if (buff[i] == ' ' && !paramPart) {
			if (buff[i+1] != '\0' && buff[i+1] != ' ') {
				paramPart = 1;
			}
			i++;
			continue;
		}
		if (!paramPart) {
			cmd[cmdI] = toupper(buff[i]);
			cmdI++;
		} else {
			param[paramI] = buff[i];
			paramI++;
		}
		i++;
	}
//	printf("cmd: %s\n", cmd);
//	printf("cmd len: %lu\n", strlen(cmd));
//	printf("param: %s\n", param);
//	printf("param len: %lu\n", strlen(param));
	strcpy(ftp_cmd.args, trim_str(param, (int) strlen(param)));
//	printf("ftp_cmp.args: %s\n", ftp_cmd.args);
//	printf("ftp_cmp.args len: %lu\n", strlen(ftp_cmd.args));
	
	if (strcmp(cmd, "QUIT") == 0) {
		ftp_cmd.cmd = QUIT;
	} else if (strcmp(cmd, "USER") == 0) {
		ftp_cmd.cmd = USER;
	} else if (strcmp(cmd, "CWD") == 0) {
		ftp_cmd.cmd = CWD;
	} else if (strcmp(cmd, "CDUP") == 0) {
		ftp_cmd.cmd = CDUP;
	} else if (strcmp(cmd, "TYPE") == 0) {
		ftp_cmd.cmd = TYPE;
	} else if (strcmp(cmd, "MODE") == 0) {
		ftp_cmd.cmd = MODE;
	} else if (strcmp(cmd, "STRU") == 0) {
		ftp_cmd.cmd = STRU;
	} else if (strcmp(cmd, "RETR") == 0) {
		ftp_cmd.cmd = RETR;
	} else if (strcmp(cmd, "PASV") == 0) {
		ftp_cmd.cmd = PASV;
	} else if (strcmp(cmd, "NLST") == 0) {
		ftp_cmd.cmd = NLST;
	}
	
	
	return ftp_cmd;
}