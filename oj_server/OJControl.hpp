/*
    核心逻辑:所有交互通过control完成，control通过modle加载所有题目，通过view构建各种网页需求
*/
#pragma once

#include <iostream>
#include <string>
#include <mutex>
#include <algorithm>
#include "../common/httplib.h"
//#include "OJModel.hpp"
#include "OJView.hpp"
#include "../common/Log.hpp"
#include "../util/Util.hpp"

using namespace UtilNameSpace;
using namespace LogNameSpace;
using namespace ViewNameSpace;


const std::string confFilePath = "./conf/worker.conf";

namespace ControlNameSpace
{
    /*
        主机类
    */
    class Worker
    {
    protected:
        std::string _ip;  // 编译服务的ip
        int _port;        // 端口
        uint64_t _load;   // 编译服务的负载
        std::mutex *_mtx; // C++mutex是禁止拷贝的，因此出现了报错。所以此处我们使用指针
    public:
        Worker() : _ip(""),
                   _port(0),
                   _load(0),
                   _mtx(nullptr)
        {
        }

        Worker(const std::string &ip, int port, uint64_t load) : _ip(ip),
                                                                 _port(port),
                                                                 _load(load),
                                                                 _mtx(new std::mutex())
        {
        }

        ~Worker()
        {
        }

        /*
            负载修改
        */
        // 提升负载
        void IncLoad()
        {
            if (_mtx)
            {
                _mtx->lock();
                ++_load;
                _mtx->unlock();
            }
            else
            {
            }
        }
        // 减少负载
        void DecLoad()
        {
            if (_mtx)
            {
                _mtx->lock();
                --_load;
                _mtx->unlock();
            }
            else
            {
            }
        }

        void ResetLoad()
        {
            if (_mtx)
            {
                _mtx->lock();
                _load=0;
                _mtx->unlock();
            }
            else
            {
            }
        }

        /*
            访问
        */
        uint64_t GetLoad() const
        {
            return _load;
        }

        std::string GetIp() const
        {
            return _ip;
        }

        int GetPort() const
        {
            return _port;
        }

        void ShowLoad(){
            std::cout<<GetIp()<<":"<<GetPort()<<":"<<GetLoad()<<std::endl; 
        }
    };

    /*
         负载均衡模块
    */
    class LoadBlance
    {
    protected:
        // 提供编译服务的所有主机，每一台主机都有相应的下标，用此下标充当主机ID
        std::vector<Worker> _worker;
        // 在线主机ID
        std::vector<int> _onlineWorkerId;
        // 离线主机ID
        std::vector<int> _offlineWorkerId;
        // 保证LoadBlance数据安全
        std::mutex *_mtx;

    public:
        LoadBlance() : _mtx(new std::mutex())
        {
            if (!LoadConf(confFilePath))
            {
                assert(0);
            }
        }
        ~LoadBlance() {}

        /*
            功能：
                1. 选择主机，并更新负载
                2. 可能需要离线某个主机

            id: 输出型参数，选择的主机的Id
            w: 输出型参数
        */
        bool SelectWorker(int &id, Worker **w /*为避免引起赋值构造，此处使用二级指针*/)
        {
            _mtx->lock();
            /*
                负载均衡方案：
                1.随机选择+hash
                2.轮询找负载最小
            */
            int onlineNum = _onlineWorkerId.size();
            if (onlineNum == 0)
            {
                _mtx->unlock(); // 未解锁导致错误，切记切记加锁后一定不要忘记解锁

                LOG(FATAL) << "提供CAR服务的主机全部离线" << std::endl;
                return false;
            }

            // 找到在线id最小的第一台主机
            id = _onlineWorkerId[0];
            *w = &_worker[id];
            uint64_t minLoad = (*w)->GetLoad();

            for (int i = 1; i < onlineNum; ++i)
            {
                if (_worker[_onlineWorkerId[i]].GetLoad() < minLoad)
                {
                    minLoad = _worker[_onlineWorkerId[i]].GetLoad();
                    id = _onlineWorkerId[i];
                    // w = _worker[id]; ！！！！！赋值构造
                    *w = &_worker[id];
                }
            }

            // (*w)->IncLoad();

            _mtx->unlock();
            return true;
        }

        // 上线所有主机
        void OnlineWorker()
        {
            _mtx->lock();
            _onlineWorkerId.insert(_onlineWorkerId.end(),_offlineWorkerId.begin(),_offlineWorkerId.end());
            _mtx->unlock();
            LOG("INFO")<<"重新上线主机"<<std::endl;
        }

        /*
            离线主机并清零负载
        */
        void OfflineWorker(int id)
        {
            _mtx->lock();
            for(auto it = _onlineWorkerId.begin();it!=_onlineWorkerId.end();++it){
                //查找要离线的主机
                if(*it == id){
                    _onlineWorkerId.erase(it);
                    _worker[id].ResetLoad();
                    // _offlineWorkerId.push_back(*it);//erase操作会引起迭代器失效
                    _offlineWorkerId.push_back(id);
                    // _worker[id].ResetLoad();
                    break;
                }
            }
            _mtx->unlock();
        }

