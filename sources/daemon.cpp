//
// Created by trons on 16-10-12.
//

#include "daemon.hpp"

namespace judge{
    daemon::daemon() {

    }

    int daemon::init() {
        pid_t pid;

        if ((pid = fork()) < 0)
            return (-1);

        else if (pid != 0)
            exit(0); /* parent exit */

        /* child continues */

        setsid(); /* become session leader */

        chdir(oj_home); /* change working directory */

        umask(0); /* clear file mode creation mask */

        close(0); /* close stdin */
        close(1); /* close stdout */

        close(2); /* close stderr */

        int fd = open( "/dev/null", O_RDWR );
        dup2( fd, 0 );
        dup2( fd, 1 );
        dup2( fd, 2 );
        if ( fd > 2 ){
            close( fd );
        }

        return (0);
    }

    int daemon::lockfile(int fd) {
        struct flock fl;
        fl.l_type = F_WRLCK;
        fl.l_start = 0;
        fl.l_whence = SEEK_SET;
        fl.l_len = 0;
        return (fcntl(fd, F_SETLK, &fl));
    }

    void daemon::run_client(int runid, int clientid) {
        char buf[BUFFER_SIZE], runidstr[BUFFER_SIZE];
        struct rlimit LIM;
        LIM.rlim_max = 800;
        LIM.rlim_cur = 800;
        setrlimit(RLIMIT_CPU, &LIM);

        LIM.rlim_max = 80 * STD_MB;
        LIM.rlim_cur = 80 * STD_MB;
        setrlimit(RLIMIT_FSIZE, &LIM);

        LIM.rlim_max = STD_MB << 11;
        LIM.rlim_cur = STD_MB << 11;
        setrlimit(RLIMIT_AS, &LIM);

        LIM.rlim_cur = LIM.rlim_max = 200;
        setrlimit(RLIMIT_NPROC, &LIM);

        //buf[0]=clientid+'0'; buf[1]=0;
        sprintf(runidstr, "%d", runid);
        sprintf(buf, "%d", clientid);

        //write_log("sid=%s\tclient=%s\toj_home=%s\n",runidstr,buf,oj_home);
        //sprintf(err,"%s/run%d/error.out",oj_home,clientid);
        //freopen(err,"a+",stderr);

        if (!DEBUG)
            execl("/usr/bin/judge_client", "/usr/bin/judge_client", runidstr, buf,
                  oj_home, (char *) NULL);
        else
            execl("/usr/bin/judge_client", "/usr/bin/judge_client", runidstr, buf,
                  oj_home, "debug", (char *) NULL);

        //exit(0);
    }

    int daemon::executesql(const char * sql) {
        if (mysql_real_query(conn, sql, strlen(sql))) {
            if (DEBUG)
                judge::utils::write_log("%s", mysql_error(conn));
            sleep(20);
            conn = NULL;
            return 1;
        } else
            return 0;
    }

    void daemon::init_mysql_conf() {
        FILE *fp = NULL;
        char buf[BUFFER_SIZE];
        host_name[0] = 0;
        user_name[0] = 0;
        password[0] = 0;
        db_name[0] = 0;
        port_number = 3306;
        max_running = 3;
        sleep_time = 1;
        oj_tot = 1;
        oj_mod = 0;
        strcpy(oj_lang_set, "0,1,2,3,4,5,6,7,8,9,10");
        fp = fopen("/home/judge/etc/judge.conf", "r");
        if (fp != NULL) {
            while (fgets(buf, BUFFER_SIZE - 1, fp)) {
                judge::utils::read_buf(buf, "OJ_HOST_NAME", host_name);
                judge::utils::read_buf(buf, "OJ_USER_NAME", user_name);
                judge::utils::read_buf(buf, "OJ_PASSWORD", password);
                judge::utils::read_buf(buf, "OJ_DB_NAME", db_name);
                judge::utils::read_int(buf, "OJ_PORT_NUMBER", &port_number);
                judge::utils::read_int(buf, "OJ_RUNNING", &max_running);
                judge::utils::read_int(buf, "OJ_SLEEP_TIME", &sleep_time);
                judge::utils::read_int(buf, "OJ_TOTAL", &oj_tot);

                judge::utils::read_int(buf, "OJ_MOD", &oj_mod);

                judge::utils::read_int(buf, "OJ_HTTP_JUDGE", &http_judge);
                judge::utils::read_buf(buf, "OJ_HTTP_BASEURL", http_baseurl);
                judge::utils::read_buf(buf, "OJ_HTTP_USERNAME", http_username);
                judge::utils::read_buf(buf, "OJ_HTTP_PASSWORD", http_password);
                judge::utils::read_buf(buf, "OJ_LANG_SET", oj_lang_set);

            }
            sprintf(query,
                    "SELECT solution_id FROM solution WHERE language in (%s) and result<2 and MOD(solution_id,%d)=%d ORDER BY result ASC,solution_id ASC limit %d",
                    oj_lang_set, oj_tot, oj_mod, max_running * 2);
            sleep_tmp = sleep_time;
            //	fclose(fp);
        }
    }

