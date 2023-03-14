#pragma once

#include <stdio.h>
#include <json/json.h>
#include <sstream>
#include <memory>
#include <signal.h>
#include "Compiler.hpp"
#include "Runner.hpp"
#include "../common/Log.hpp"
using namespace CompilerNameSpace;
using namespace RunnerNameSpace;
using namespace LogNameSpace;

namespace CARNameSpace
{
    class CAR
    {
    public:
        // 清理临时文件
        static void CleanTempFile(const std::string &fileName){
            // 源文件
            std::string srcFile = FileUtil::SrcFormat(fileName);
            if(FileUtil::IsExist(srcFile)) unlink(srcFile.c_str());

            // 可执行文件
            std::string exeFile = FileUtil::ExeFormat(fileName);
            if(FileUtil::IsExist(exeFile)) unlink(exeFile.c_str());

            // 编译错误文件
            std::string compileErrFile = FileUtil::CompileErrFormat(fileName);
            if(FileUtil::IsExist(compileErrFile)) unlink(compileErrFile.c_str());

            // 自定义标准输入
            std::string stdinFile = FileUtil::Stdin(fileName);
            if(FileUtil::IsExist(stdinFile)) unlink(stdinFile.c_str());

            // 自定义标准输出
            std::string stdoutFile = FileUtil::Stdout(fileName);
            if(FileUtil::IsExist(stdoutFile)) unlink(stdoutFile.c_str());

            // 自定义标准错误
            std::string stderrFile = FileUtil::Stderr(fileName);
            if(FileUtil::IsExist(stderrFile)) unlink(stderrFile.c_str());
        }
        // 通过状态码获取报错原因
        // statusCode>0 程序收到信号异常终止
        // statusCode<0 非运行报错
        static std::string StatusToInfo(int statusCode,const std::string &fileName)
        {
            std::string info;
            switch (statusCode)
            {
            case 0:
                info = "运行完毕";
                break;
            case -1:
            case -2:
                info = "json解析错误";
                break;
            case -3:
                info = "代码为空";
                break;
            case -4:
                info = "写入错误";
                break;
            case -5:
                // info = "编译错误";
                FileUtil::ReadFile(FileUtil::CompileErrFormat(fileName),info,true);
                break;
            case -10:
                info = "未通过全部测试用例";
                break;
            case SIGABRT: // 6
                info = "内存使用超过限制";
                break;
            case SIGXCPU: // 24
                info = "内存使用超过限制";
                break;
            default:
                info = "未知错误";
                break;
            }

            return info;
        }

        static void Serialization(const Json::Value &value, std::string &str)
        {
            std::ostringstream os;
            Json::StreamWriterBuilder writerBuilder;
            std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
            jsonWriter->write(value, &os);
            str = os.str();
        }
        /*
            输入json key
            1. code:代码 string
            2. input:   string
            3. cpuLimit: int
            4. memLimit: int

            输出json key
            1. status:自定义状态码（int）
            2. reason:原因描述(string)
            3. stdout:运行结果（选填）
            4. stderr:错误（选填）
        */
        static void Start(const std::string &inputJson, std::string &outJson)
        {
            int statusCode; // 状态码，为便于信号复用，自定义的状态码均为负数

            // 解析inputJson所用
            Json::Value inValue;
            JSONCPP_STRING errs;
            Json::CharReaderBuilder readerBuilder;
            std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());

            Json::Value outValue;
            std::string code;
            std::string input;
            int cpuLimit;
            int memLimit;
            std::string fileName; // 待生成具有唯一性的文件名
            int runRes;

            if (!jsonReader->parse(inputJson.c_str(),
                                  inputJson.c_str() + inputJson.length(),
                                  &inValue, &errs))
            {
                LOG(ERROR) << "parse json err_1" << std::endl;
                statusCode = -1;
                goto END;
            }

            if (!errs.empty())
            {
                LOG(ERROR) << "parse json err_2" << std::endl;
                statusCode = -2;
                goto END;
            }
    
            code = inValue["code"].asString();
            input = inValue["input"].asString();
            cpuLimit = inValue["cpuLimit"].asInt();
            memLimit = inValue["memLimit"].asInt();

            if (code.size() == 0)
            {
                LOG(INFO) << std::endl;
                statusCode = -3; // 代码为空
                goto END;
            }
            // 生成具有唯一性的文件名
            fileName = FileUtil::GenUniqueId();

            if (!FileUtil::Write(FileUtil::SrcFormat(fileName), code))
            {
                statusCode = -4; // 写入错误
                goto END;
            }
            if (!Compiler::Compile(fileName))
            {
                statusCode = -5; // 编译错误
                goto END;
            }

            runRes = Runner::Run(fileName, cpuLimit, memLimit);

            // 运行错误
            if (runRes < 0)
            {
                statusCode = runRes; // 未知的内部错误
            }
            else if (runRes > 0)
            {
                statusCode = runRes; // 程序收到信号异常终止
            }
            else
            {
                statusCode = 0;
                // 成功
            }

        END:
            outValue["status"] = statusCode;
            outValue["reason"] = StatusToInfo(statusCode,fileName);
            // 只有运行成功（无论结果对错）时，才会有输出
            if (statusCode == 0)
            {
                std::string buf;
                FileUtil::ReadFile(FileUtil::Stdout(fileName),buf,true);
                outValue["stdout"] = buf;
            }
            // 运行成功或运行异常
            if (statusCode >= 0)
            {
                std::string buf;
                FileUtil::ReadFile(FileUtil::Stderr(fileName),buf,true);
                outValue["stderr"] =  buf;
                
            }

            // 序列化
            JsonUtil::Serialization(outValue,outJson);
            Serialization(outValue, outJson);

            //清理临时文件
            CleanTempFile(fileName);
        }
    };
}