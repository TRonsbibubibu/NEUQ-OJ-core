//
// Created by trons on 16-10-11.
//
#include <iostream>
#include <cstring>
#include "redis_client.hpp"
#include "constant.hpp"

namespace judge{

    redis_client::redis_client(){
    }

    void
    redis_client::init(const std::string& host, unsigned int port){
        std::cout<<"=======init begin======="<<std::endl;
        client.connect(host, port, [](cpp_redis::redis_client &){
            std::cout << "client disconnected (disconnection handler)" << std::endl;
        });
    }

    const std::string
    redis_client::getOneSolution(const std::string& key) {
        const std::vector<std::string> vectorKey{key};
        char* i = new char[BUFFER_SIZE];
        client.blpop(vectorKey, 0, [i](cpp_redis::reply &reply){
            strcpy(i, reply.as_array()[1].as_string().c_str());
        }).sync_commit();
        return std::string(i);
    }

    void
    redis_client::disconnect() {
        client.disconnect();
    }
}