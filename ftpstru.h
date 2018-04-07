#ifndef _FTPSTRU__
#define _FTPSTRU__

#include <stdio.h>
#include <string.h>

typedef enum {
	FILE_STRU,
	RECORD_STRU,
	PAGE_STRU,
	INVALID_STRU
} ftp_stru_t_t;

typedef struct ftp_stru {
	ftp_stru_t_t stru;
	char name[10];
} ftp_stru_t;

ftp_stru_t change_stru(char* buff);

#endif