#ifndef _DECRYPY_SOURCE_FILE
#define _DECRIPT_SOURCE_FILE
#include <Python.h>
#include <math.h>

#include <openssl/aes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define KEY  "8cc72b05705d5c46f412af8cbed55aad"
#define IV   "667b02a85c61c786def4521b060265e8"
#define HEADINFO "encrypted-python-source-file-header"
#define _NO_DECRYPT_FILE_OUTPUT

FILE* d_open(char *filename, const char *modes);
int   dopen(const char *pathname, int flags, mode_t mode);

#endif