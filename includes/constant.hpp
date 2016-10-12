//
// Created by trons on 16-10-12.
//

#ifndef JUDGE_CORE_CONSTANT_H
#define JUDGE_CORE_CONSTANT_H

#include <mysql/mysql.h>

#define BUFFER_SIZE 1024
#define LOCKFILE "/var/run/judged.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define STD_MB 1048576

#define OJ_WT0 0
#define OJ_WT1 1
#define OJ_CI 2
#define OJ_RI 3
#define OJ_AC 4
#define OJ_PE 5
#define OJ_WA 6
#define OJ_TL 7
#define OJ_ML 8
#define OJ_OL 9
#define OJ_RE 10
#define OJ_CE 11
#define OJ_CO 12
static char lock_file[BUFFER_SIZE]=LOCKFILE;
static char host_name[BUFFER_SIZE];
static char user_name[BUFFER_SIZE];
static char password[BUFFER_SIZE];
static char db_name[BUFFER_SIZE];
static char oj_home[BUFFER_SIZE];
static char oj_lang_set[BUFFER_SIZE];
static int port_number;
static int max_running;
static int sleep_time;
static int sleep_tmp;
static int oj_tot;
static int oj_mod;
static int http_judge = 0;
static char http_baseurl[BUFFER_SIZE];
static char http_username[BUFFER_SIZE];
static char http_password[BUFFER_SIZE];

static bool STOP = false;
static int DEBUG = 1;
static int ONCE = 0;
static MYSQL *conn;
static MYSQL_RES *res;
static MYSQL_ROW row;
//static FILE *fp_log;
static char query[BUFFER_SIZE];
#endif //JUDGE_CORE_CONSTANT_H
