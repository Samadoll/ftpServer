#include <stdio.h>
#include <string.h>
#include "ftpmode.h"

ftp_mode_t change_mode(char* buff) {
	ftp_mode_t defaultMode;
	defaultMode.mode = INVALID_MODE;
	defaultMode.name = '0';
	if (strlen(buff) == 1) {
		ftp_mode_t new_mode;
		switch (buff[0]) {
			case 's':
			case 'S':
				new_mode.mode = STREAM;
				new_mode.name = 'S';
				return new_mode;
			case 'b':
			case 'B':
				new_mode.mode = BLOCK;
				new_mode.name = 'B';
				return new_mode;
			case 'c':
			case 'C':
				new_mode.mode = COMPRESSED;
				new_mode.name = 'C';
				return new_mode;
		}
	}
	return defaultMode;
}