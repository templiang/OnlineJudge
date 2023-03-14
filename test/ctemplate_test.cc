/*
    国内镜像源  git clone https://hub.nuaa.cf/OlafvdSpek/ctemplate.git
    sudo yum update
    sudo yum install autoconf automake libtool
    ./autogen.sh && ./configure
    sudo make install

    报错解决：Error: Failed to download metadata for repo 'appstream': Cannot prepare internal mirrorlist: No URLs in mirrorlist
    sudo sed -i -e "s|mirrorlist=|#mirrorlist=|g" /etc/yum.repos.d/CentOS-*
    sudo sed -i -e "s|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g" /etc/yum.repos.d/CentOS-*
*/

// 渲染功能：将数据与网页合并
#include <iostream>
#include <string>
#include <ctemplate/template.h>


int main(){
    const std::string inHtml = "./test.html";
    std::string value1 = "hello";
    std::string value2 = "world";

    // 形成数据字典
    ctemplate::TemplateDictionary root("test");  //类似于unordered_map<> test;
    root.SetValue("key1",value1); //类似于test.insert({"key1",value1})
    root.SetValue("key2",value2);

    // 获取被渲染网页对象
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(inHtml,ctemplate::DO_NOT_STRIP);

    //添加字典数据到网页中
    std::string outHtml;
    tpl->Expand(&outHtml,&root);

    std::cout<<outHtml<<std::endl;
}