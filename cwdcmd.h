#ifndef _CWDCMD__
#define _CWDCMD__

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int starting_str(char* src, char* dst);

int contains_str(char* src, char* dst);

int ftp_cwd(char* buff);

#endif