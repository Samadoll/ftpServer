#ifndef _FTPTYPE__
#define _FTPTYPE__

#include <stdio.h>
#include <string.h>

typedef enum {
	ASCII,
	BINARY,
	INVALID_TYPE
} ftp_type_t_t;

typedef struct ftp_type {
	ftp_type_t_t type;
	char name[10];
} ftp_type_t;

ftp_type_t change_type(char* buff);

#endif