//
// Created by trons on 16-10-12.
//
#include "utils.hpp"
#include <stdarg.h>

namespace judge{
    void utils::trim(char *c) {
        char buf[BUFFER_SIZE];
        char * start, *end;
        strcpy(buf, c);
        start = buf;
        while (isspace(*start))
            start++;
        end = start;
        while (!isspace(*end))
            end++;
        *end = '\0';
        strcpy(c, start);
    }

    bool utils::read_buf(char *buf, const char *key, char *value){
        if (strncmp(buf, key, strlen(key)) == 0) {
            strcpy(value, buf + judge::utils::after_equal(buf));
            judge::utils::trim(value);
            if (DEBUG)
                printf("%s\n", value);
            return 1;
        }
        return 0;
    };

    void utils::read_int(char * buf, const char * key, int * value){
        char buf2[BUFFER_SIZE];
        if (read_buf(buf, key, buf2))
            sscanf(buf2, "%d", value);
    }

    int utils::after_equal(char * c) {
        int i = 0;
        for (; c[i] != '\0' && c[i] != '='; i++)
            ;
        return ++i;
    }

    void utils::write_log(const char *fmt, ...) {
        va_list ap;
        char buffer[4096];
//	time_t          t = time(NULL);
//	int             l;
        sprintf(buffer, "%s/log/client.log", "/home/judge");
        FILE *fp = fopen(buffer, "ae+");
        if (fp == NULL) {
            fprintf(stderr, "openfile error!\n");
            system("pwd");
        }
        va_start(ap, fmt);
        vsprintf(buffer, fmt, ap);
        fprintf(fp, "%s\n", buffer);
        if (DEBUG)
            printf("%s\n", buffer);
        va_end(ap);
        fclose(fp);
    }
}