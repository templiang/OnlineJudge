// It's extremely easy to setup. Just include the httplib.h file in your code!

#include <iostream>
//#include "../common/httplib.h"
#include "../lib/cpp-httplib/httplib.h"
using namespace httplib;

int main(){
    httplib::Server svr;
    // 注册 get方法   /hello
    svr.Get("/hello",[](const httplib::Request &req,httplib::Response &res){
        res.set_content("Hello World!", "text/plain;charset=utf-8");
    });
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}