        // 
        void ShowOnlineLoad(){
            if(_onlineWorkerId.size()==0){
                LOG(FINAL)<<"当前没有主机在线"<<std::endl;
                return;
            }
            for(const auto &id:_onlineWorkerId){
                LOG(INFO)<<"当前在线主机负载"<<std::endl;
                std::cout<<_worker[id].GetIp()<<":"<<_worker[id].GetPort()<<":"<<_worker[id].GetLoad()<<std::endl;
            }
        }


        void ShowAllWorker(){
            for(const auto &w:_worker){
                LOG(INFO)<<"主机信息："<<std::endl;
                std::cout<<w.GetIp()<<":"<<w.GetPort()<<std::endl;
            }
        }


    protected:
        // 加载配置文件
        bool LoadConf(const std::string &confFile)
        {
            std::string conf;
            std::ifstream ifs(confFile);

            if (!ifs.is_open())
            {
                // 配置文件打开失败
                LOG(FATAL) << "配置文件打开失败" << std::endl;
                return false;
            }

            std::string line;

            while (getline(ifs, line))
            {
                std::vector<std::string> res;
                // line -->ip:port
                StringUtil::SplitString(line, res, ':');

                if (res.size() != 2)
                {
                    // 该行数据不可用
                    LOG(WARING) << "主机配置文件存在无效行，请检查" << std::endl;
                    continue;
                }
                // 构建一个Worker对象，并存入worker数组
                Worker w(res[0], atoi(res[1].c_str()), 0);
                _worker.push_back(w);
                _onlineWorkerId.push_back(_worker.size() - 1);
            }
            ShowAllWorker();
            return true;
        }
    };

    class Control
    {
    protected:
        Model _model;
        View _view;
        LoadBlance _lb;
    public:
        Control()
        {
        }
        ~Control()
        {
        }

        // html:输出型参数，我们将构建一个包含所有题目信息的html文件
        bool AllQuestions(std::string &html)
        {
            std::vector<Question_t> allq;
            if (_model.GetAllQuestions(allq))
            {
                // 根据题目构建网页
                std::sort(allq.begin(),allq.end(),[](const Question_t &q1,const Question_t &q2){
                    return atoi(q1.number.c_str()) < atoi(q2.number.c_str());
                });
                LOG(DEBUG) << std::endl;
                _view.ExpandHtmlAllQ(allq, html);
                return true;
            }

            return false;
        }

        /*
            number:题号
            html:输出型参数
        */
        bool Question(const std::string &number, std::string &html)
        {
            Question_t q;

            if (_model.GetQuestionByNum(number, q))
            {
                // 构建该题目的html
                _view.ExpandHtmlQ(q, html);
                return true;
            }

            return false;
        }

        /*
            1. 根据题目编号，拿到相应的题目细节
            1. 将json进行反序列化，得到题目id和源代码
            2. 重新拼接用户代码+测试用例代码，形成新的代码
            3. 选择负载最低的主机，发起http请求，得到结果
                选取规则：一直选择，直到主机可用，如果始终不可用，就停止整个服务
            4. 将结果付给outJson
        */
        void Judge(const std::string &number, const std::string &inJson, std::string &outJson)
        {
            Question_t q;
            if(!_model.GetQuestionByNum(number,q)){
                //
                return;
            }

            // 反序列化。解析http json串
            Json::Value value;   
            JsonUtil::Deserialization(inJson,value);

            /*
                构建CAR格式的json串
                key:
                    1. code:代码 string
                    2. input:   string
                    3. cpuLimit: int
                    4. memLimit: int
            */
            Json::Value CARValue;
            std::string CARJson;
            CARValue["code"] = value["code"].asString() +"\n"+ q.tail;//完整代码由用户编写代码和该题目的测试用例拼接而成
            CARValue["input"] = value["input"].asString();
            CARValue["cpuLimit"] = q.cpuLimit;
            CARValue["memLimit"] = q.memLimit;
            JsonUtil::Serialization(CARValue,CARJson);

            // 选择提供CAR服务的主机
            while(1){
                int id=-1;
                Worker *w = nullptr;
                if(!_lb.SelectWorker(id,&w)){
                    break;
                }
                //LOG(INFO)<<"选取主机ID:"<<id<<"\n详情"<<"ip:"<<w->GetIp()<<":"<<w->GetPort()<<std::endl;
                
                //发起http请求
                httplib::Client cli(w->GetIp(),w->GetPort());
                w->IncLoad();
                w->ShowLoad();
                
                if(auto res = cli.Post("/CAR",CARJson,"application/json;charset=utf-8")){
                    if(res->status == 200){
                        // 请求成功,将CAR运行结果赋予outJson
                        outJson = res->body;
                        w->DecLoad();
                        break;
                    }
                    // 状态码不为200，请求失败，继续
                    w->DecLoad();
                }
                else{//请求失败，该主机因某些原因不可用，暂时下线该主机。继续请求
                    LOG(ERROR)<<"主机ID:"<<id<<"."<<"ip:"<<w->GetIp()<<":"<<w->GetPort()<<"可能已经离线"<<std::endl;
                    _lb.OfflineWorker(id);//离线并清零负载
                    // for test
                    _lb.ShowOnlineLoad();
                }

            }
        }

        void ReonlineWorker(){
            _lb.OnlineWorker();
        }

    };
}
