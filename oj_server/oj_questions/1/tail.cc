// 为了测试时不报错
#ifndef COMPILE_ONLINE
#include "./header.cc"
#endif

#include <iostream>
#include <vector>

bool Test1(){
    std::vector<int> v= {3,2,4,7};
    int target = 6;
    std::vector<int> res = Solution().twoSum(v,target);
    if(res[0] == 1 && res[1] == 2 || res[1] == 1 && res[0] == 2){
        return true;
    }
    return false;
}
// void Test1(){
//     int number = 121;
//     Solution().isPalindrome(number);
// }

bool Test2(){
    std::vector<int> v= {2,7,11,15};
    int target = 9;
    std::vector<int> res = Solution().twoSum(v,target);
    if(res[0] == 1 && res[1] == 2 || res[1] == 1 && res[0] == 2){
        return true;
    }
    return false;
}

bool Test3(){
    std::vector<int> v= {3,3};
    int target = 6;
    std::vector<int> res = Solution().twoSum(v,target);
    if(res[0] == 0 && res[1] == 1 || res[1] == 0 && res[0] == 1){
        return true;
    }
    return false;
}
int main()
{
    if(!Test1()){
        std::cout<<"测试用例1不通过"<<std::endl;
        goto END;
    }
    if(!Test2()){
        std::cout<<"测试用例2不通过"<<std::endl;
        goto END; 
    }
    if(!Test3()){
        std::cout<<"测试用例3不通过"<<std::endl;
        goto END; 
    }
    std::cout<<"通过全部测试用例"<<std::endl;
    END:
    return 0;
}