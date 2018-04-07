#ifndef _FTPMODE__
#define _FTPMODE__

#include <stdio.h>
#include <string.h>

typedef enum {
	STREAM,
	BLOCK,
	COMPRESSED,
	INVALID_MODE
} ftp_mode_t_t;

typedef struct ftp_mode {
	ftp_mode_t_t mode;
	char name;
} ftp_mode_t;

ftp_mode_t change_mode(char* buff);

#endif