    int daemon::init_mysql() {
        if (conn == NULL) {
            conn = mysql_init(NULL);		// init the database connection
            /* connect the database */
            const char timeout = 30;
            mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

            if (!mysql_real_connect(conn, host_name, user_name, password, db_name,
                                    port_number, 0, 0)) {
                if (DEBUG)
                    judge::utils::write_log("%s", mysql_error(conn));
                sleep(2);
                return 1;
            } else {
                return 0;
            }
        } else {
            return executesql("set names utf8");
        }
    }

    int daemon::_get_jobs_mysql(int * jobs) {
        if (mysql_real_query(conn, query, strlen(query))) {
            if (DEBUG)
                judge::utils::write_log("%s", mysql_error(conn));
            sleep(5);
            return 0;
        }
        res = mysql_store_result(conn);
        int i = 0;
        int ret = 0;
        while ((row = mysql_fetch_row(res)) != NULL) {
            jobs[i++] = atoi(row[0]);
        }
        ret = i;
        while (i <= max_running * 2)
            jobs[i++] = 0;
        return ret;
    }

    int daemon::get_jobs(int * jobs) {
        return daemon::_get_jobs_mysql(jobs);
    }

    int daemon::work() {
        //      char buf[1024];
        static int retcnt = 0;
        int i = 0;
        static pid_t ID[100];
        static int workcnt = 0;
        int runid = 0;
        int jobs[max_running * 2 + 1];
        pid_t tmp_pid = 0;

        //for(i=0;i<max_running;i++){
        //      ID[i]=0;
        //}

        //sleep_time=sleep_tmp;
        /* get the database info */
        if (!get_jobs(jobs))
            retcnt = 0;
        /* exec the submit */
        for (int j = 0; jobs[j] > 0; j++) {
            runid = jobs[j];
            if (runid % oj_tot != oj_mod)
                continue;
            if (DEBUG)
                judge::utils::write_log("Judging solution %d", runid);
            if (workcnt >= max_running) {           // if no more client can running
                tmp_pid = waitpid(-1, NULL, 0);     // wait 4 one child exit
                for (i = 0; i < max_running; i++){     // get the client id
                    if (ID[i] == tmp_pid){
                        workcnt--;
                        retcnt++;
                        ID[i] = 0;
                        break; // got the client id
                    }
                }
            } else {                                             // have free client

                for (i = 0; i < max_running; i++)     // find the client id
                    if (ID[i] == 0)
                        break;    // got the client id
            }
            if(i<max_running){
                if (workcnt < max_running && check_out(runid, OJ_CI)) {
                    workcnt++;
                    ID[i] = fork();                                   // start to fork
                    if (ID[i] == 0) {
                        if (DEBUG)
                            judge::utils::write_log("<<=sid=%d===clientid=%d==>>\n", runid, i);
                        run_client(runid, i);    // if the process is the son, run it
                        exit(0);
                    }

                } else {
                    ID[i] = 0;
                }
            }
        }
        while ((tmp_pid = waitpid(-1, NULL, WNOHANG)) > 0) {
            for (i = 0; i < max_running; i++){     // get the client id
                if (ID[i] == tmp_pid){

                    workcnt--;
                    retcnt++;
                    ID[i] = 0;
                    break; // got the client id
                }
            }
            printf("tmp_pid = %d\n", tmp_pid);
        }
        if (!http_judge) {
            mysql_free_result(res);                         // free the memory
            executesql("commit");
        }
        if (DEBUG && retcnt)
            judge::utils::write_log("<<%ddone!>>", retcnt);
        //free(ID);
        //free(jobs);
        return retcnt;
    }

    bool daemon::check_out(int solution_id, int result) {
        return _check_out_mysql(solution_id, result);
    }

    bool daemon::_check_out_mysql(int solution_id, int result) {
        char sql[BUFFER_SIZE];
        sprintf(sql,
                "UPDATE solution SET result=%d,time=0,memory=0,judgetime=NOW() WHERE solution_id=%d and result<2 LIMIT 1",
                result, solution_id);
        if (mysql_real_query(conn, sql, strlen(sql))) {
            syslog(LOG_ERR | LOG_DAEMON, "%s", mysql_error(conn));
            return false;
        } else {
            if (mysql_affected_rows(conn) > 0ul)
                return true;
            else
                return false;
        }
    }





    int daemon::already_running() {
        int fd;
        char buf[16];
        fd = open(lock_file, O_RDWR | O_CREAT, LOCKMODE);
        if (fd < 0) {
            syslog(LOG_ERR | LOG_DAEMON, "can't open %s: %s", LOCKFILE,
                   strerror(errno));
            exit(1);
        }
        if (lockfile(fd) < 0) {
            if (errno == EACCES || errno == EAGAIN) {
                close(fd);
                return 1;
            }
            syslog(LOG_ERR | LOG_DAEMON, "can't lock %s: %s", LOCKFILE,
                   strerror(errno));
            exit(1);
        }
        ftruncate(fd, 0);
        sprintf(buf, "%d", getpid());
        write(fd, buf, strlen(buf) + 1);
        return (0);
    }

    void daemon::call_for_exit(int s) {
        STOP = true;
        printf("Stopping judged...\n");
    }
}