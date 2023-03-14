/*
    此为数据库版Model
*/
#pragma once
#include "mysql_include/mysql.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <assert.h>
#include "../common/Log.hpp"
using namespace LogNameSpace;
/*
    根据题目list文件，加载所有题目的信息到内存中
*/
namespace Model_sqlNameSpace
{
    struct Question_t
    {
        std::string number; // 题目编号
        std::string title;  // 题目标题
        std::string star;   // 难度
        std::string desc;   // 题目描述
        std::string header; // 预设编译器代码
        std::string tail;   // 测试用例，需与header拼接形成完整代码
        int cpuLimit;
        int memLimit;
    };
    const std::string questionsTableName = "oj_questions";
    const std::string user = "oj_client";
    const std::string passwd = "liang";
    const std::string db = "oj";
    const int port = 3306;
    // Model:主要用来和数据进行交互，对外提供访问数据的接口
    class Model
    {
    protected:
        std::unordered_map<std::string, Question_t> NumberToQuestion;

    public:
        bool QueryMysql(const std::string &sql, std::vector<Question_t> &questions)
        {
            MYSQL *my = mysql_init(nullptr);

            if (nullptr == mysql_real_connect(my, "127.0.0.1", user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0))
            {
                LOG(FATAL) << "连接数据库失败" << std::endl;
                return false;
            }

            //!!!!!!!!!!不设置编码格式会出现乱码问题
            mysql_set_character_set(my, "utf8");

            // 执行sql语句
            if (0 != mysql_query(my, sql.c_str()))
            {
                LOG(WARNING) << sql << "执行出错"
                             << "\n";
                return false;
            }

            // 提取结果
            MYSQL_RES *res = mysql_store_result(my);

            // 分析结果
            int rows = mysql_num_rows(res);   // 获得行数量
            int cols = mysql_num_fields(res); // 获得列数量

            Question_t q;
            for (int i = 0; i < rows; i++)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                q.number = row[0];
                q.title = row[1];
                q.star = row[2];
                q.desc = row[3];
                q.header = row[4];
                q.tail = row[5];
                q.cpuLimit = atoi(row[6]);
                q.memLimit = atoi(row[7]);

                questions.push_back(q);
            }

            // 释放结果空间
            free(res);
            // 关闭mysql连接
            mysql_close(my);

            return true;
        }

        // 提供给外界的接口
        // 获取所有题目
        // questions:输出型参数
        bool GetAllQuestions(std::vector<Question_t> &questions)
        {
            std::string sql = "select * from " + questionsTableName;
            if (!QueryMysql(sql, questions))
            {
                return false;
            }
            return true;
        }

        // 通过编号题目获取完整题目信息
        bool GetQuestionByNum(const std::string &number, Question_t &question)
        {
            bool res = false;
            std::string sql = "select * from " + questionsTableName;
            sql += " where number=";
            sql += number;
            std::vector<Question_t> q;
            if (QueryMysql(sql, q))
            {
                if (q.size() == 1)
                {
                    question = q[0];
                    res = true;
                }
            }

            return res;
        }
    };

}