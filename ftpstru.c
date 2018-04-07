#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ftpstru.h"

ftp_stru_t change_stru(char* buff) {
	ftp_stru_t default_stru;
	default_stru.stru = INVALID_STRU;
	strcpy(default_stru.name, "0");
	
	if (strlen(buff) == 1) {
		ftp_stru_t new_stru;
		char target_stru = tolower(buff[0]);
		switch (target_stru) {
			case 'f':
				new_stru.stru = FILE_STRU;
				strcpy(new_stru.name, "FILE");
				break;
			case 'r':
				new_stru.stru = RECORD_STRU;
				strcpy(new_stru.name, "RECORD");
				break;
			case 'p':
				new_stru.stru = PAGE_STRU;
				strcpy(new_stru.name, "PAGE");
				break;
			default:
				new_stru = default_stru;
		}
		return new_stru;
	}
	return default_stru;
}