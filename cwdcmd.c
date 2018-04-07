#include <stdio.h>
#include <string.h>
#include "cwdcmd.h"
#include <unistd.h>

//int starting_str(char* src, char* dst) {
//	int i = 0;
//	int flag = 0;
//	while(src[i] != '\0' && dst[i] != '\0') {
//		if (i == 0 && src[i] == dst[i]) {
//			flag = 1;
//		} else if (flag && src[i] != dst[i]) {
//			flag = 0;
//		}
//		i++;
//	}
//	return flag;
//}

int starting_str(char* src, char* dst) {
	int len = (int) strlen(dst);
	char temp[255] = {'\0'};
	int i = 0;
	while(i < len) {
		temp[i] = src[i];
		i++;
	}
	if (strcmp(temp, dst) == 0) {
		return 1;
	} else {
		return 0;
	}
}

int contains_str(char* src, char* dst) {
	int flag = 0;
	int i = 0;
	int j = 0;
	int srclen = (int) strlen(src);
	int dstlen = (int) strlen(dst);
	if (srclen < dstlen) {
		return 0;
	}
	while (src[i] != '\0') {
		if (!flag && (srclen - i) < dstlen) {
			return 0;
		} else if (src[i] == dst[j]) {
			flag = 1;
			j++;
		} else if (src[i] != dst[j]) {
			j = 0;
			flag = 0;
		}
		if (dst[j] == '\0' && flag) {
			return 1;
		}
		i++;
	}
	return 1;
}

int valid_dir(char* buff) {
	if (starting_str(buff, "./") || contains_str(buff, "../")) {
		return 0;
	}
	return 1;
}

int ftp_cwd(char* buff) {
	int flag = -1;
	if (valid_dir(buff)) {
		flag = chdir(buff);
	}
	return flag;
}