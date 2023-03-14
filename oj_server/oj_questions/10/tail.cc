// 为了测试时不报错
#ifndef COMPILE_ONLINE
#include "./header.cc"
#endif

void Test1(){
    std::vector<int> test= {1,2,3,4,5,6};
    Solution().Max(test);
}

void Test2(){

}

int main()
{
    Test1();
    Test2();

    return 0;
}