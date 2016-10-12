#include <iostream>
#include <cstring>
#include "daemon.hpp"

int main(int argc, char** argv) {
    //TODO separate the mysql client
    //TODO set the oj_home in the judge.conf
    //TODO clear the unuse var
    //TODO redis client
    //TODO modify the judge_client sql
    DEBUG = (argc > 2);
    ONCE = (argc > 3);
    if (argc > 1)
        strcpy(oj_home, argv[1]);
    else
        strcpy(oj_home, "/home/judge");
    chdir(oj_home);    // change the dir

    sprintf(lock_file,"%s/etc/judge.pid",oj_home);
    judge::daemon daemon1;
    if (!DEBUG)
        daemon1.init();
    if ( daemon1.already_running()) {
        syslog(LOG_ERR | LOG_DAEMON,
               "This daemon program is already running!\n");
        printf("%s already has one judged on it!\n",oj_home);
        return 1;
    }
    if(!DEBUG)
        system("/sbin/iptables -A OUTPUT -m owner --uid-owner judge -j DROP");
//	struct timespec final_sleep;
//	final_sleep.tv_sec=0;
//	final_sleep.tv_nsec=500000000;
    daemon1.init_mysql_conf();	// set the database info
    signal(SIGQUIT, judge::daemon::call_for_exit);
    signal(SIGKILL, judge::daemon::call_for_exit);
    signal(SIGTERM, judge::daemon::call_for_exit);
    int j = 1;
    while (1) {			// start to run
        while (j && (http_judge || !daemon1.init_mysql())) {

            j = daemon1.work();

        }
        if(ONCE) break;
        sleep(sleep_time);
        j = 1;
    }
    return 0;
}

