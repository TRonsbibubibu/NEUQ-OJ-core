//
// Created by trons on 16-10-12.
//

#ifndef JUDGE_CORE_UTILS_H
#define JUDGE_CORE_UTILS_H

#include <stdlib.h>
#include "constant.hpp"
#include "string.h"
#include "ctype.h"
#include "stdio.h"

namespace judge{
    class utils {
public:
     static void trim(char *c);
     static bool read_buf(char *buf, const char *key, char *value);
     static void read_int(char * buf, const char * key, int * value);
     static int after_equal(char * c);
     static void write_log(const char *fmt, ...);
};
}


#endif //JUDGE_CORE_UTILS_H
