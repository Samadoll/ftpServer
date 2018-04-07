#ifndef _PARSER__
#define _PARSER__

#include <stdio.h>
#include <string.h>

typedef enum {
	USER,
	QUIT,
	CWD,
	CDUP,
	TYPE,
	MODE,
	STRU,
	RETR,
	PASV,
	NLST,
	INVALID
} CMD;

typedef struct ftp_cmd {
	CMD cmd;
	char args[255];
} ftp_cmd_t;

ftp_cmd_t parse_buff(char* buff);

#endif