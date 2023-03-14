#include <iostream>
#include <json/json.h>
#include <memory>
#include <sstream>

int main(){

    // 序列化
    Json::Value root;
    std::ostringstream os;
    std::string s;
    
    root["code"] = "中文测试";
    root["user"] = "ang";
    root["id"] = "123456";
    
    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(root,&os);
    s = os.str();

    printf("%s\n",s.c_str());

    // 反序列化
    JSONCPP_STRING errs;
    Json::Value root_;
    Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    jsonReader->parse(s.c_str(), s.c_str()+s.length(), &root_, &errs);

    std::cout << "code " << root_["code"].asString() << std::endl;
    std::cout << "user " << root_["user"].asString() << std::endl;
    std::cout << "user " << root_["id"].asString() << std::endl;
    
    
    return 0;
}