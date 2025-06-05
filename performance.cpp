#include <mqtt/async_client.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <shared_mutex>
#include <atomic>
using namespace std;
#include <atomic>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>


class ThreadPool {
public:
    explicit ThreadPool(size_t threads = 2) : stop(false) {
        for(size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this]{ 
                            return stop || !tasks.empty(); 
                        });
                        if(stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    
    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }
    
    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker : workers) {
            if(worker.joinable()) worker.join();
        }
    }
    
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{false};
};


ThreadPool thread_pool_;
std::shared_mutex mtx;
void sub_callback_func(std::string topic, std::string msg_id);
std::string get_msgid_frommsg(std::string msg);
class action_listener : public virtual mqtt::iaction_listener
{
    std::string name_;

    void on_failure(const mqtt::token& tok) override {}

    void on_success(const mqtt::token& tok) override {}

public:
    action_listener(const std::string& name) : name_(name) {}
};

class subcallback : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
private:
    int nretry_ = 0;
    mqtt::async_client* cli_;
    mqtt::connect_options& options_;
    action_listener subListener_;
    std::string topic_;
    int qos_;

    void on_failure(const mqtt::token& tok) override
    {
        std::cout << "Connection attempt failed" << std::endl;
        if (++nretry_ > 5)
            exit(1);
        reconnect();
        nretry_++;
    }
    void on_success(const mqtt::token& tok) override {}

    void reconnect()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        try {
            cli_->connect(options_, nullptr, *this);
        }

        catch (const mqtt::exception& exc) {
            std::cerr << "Error: " << exc.what() << std::endl;
            exit(1);
        }
    }

    void connected(const std::string& cause) override
    {
        std::cout << "\nConnection success" << std::endl;

        // cli_->subscribe(topic_, qos_, nullptr, subListener_);
        // std::cout << "connected topic is " << this->topic_ << " qos is " << this->qos_
        //           << std::endl;
        // cli_->subscribe("data/rand", qos_, nullptr, subListener_);
    }

    void message_arrived(mqtt::const_message_ptr msg) override
    {
        std::cout << " message get " << std::endl;
        std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
        // std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
        auto topic = msg->get_topic() ;
        auto mssg = msg->to_string();
        thread_pool_.enqueue([this, topic, mssg] {

            sub_callback_func(topic, get_msgid_frommsg(mssg));
        });
        // sub_callback_func(msg->get_topic(), get_msgid_frommsg(msg->to_string()));
    }
    void connection_lost(const std::string& cause) override
    {
        std::cout << "\nConnection lost" << std::endl;
        if (!cause.empty())
            std::cout << "\tcause: " << cause << std::endl;

        std::cout << "Reconnecting..." << std::endl;
        nretry_ = 0;
        reconnect();
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override {}

public:
    subcallback(mqtt::async_client* cli, mqtt::connect_options& options)
        : nretry_(0), cli_(cli), options_(options), subListener_("Subscription")
    {
    }

    subcallback& set_topic(const std::string& topic)
    {
        std::cout << " topis= " << topic << " topic2 " << topic_ << std::endl;
        topic_ = topic;
        return *this;
    }

    subcallback& set_qos(const int& qos)
    {
        qos_ = qos;
        return *this;
    }
};

class pubcallback : public virtual mqtt::callback
{
public:
    void connection_lost(const string& cause) override
    {
        cout << "\nConnection lost" << endl;
        if (!cause.empty())
            cout << "\tcause: " << cause << endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr tok) override
    {
        cout << "\tDelivery complete for token: " << (tok ? tok->get_message_id() : -1)
             << endl;
    }
};

std::unordered_map<std::string, std::unordered_map<std::string, std::chrono::milliseconds>> topic_timesend_;
std::unordered_map<std::string, long long> time_can;

void sub_callback_func(std::string topic, std::string msg_id)
{
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch()
    )
                   .count();
    // std::unique_lock<std::shared_mutex> lock(mtx);
    auto cost = (now - topic_timesend_[topic][msg_id].count());
    //   lock.unlock();
    //   lock.lock();
    time_can[topic] += cost;
    // lock.unlock();
    std::cout<<" topic: "<<topic<<" msg_id: "<<msg_id<<" time spent: "<< cost <<" ms ."<<std::endl;
}

