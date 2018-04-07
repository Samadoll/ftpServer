#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ftpcdup.h"

int valid_dir_cdup(char* curr, char* init) {
	if (strcmp(curr, init) == 0) {
		return 0;
	}
	return 1;
}

int ftp_cdup(char* curr, char* init) {
	int flag = -1;
	if (valid_dir_cdup(curr, init)) {
		flag = chdir("..");
	}
	return flag;	
}