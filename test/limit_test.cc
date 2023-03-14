//用以测试资源约束函数
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
void handler(int signo){
    std::cout<<signo<<std::endl;
    exit(1);
}
int main(){
    //资源不足，导致OS终止进程，是通过信号终止的
    
    //测试：分别测试不同资源限制下进程终止时收到的终止信号

    for(int i=0;i<32;++i){
        //注册所有信号
        signal(i,handler);
    }

    // 1. 限制累计运行时长不超过1秒
    // RLIMIT_CPU: This  is  a  limit, in seconds, on the amount of CPU time that the process can consume
    // struct rlimit _limit;
    // _limit.rlim_cur = 1;
    // _limit.rlim_max =RLIM_INFINITY;
    // setrlimit(RLIMIT_CPU,&_limit);
    // while(1);

    // 2.限制内存使用(单位byte)
    // struct rlimit _limit;
    // _limit.rlim_cur = 1024 * 1024 *40;//40M
    // _limit.rlim_max =RLIM_INFINITY;
    // setrlimit(RLIMIT_AS,&_limit);

    // int count = 0;
    // while(1){
    //     int *p =new int[1024*1024];
    //     ++count;
    //     std::cout<<count<<std::endl;
    //     sleep(1);
    // }
    return 0;
}