#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <json/json.h>
#include <atomic>
#include <vector>
#include <memory>

// 工具模块，提供常用的工具
namespace UtilNameSpace
{
    const std::string tempPath = "./temp/";
    class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            struct timeval time;
            gettimeofday(&time, nullptr);
            return std::to_string(time.tv_sec);
        }

        // 获取毫秒时间戳
        static std::string GetTimeStamp_ms()
        {
            struct timeval time;
            gettimeofday(&time, nullptr);
            return std::to_string(time.tv_sec * 1000 + time.tv_usec / 1000);
        }
    };
    class FileUtil
    {
    public:
        // 编译时临时文件
        // 构建 源文件路径+后缀 的完整格式 ./xxx/fileName.cc
        static std::string SrcFormat(const std::string &fileName)
        {
            std::string output = tempPath + fileName + ".cc";
            return output;
        }
        // 构建 可执行程序+后缀 的完整格式 ./xxx/fileName.exe
        static std::string ExeFormat(const std::string &fileName)
        {
            std::string output = tempPath + fileName + ".exe";
            return output;
        }
        // 构建 标准错误+后缀 的完整格式 ./xxx/fileName.errlog
        static std::string CompileErrFormat(const std::string &fileName)
        {
            std::string output = tempPath + fileName + ".errlog";
            return output;
        }

        // 运行时
        /*
            程序在启动时默认打开标准输入输出流，在此三个文件需要我们自己提供
        */
        static std::string Stdin(const std::string &fileName)
        {
            std::string output = tempPath + fileName + ".stdin";
            return output;
        }
        static std::string Stdout(const std::string &fileName)
        {
            std::string output = tempPath + fileName + ".stdout";
            return output;
        }
        static std::string Stderr(const std::string &fileName)
        {
            std::string output = tempPath + fileName + ".stderr";
            return output;
        }

        // 生成具有唯一性的文件名  原子性递增唯一值+毫秒级时间戳
        static std::string GenUniqueId()
        {
            std::string fileName;
            std::atomic_uint64_t id; // 具有原子性
            ++id;
            fileName = std::to_string(id) + "." + TimeUtil::GetTimeStamp_ms();
            return fileName;
        }

        // 写
        static bool Write(const std::string &fileNameWithPath, std::string code)
        {
            std::ofstream ofs(fileNameWithPath, std::ofstream::out);
            if (!ofs.is_open())
            {
                return false;
            }

            ofs.write(code.c_str(), code.size());
            ofs.close();

            return true;
        }

        // 读
        static bool ReadFile(const std::string &fileNameWithPath, std::string &content, bool keepLineSeparator)
        {
            std::ifstream ifs(fileNameWithPath, std::ifstream::in);
            if (!ifs.is_open())
            {
                return false;
            }
            content.clear();
            std::string line;
            // getline不保存行分隔符
            while (getline(ifs, line))
            {
                content += line;
                keepLineSeparator ? (content += "\n") : content;
            }
            ifs.close();

            return true;
        }

        // 判断指定路径下的文件是否存在
        static bool IsExist(const std::string &fileNameWithPath)
        {
            struct stat fileStat;
            if (stat(fileNameWithPath.c_str(), &fileStat) == 0)
            {
                return true;
            }
            return false;
        }
    };

    class StringUtil
    {
    public:
        /*
            str:待切分的字符串
            target:输出型参数，切分后存于此处
            separator:分隔符
        */
        static void SplitString(const std::string &str, std::vector<std::string> &target, const char separator)
        {
            target.clear();

            std::string token; // 用于存储每个子字符串

            for (char c : str)
            {
                if (c == separator)
                {
                    if (!token.empty())
                    {
                        target.push_back(token);
                        token.clear();
                    }
                }
                else
                {
                    token += c;
                }
            }

            if (!token.empty())
            {                            // 处理最后一个子字符串
                target.push_back(token); // 存储子字符串到目标vector容器中
            }
        }
    };

    class JsonUtil
    {
    public:
        // value:需要序列化的Value值
        // str:输出型参数
        static void Serialization(const Json::Value &inValue, std::string &outputJson)
        {
            std::ostringstream os;
            Json::StreamWriterBuilder writerBuilder;
            std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
            jsonWriter->write(inValue, &os);
            outputJson = os.str();
        }
        /*
            inputJson:需要反序列化的json字符串
            outValue:输出型参数。存储反序列化后的Value
        */
        static bool Deserialization(const std::string &inputJson, Json::Value &outValue)
        {
            JSONCPP_STRING errs;
            Json::CharReaderBuilder readerBuilder;
            std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());

            if (!errs.empty() || !jsonReader->parse(inputJson.c_str(),
                                                    inputJson.c_str() + inputJson.length(),
                                                    &outValue, &errs))
            {
                return false;
            }

            return true;
        }

        /*
            构建url
        */
        static std::string CreateUrl(const std::string &ip, int port)
        {
            std::string url = ip+":"+ std::to_string(port);
            return url;
        }
    };

}