std::vector<std::string> init(int num)
{    std::cout<<" 233333333333333333333: num is "<<num<<std::endl;
    std::vector<std::string> vec;
    //    time_can.resize(num);
    for (int i = 0; i < num; i++) {
        vec.emplace_back("topic" + std::to_string(i));
        //   topic_timesend_["topic"+std::to_string(num)].resize(msg_num);
    }
        std::cout<<" 4444444444444444444444: "<<std::endl;
    return vec;
}

std::string get_msgid_frommsg(std::string msg)
{
    std::string msg_id;
    for (auto pos = msg.rbegin(); pos != msg.rend(); pos++) {
        if (*pos == '+') {
            break;
        }
        msg_id += *pos;
    }
    reverse(msg_id.begin(), msg_id.end());
    // std::string id = reverse(msg_id.begin(), msg_id.end());
    return msg_id;
}

std::string encap_msg(int size, int msg_id)
{
    std::string msg(size, '+');
    msg += std::to_string(msg_id);
    return msg;
}
void muti_sub(
    std::vector<std::string> topic_vec, mqtt::async_client* cli,
    mqtt::connect_options& conn_option
)
{

    for (auto& topic : topic_vec) {
        // subcallback cb(cli, conn_option);
        // cb.set_qos(2).set_topic(topic);
        // cli->connect(conn_option, nullptr, cb);
                std::cout<<"subscriber topic  : "<<topic<<std::endl;
        auto tok = cli->subscribe(topic, 2);
        tok->wait();
               std::cout<<" 555555555555555555333333333: "<<std::endl;
    }
        // std::cout<<" 11111111111111: "<<std::endl;
}
void muti_pub_msg(mqtt::async_client& cli_pub, std::vector<std::string>& topic_vec, int &send_times)
{
    int msg_id = 0;
    for(int i=0; i<send_times; i++)
    {
        for (auto topic : topic_vec) {
            auto msg = encap_msg(1024 * 1024 * 2, msg_id);
            auto tok = cli_pub.publish(topic, msg, 2, false);
            tok->wait();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            // std::unique_lock<std::shared_mutex> lock(mtx);
            topic_timesend_[topic][std::to_string(msg_id)] = timestamp;
            // lock.unlock();
            //  std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout<<" publish topic:  "<<topic<<" msg_id is  "<<msg_id<<" timestamp is: "<<timestamp.count()<<std::endl;
        }
        msg_id++;
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

}


void sub()
{
    auto url("tcp://192.168.147.1");
    auto client_id_sub("sub");
    auto qos = 2;

    mqtt::connect_options conn_option;
    mqtt::create_options createOpts(MQTTVERSION_5);
    mqtt::async_client *cli_sub = new mqtt::async_client(url, client_id_sub, createOpts);
    subcallback *cb = new subcallback(cli_sub, conn_option);
    cli_sub->set_callback(*cb);
    // cb.set_qos(2);

    int send_times = 1000;
    int topic_num = 50;
    std::vector<std::string> topic_vec = init(topic_num);  
    auto conn_tok = cli_sub->connect(conn_option, nullptr, *cb);
    conn_tok->wait();
    muti_sub(topic_vec, cli_sub, conn_option);
    // while(true)
    // {
    //         std::this_thread::sleep_for(std::chrono::milliseconds(800));
    // }
}

void sub2()
{
    auto url("tcp://192.168.147.1");
    auto client_id_sub("sub2");
    auto qos = 2;

    mqtt::connect_options conn_option;
    mqtt::create_options createOpts(MQTTVERSION_5);
    mqtt::async_client *cli_sub = new mqtt::async_client(url, client_id_sub, createOpts);
    subcallback *cb = new subcallback(cli_sub, conn_option);
    cli_sub->set_callback(*cb);
    // cb.set_qos(2);

    int send_times = 1000;
    int topic_num = 100;
    std::vector<std::string> topic_vec = init(topic_num);  
    auto conn_tok = cli_sub->connect(conn_option, nullptr, *cb);
    conn_tok->wait();

        std::vector<std::string> vec;
    //    time_can.resize(num);
    for (int i = 50; i < 100; i++) {
        vec.emplace_back("topic" + std::to_string(i));
        //   topic_timesend_["topic"+std::to_string(num)].resize(msg_num);
    }
    muti_sub(vec, cli_sub, conn_option);
    // while(true)
    // {
    //         std::this_thread::sleep_for(std::chrono::milliseconds(800));
    // }
}
// std::atomic<bool> can_calculate {false};
// void cauculate_time()
// { 
//     while(can_calculate.load())
//     {
//         for(auto &time:time_can)
//         {
//            std::cout<<" topic :"<<time.first<<" average timer is : "<<time.second/1000<<std::endl;
//         }
//         break;
//     }
// }
void pub()
{
    auto url("tcp://192.168.147.1");
    auto client_id_pub("pub");
    auto qos = 2;
    auto topic("data/rand");
        mqtt::connect_options conn_option;
    

     mqtt::create_options createOpts(MQTTVERSION_5);
    mqtt::async_client cli_pub(url, client_id_pub, createOpts);
 
    // cli_sub.set_buffer_size(MAX_BUFFERED_MESSAGES); 
    std::cout<<" 11111111111111: "<<std::endl;


    // cli_sub.set_callback_thread();

    // .set_topic(topic);

    pubcallback pub_cb;
    auto cpnn_pub = cli_pub.connect(conn_option);
    cpnn_pub->wait();
    std::cout<<" 2222222222222222: "<<std::endl;
    // mqtt::async_client client;
    int send_times = 1000;
    int topic_num = 50;
    std::vector<std::string> topic_vec = init(topic_num);  // 1: topic num 2: msg num per topic
    muti_pub_msg(cli_pub, topic_vec, send_times);
    // cauculate_time();
}

void pub2()
{
    auto url("tcp://192.168.147.1");
    auto client_id_pub("pub2");
    auto qos = 2;
    auto topic("data/rand");
        mqtt::connect_options conn_option;
    

     mqtt::create_options createOpts(MQTTVERSION_5);
    mqtt::async_client cli_pub(url, client_id_pub, createOpts);
 
    // cli_sub.set_buffer_size(MAX_BUFFERED_MESSAGES); 
    std::cout<<" 11111111111111: "<<std::endl;


    // cli_sub.set_callback_thread();

    // .set_topic(topic);

    pubcallback pub_cb;
    auto cpnn_pub = cli_pub.connect(conn_option);
    cpnn_pub->wait();
    std::cout<<" 2222222222222222: "<<std::endl;
    // mqtt::async_client client;
    int send_times = 1000;
    int topic_num = 100;
    std::vector<std::string> vec;
    //    time_can.resize(num);
    for (int i = 50; i < 100; i++) {
        vec.emplace_back("topic" + std::to_string(i));
        //   topic_timesend_["topic"+std::to_string(num)].resize(msg_num);
    }

    // std::vector<std::string> topic_vec = init(topic_num);  // 1: topic num 2: msg num per topic
    muti_pub_msg(cli_pub, vec, send_times);
    // cauculate_time();
}

void start()
{

    thread th(sub);
    // //  cli_sub.subscribe("11", 2);
    // sub();
    thread th2(pub2);
    thread th3(sub2);
    pub();
    th3.join();
    th2.join();
    th.join();
    while(true)
    {
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
}

int main() { 

    start();
    return 0; }





// int main() {
//     // Assume client is created and connected already
//     // Setup callbacks for message arrival and connection lost
//     MQTTAsync_setCallbacks(client, nullptr, connectionLost, messageArrived, nullptr);

//     // Subscribe asynchronously
//     asyncSubscribe();

//     // Launch multiple threads to publish concurrently
//     const int publisher_threads = 8;
//     const int messages_per_thread = 1000;

//     std::vector<std::thread> threads;
//     for (int i = 0; i < publisher_threads; ++i) {
//         threads.emplace_back([i, messages_per_thread]() {
//             for (int j = 0; j < messages_per_thread; ++j) {
//                 std::string msg = "Thread " + std::to_string(i) + " message " + std::to_string(j);
//                 asyncPublish(msg);
//                 // Optional: throttle publishing speed
//                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//             }
//         });
//     }

//     // Wait for all publishers to finish
//     for (auto& t : threads) {
//         t.join();
//     }

//     // Wait for all messages to be delivered (simple heuristic)
//     std::this_thread::sleep_for(std::chrono::seconds(5));

//     std::cout << "Total messages delivered: " << delivered_count.load() << std::endl;

//     return 0;
// }