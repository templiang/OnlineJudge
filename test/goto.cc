#include <iostream>

int main(){

    if(true){
        goto END;
    }
    std::cout<<"111111111"<<"\n";
    END:
        std::cout<<"execute goto"<<"\n";

}