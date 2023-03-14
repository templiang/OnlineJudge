/*
    此为文件版Model
*/
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <assert.h>
#include "../common/Log.hpp"
#include "../util/Util.hpp"
#include "../common/Log.hpp"
using namespace LogNameSpace;
using namespace UtilNameSpace;
using namespace LogNameSpace;
/*
    根据题目list文件，加载所有题目的信息到内存中
*/
namespace ModelNameSpace
{
    struct Question_t{
        std::string number; //题目编号
        std::string title; //题目标题
        std::string star;//难度
        std::string desc;//题目描述
        std::string header;//预设编译器代码
        std::string tail;//测试用例，需与header拼接形成完整代码
        int cpuLimit;
        int memLimit;
    };

    const std::string questionListPath = "./oj_questions/Questions.list";
    const std::string questionPath = "./oj_questions/";

    // Model:主要用来和数据进行交互，对外提供访问数据的接口
    class Model
    {
    protected:
        std::unordered_map<std::string,Question_t> NumberToQuestion;
    public:
        Model(){
            if(!LoadQuestions(questionListPath)){
                assert(0);
            }
        }
        ~Model(){

        }
        
        // 提供给外界的接口
        // 获取所有题目
        // questions:输出型参数
        bool GetAllQuestions(std::vector<Question_t> &questions){
            
            questions.clear();

            if(NumberToQuestion.size() == 0){
                return false;
            }
            for(const auto &e : NumberToQuestion){
                questions.push_back(e.second);
            }
            return true;
        }

        // 通过编号题目获取完整题目信息
        bool GetQuestionByNum(const std::string &number,Question_t &question){
            auto it = NumberToQuestion.find(number);
            if(it == NumberToQuestion.end()){
                //map中无此key
                return false;
            }
            question = it->second;
            return true;

        }

    protected:
        // 加载配置文件question.list，并加载所有题目文件
        bool LoadQuestions(const std::string &questionListPath){
            std::ifstream ifs(questionListPath);
            std::string line;

            if(!ifs.is_open()){
                LOG(ERROR)<<"题目加载失败，请检查是否存在Question.list文件"<<std::endl;
                return false;
            }

            while(getline(ifs,line)){
                // line -> 1 回文数 简单 1 20 
                std::vector<std::string> res;
                StringUtil::SplitString(line,res,' ');

                if(res.size()!=5){
                    //该行信息有误
                    LOG(WARNING)<<"加载部分题目出现错误"<<std::endl;
                    continue;
                }

                Question_t q;
                q.number = res[0];
                q.title = res[1];
                q.star = res[2];
                q.cpuLimit = atoi(res[3].c_str());
                q.memLimit = atoi(res[4].c_str());
                // q.header
                FileUtil::ReadFile(questionPath+q.number+"/header.cc",q.header,true);
                // q.desc
                FileUtil::ReadFile(questionPath+q.number+"/desc.txt",q.desc,true);
                // q.tail;
                FileUtil::ReadFile(questionPath+q.number+"/tail.cc",q.tail,true);

                NumberToQuestion.insert(std::make_pair(q.number,q));
                //NumberToQuestion.insert({q.number,q});
            }

            ifs.close();
            LOG(INFO)<<"加载题库成功,共加载题目数量:"<<NumberToQuestion.size()<<std::endl;
            return true;
        }
    

    };

}