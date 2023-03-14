#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../util/Util.hpp"
#include "../common/Log.hpp"
using namespace LogNameSpace;
using namespace UtilNameSpace;

namespace RunnerNameSpace
{
    class Runner
    {
    public:
        Runner()
        {
        }
        ~Runner() {}
        
        static bool setResourceLimit(int cpu,int mem){
            struct rlimit _cpu,_mem;
            // cpu资源限制
            _cpu.rlim_cur =cpu;
            _cpu.rlim_max =RLIM_INFINITY;
            // 内存资源限制
            _mem.rlim_cur=mem*1024*1024;
            _mem.rlim_max=RLIM_INFINITY;

            if(setrlimit(RLIMIT_CPU,&_cpu) < 0 
                || setrlimit(RLIMIT_AS,&_mem)< 0){
                //如果限制资源失败
                return false;
            }
            return true;
        }
        /*
            运行有以下几种情况：
            1.代码运行完成,结果正确
            2.代码运行完成,结果错误
            3.代码运行异常
            Run提供基础运行功能，只考虑是否能够正常运行。并不判断运行结果正确与否，结果是否正确交由测试用例决定

        */
        /*
            executeName 可执行程序名
            cpuLimit 允许使用的最大CPU资源。单位cpu占用时间(秒)
            memoryLimit 允许使用的最大内存资源。单位(Mb)
            return:
            0 正常运行
            >0 程序运行时异常，返回值为退出信号
            <0 Run本身内部错误
        */
        static int Run(const std::string &executeName,int cpuLimit,int memoryLimit)
        {
            /*
                程序在启动时默认打开标准输入输出流，用以提供输入、输出、错误输出。在此三个文件需要我们自己创建提供
                并由子进程继承
            */
            umask(0);
            int stdinFd = open(FileUtil::Stdin(executeName).c_str(), O_CREAT | O_RDONLY, 0644);
            int stdoutFd = open(FileUtil::Stdout(executeName).c_str(),O_CREAT | O_WRONLY,0644);
            int stderrFd = open(FileUtil::Stderr(executeName).c_str(),O_CREAT | O_WRONLY,0644);
            
            if(stdinFd < 0 || stdoutFd < 0 || stderrFd <0){
                LOG(ERROR)<<"open std stream failed"<<std::endl;
                return -1;
            }

            pid_t pid = fork();
            
            if (pid < 0)
            {
                LOG(ERROR) << "run failure" << std::endl;
                return -2;
            }

            if (pid == 0)
            {
                if(setResourceLimit(cpuLimit,memoryLimit) == false){
                    LOG(ERROR) << "set resource limit error" << std::endl;
                    exit(-4);
                }
                //将stdin/out/err重定向到我们自己创建的文件
                dup2(stdinFd,0);
                dup2(stdoutFd,1);
                dup2(stderrFd,2);

                execl(FileUtil::ExeFormat(executeName).c_str(), FileUtil::ExeFormat(executeName).c_str(),nullptr);

                // 替换成功后绝不会到此
                LOG(ERROR) << "execl failed" << std::endl;
                exit(-3);
            }
            else
            {   
                //记得关闭文件
                close(stdinFd);
                close(stdoutFd);
                close(stderrFd);

                int status;
                waitpid(pid, &status, 0);
                //无论对错或是异常，到此运行完成

                LOG(INFO)<<"run success.status:"<< (status & 0X7F)<<std::endl;

                //status位图只使用 低16比特位，高16比特位不用
                //若子进程正常退出  [15...8|7...0]  ->低八位为0，高八位存储退出状态
                //若子进程异常退出  [[15...8|7|6...0]] -->低七位保存终止信号，第8位标志是否core dump，高八位未使用
                //status & 0X7F  若正常退出则为0，若异常退出则为终止信号
                return (status & 0X7F);

            }



        }
    };
}