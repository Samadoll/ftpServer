#include <stdio.h>
#include "login.h"

int ftp_login(char* buff) {
	if (strcmp(buff, "cs317") == 0) {
		return 1;
	}
	return 0;
}