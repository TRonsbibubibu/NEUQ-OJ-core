//
// Created by trons on 16-10-12.
//

#ifndef JUDGE_CORE_DAWMON_H
#define JUDGE_CORE_DAWMON_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <mysql/mysql.h>
#include <wait.h>
#include <sys/resource.h>
#include "constant.hpp"
#include "utils.hpp"
namespace judge{

class daemon {
public:
    daemon(void);

public:
    int init(void);
    void run_client(int runid, int clientid);

    int _get_jobs_mysql(int * jobs);
    int init_mysql();
    int executesql(const char * sql);
    int get_jobs(int * jobs);
    bool check_out(int solution_id, int result);
    bool _check_out_mysql(int solution_id, int result);
    void init_mysql_conf();
    int work();

    int lockfile(int fd);
    int already_running();
    static void call_for_exit(int s);
};
}


#endif //JUDGE_CORE_DAWMON_H
