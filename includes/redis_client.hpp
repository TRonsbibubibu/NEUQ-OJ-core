//
// Created by trons on 16-10-11.
//

#ifndef JUDGE_CORE_REDIS_CLIENT_H
#define JUDGE_CORE_REDIS_CLIENT_H

#include <atomic>
#include <iostream>
#include "redis_client.hpp"
#include <cpp_redis/cpp_redis>

namespace judge{

class redis_client {

public:
    redis_client(void);

public:
    void init(const std::string& host = "127.0.0.1", unsigned int port = 6379);
    const std::string getOneSolution(const std::string& key);
    void disconnect();

private:
    cpp_redis::redis_client client;
};

}


#endif //JUDGE_CORE_REDIS_CLIENT_H
