#include <iostream>
#include "../common/httplib.h"
// #include "../lib/cpp-httplib/httplib.h"
#include "CompileAndRun.hpp"
using namespace CARNameSpace;
using namespace httplib;
// CAR -> compile and run

void Usage(){
    std::cerr<<"Usage:./CARServer port"<<std::endl;
}
int main(int argc,char **argv){

    if(argc!=2){
        Usage();
        return -1;
    }

    int _port = atoi(argv[1]);


    httplib::Server svr;
    // 注册 get方法   /hello
    svr.Get("/hello",[](const httplib::Request &req,httplib::Response &res){
        res.set_content("Hello World!", "text/plain;charset=utf-8");
    });

    svr.Post("/CAR",[](const Request &req ,Response &res){
        // 获取请求正文
        LOG(INFO)<<"收到post请求"<<std::endl;
        std::string reqJson = req.body;//正文中即input json串
        std::string resJson;
        
        CAR::Start(reqJson,resJson);
        LOG(INFO)<<"CAR模块运行完成"<<std::endl;
        res.set_content(resJson,"application/json;charset=utf-8");//

    });
    
    svr.listen("0.0.0.0", _port);
    return 0;
}