#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ftptype.h"

ftp_type_t change_type(char* buff) {
	ftp_type_t default_type;
	default_type.type = INVALID_TYPE;
	strcpy(default_type.name, "0");
	
	if (strlen(buff) == 1) {
		ftp_type_t new_type;
		char target_type = tolower(buff[0]);
		switch (target_type) {
			case 'a':
				new_type.type = ASCII;
				strcpy(new_type.name, "ASCII");
				break;
			case 'i':
				new_type.type = BINARY;
				strcpy(new_type.name, "BINARY");
				break;
			default:
				new_type = default_type;
		}
		return new_type;
	}
	return default_type;
}