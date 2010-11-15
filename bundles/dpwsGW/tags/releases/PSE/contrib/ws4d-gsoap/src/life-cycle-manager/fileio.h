#include <stdio.h>
#include <stdlib.h>
int readfile(const char* filename, unsigned char* bytes);
int writefile(const char* filename, unsigned char* bytes, size_t len);
int getFileCrc (const char *filenameinzip, void *buf, unsigned long size_buf,
                unsigned long *result_crc);

