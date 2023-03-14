#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../util/Util.hpp"
#include "../common/Log.hpp"
using namespace UtilNameSpace;
using namespace LogNameSpace;
//编译功能模块，只负责代码的编译工作
//流程:cpp文件 --> 编译生成 

namespace CompilerNameSpace{
    class Compiler{
    public:
        Compiler(){
        }
        ~Compiler(){
        }
        //传入参数：
        //srcFileName:需要编译的文件名
        //功能:编译，结果放入临时文件夹下./temp/
        //生成
        static bool Compile(const std::string &srcFileName){
            int pid = fork();
            if(pid<0){
                LOG(ERROR)<<"compile failed because fork failure"<<std::endl;
                exit(1);
            }
            if(pid==0){
                //子进程,进行程序替换，调用编译器执行编译操作
                umask(0);
                //创建.errlog文件，用以记录可能出现的编译报错信息
                int errFd = open(FileUtil::CompileErrFormat(srcFileName).c_str(),O_WRONLY | O_CREAT, 0644);
                if(errFd<0){
                    LOG(WARING)<<"create errlog file failed"<<std::endl;
                    exit(2);
                }
                //将标准错误stderr重定向到errfd
                dup2(errFd,2);
                //g++ -o target srcfile -std=c++11
                //execlp("要执行谁","怎么执行",...参数,nullptr)  ls -a --> execlp("ls","ls","-a",nullptr);
                execlp("g++","g++","-o",\
                    FileUtil::ExeFormat(srcFileName).c_str(),\
                    FileUtil::SrcFormat(srcFileName).c_str(),\
                    "-std=c++11","-DCOMPILE_ONLINE",nullptr);


                //程序替换后，不会执行到此
                LOG(ERROR)<<"execlp failed"<<std::endl;
                exit(3);
            }
            else{
                //父进程。负责回收子进程，判断是否编译成功
                //waitpid(id,NULL,0);//等待指定的一个进程
                waitpid(pid,nullptr,0);
                if(FileUtil::IsExist(FileUtil::ExeFormat(srcFileName))){
                    //exe文件存在，编译成功
                    LOG(INFO)<<"compile sucess"<<std::endl;
                    return true;
                }
            }

            //
            LOG(ERROR)<<"compile failed"<<std::endl;
            return false;
        }
    };
}