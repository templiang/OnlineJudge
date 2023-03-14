#include <iostream>
#include "../common/Log.hpp"
#include "../common/httplib.h"
#include "OJControl.hpp"

using namespace LogNameSpace;
using namespace httplib;
using namespace ControlNameSpace;

Control ctrl;

void Repair(int signo){
    ctrl.ReonlineWorker();
}

int main(){
    /*
        用户请求的服务路由功能
        1. 获取所有题目列表
        2. 根据题目编号获取题目内容
    */

    signal(3,Repair); // 捕捉3号信号(ctrl+\)，重定义用以重新上线编译主机
    Server svr;
    //Control ctrl;
    //*ctrl_ptr = ctrl;

    svr.set_base_dir("./wwwroot");// 设置首页根目录

    // 获取题目列表
    svr.Get("/AllQuestions",[&](const Request &req,Response &res){
        // 返回包含所有题目的html网页
        std::string html;
        ctrl.AllQuestions(html);

        res.set_content(html,"text/html;charset=utf-8");
    });

    // 根据题目编号获取题目内容
    // d+正则表达式，匹配多个数字  --> question/101
    svr.Get(R"(/Question/(\d+))",[&](const Request &req,Response &res){
        std::string number = req.matches[1];
        std::string html;
        ctrl.Question(number,html);
        res.set_content(html,"text/html;charset=utf-8");
    });

    // 用户提交代码，请求判题
    svr.Post(R"(/Judge/(\d+))",[&](const Request &req,Response &res){
        
        std::string number = req.matches[1];
        std::string reqJson =req.body;
        std::string resJson;

        ctrl.Judge(number,reqJson,resJson);
        res.set_content(resJson,"application/json;charset=utf-8");
    });

    svr.Post("/Register",[&](const Request &req,Response &res){
        std::string html;
        FileUtil::ReadFile("./template_html/Login.html",html,true);
        res.set_content("测试","application/json;charset=utf-8");
    });

    svr.Get("/Login",[&](const Request &req,Response &res){
        std::string html;
        FileUtil::ReadFile("./template_html/Login.html",html,true);
        res.set_content(html,"text/html;charset=utf-8");
    });

    svr.listen("0.0.0.0",8080);
    return 0;
}