#pragma once

#include <ctemplate/template.h>
#include <string>
#include <iostream>
#include "OJModel_sql.hpp"

using namespace Model_sqlNameSpace;

namespace ViewNameSpace{
    const std::string templatePath = "./template_html/";
    class View{
    protected:
        
    public:
        View(){

        }
        ~View(){

        }
        /*
            题目编号 题目标题 题目难度
            questions:输入性参数
            html:输出型参数，渲染后的html网页
        */
        void ExpandHtmlAllQ(const std::vector<Question_t> &questions,std::string &html){
            // html路径
            const std::string allQHtml=templatePath+"AllQuestions.html";

            // 形成字典
            ctemplate::TemplateDictionary root("allQuestions");

            for(const auto &q:questions){
                /*
                    待填充内容
                    <tr>
                        <td>{{number}}</td>
                        <td>{{title}}</td>
                        <td>{{star}}</td>
                    </tr>
                */
               ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("questionsList");
               sub->SetValue("number",q.number);
               sub->SetValue("title",q.title);
               sub->SetValue("star",q.star);
            }
            // 获取需要被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(allQHtml,ctemplate::DO_NOT_STRIP/*获取*/);

            // 渲染
            tpl->Expand(&html,&root);
        }
        void ExpandHtmlQ(const Question_t &question,std::string &html){
            // 构建html路径
            std::string questionHtml = templatePath + "Question.html";

            // 形成字典
            ctemplate::TemplateDictionary root("Question");

            root.SetValue("number",question.number);
            root.SetValue("title",question.title);
            root.SetValue("star",question.star);
            root.SetValue("desc",question.desc);
            root.SetValue("pre_code",question.header);

            // 获取需要渲染的html
             ctemplate::Template *tpl = ctemplate::Template::GetTemplate(questionHtml,ctemplate::DO_NOT_STRIP/*获取*/);

            //渲染
            tpl->Expand(&html,&root);
        }
    };
}