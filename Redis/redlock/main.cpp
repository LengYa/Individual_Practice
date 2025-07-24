#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <sw/redis++/redis++.h>
#include <sw/redis++/patterns/redlock.h>

using namespace std;
using namespace sw::redis;

const string PRE_TCP = "tcp://";
const string PRE_UNIX = "unix://";
const string REDIS_PASSWD = "123456";
const string REDIS_IP = "192.168.127.132";

void process_order(const string& client_id, int order_id);
          
random_device rd;     
mt19937 gen(rd());  
uniform_int_distribution<> dis; 
int main() 
{
    try {
        // 1. 创建5个Redis实例（模拟分布式环境）
        // Redis 节点配置 (5个独立节点)
        vector<string> redis_nodes = {
            PRE_TCP+REDIS_PASSWD+"@"+REDIS_IP+":6379",
            PRE_TCP+REDIS_PASSWD+"@"+REDIS_IP+":6380",
            PRE_TCP+REDIS_PASSWD+"@"+REDIS_IP+":6381",
            PRE_TCP+REDIS_PASSWD+"@"+REDIS_IP+":6382",
            PRE_TCP+REDIS_PASSWD+"@"+REDIS_IP+":6383"
        };
        vector<shared_ptr<Redis>> redis_instances;
        for (const auto& node : redis_nodes) {
            redis_instances.push_back(make_shared<Redis>(node));
            cout << "Connected to Redis: " << node << endl;
        }

        if(redis_instances.empty()){
            cerr<<"no valid redis instances!\n";
            return 1;
        }
        
        // 2. 定义锁参数
        const string RESOURCE = "distributed_lock"; // 锁的资源名称
        RedMutexOptions options;
        options.ttl = 5s;               // 锁自动过期时间
        options.retry_delay = 500ms;     // 获取锁失败的重试间隔
        options.scripting = true;     // 是否启用Lua脚本, 默认是启动Lua脚本

        // 3. 创建分布式锁
        auto watcher = make_shared<LockWatcher>();
        RedMutex mtx(redis_instances.begin(), redis_instances.end(),
                     RESOURCE,
                     [](std::exception_ptr eptr) {
                         // 自动续期失败回调（生产环境需记录日志）
                         try {
                             if (eptr) std::rethrow_exception(eptr);
                         } catch (const Error &e) {
                             std::cerr << "Lock auto-extend failed: " << e.what() << std::endl;
                         }
                     },
                     options, watcher);

        unique_lock<RedMutex> lock(mtx, defer_lock);

        auto client_thread = [&](string client_id, int task_count){
            try{
                    // 尝试获取锁（非阻塞版本）    
                    // if (!lock.try_lock()) {
                    //     cerr << "[" << client_id << "] Failed to acquire lock - operation skipped" << endl;
                    //     return;
                    // }

                    lock.lock();
                    cout << "[" << client_id << "] acquired lock\n";

                    // 模拟临界区操作
                    for (int i = 1; i <= task_count; ++i) {
                        process_order(client_id, i);
                    }

                    // 手动释放锁（析构时也会自动释放）
                    lock.unlock();
                    cout << "[" << client_id << "] released lock\n";
                
                    // 随机延迟后执行下一个任务
                    dis.param(uniform_int_distribution<>::param_type(50, 300));
                    this_thread::sleep_for(chrono::milliseconds(dis(gen)));
            }catch(const exception& e) {
                cerr << "[" << client_id << "] Error: " << e.what() << endl;
            }catch(const Error& e) {
                cerr << "Redis error: " << e.what() << endl;
            }
        };

        // 4. 创建多线程模拟多个客户端并发访问
        vector<thread> clients;
        vector<string> client_ids = {"ClientA", "ClientB", "ClientC"};
        for (const auto& id : client_ids) {
            clients.emplace_back(client_thread, id, 5);
        }
        
        // 等待所有客户端完成
        for (auto& t : clients) {
            t.join();
        }
        
        cout << "All client operations completed" << endl;
    } catch (const exception& e) {
        cerr << "Main Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

// 模拟关键业务操作
void process_order(const string& client_id, int order_id) 
{
    cout << "[" << client_id << "] Starting processing order #" << order_id << endl;
    
    // 模拟业务处理时间 (100-500ms)
    dis.param(uniform_int_distribution<>::param_type(100, 500));
    this_thread::sleep_for(chrono::milliseconds(dis(gen)));
    
    cout << "[" << client_id << "] Completed order #" << order_id << endl;
}
