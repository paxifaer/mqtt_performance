#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include <mqtt/async_client.h>

class action_listener: public virtual mqtt::iaction_listener
{
    std::string name_;

    void on_failure(const mqtt::token& tok) override
    {
    }

    void on_success(const mqtt::token& tok) override
    {
    }

    public:
      action_listener(const std::string& name) :name_(name){}
      
};

class callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
   private:
      int nretry_=0;
      mqtt::async_client& cli_;
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
      void on_success(const mqtt::token& tok) override
      {
      }
      
      void reconnect()
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(2500));
         try{
            cli_.connect(options_, nullptr, *this);
         }
         catch (const mqtt::exception& exc)
         {
            std::cerr << "Error: " << exc.what() << std::endl;
            exit(1);
         }
      }

      void connected(const std::string& cause) override
      {
         std::cout << "\nConnection success" << std::endl;
   
         // cli_.subscribe(topic_, qos_, nullptr, subListener_);
         // std::cout<<"connected topic is "<<this->topic_<<" qos is "<<this->qos_<<std::endl;
 
      }

      void message_arrived(mqtt::const_message_ptr msg) override
      {
        std::cout<< " message get "<<std::endl;
        std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
        std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
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
      callback(mqtt::async_client& cli, mqtt::connect_options& options)
        :   nretry_(0), cli_(cli), options_(options), subListener_("Subscription")
      {
      }

      callback& set_topic(const std::string& topic)
      {
         std::cout<<" topis=c 1"<<topic <<" topic2 "<< topic_<<std::endl;
        topic_ = topic;
        return *this;
      }

      callback& set_qos(const int& qos)
      {
        qos_ = qos;
        return *this;
      }
};

int main()
{
   auto url("tcp://192.168.147.1");
   auto client_id("atom_client");
   auto qos = 1;
   auto topic("data/rand");
   mqtt::connect_options conn_option;
   mqtt::async_client cli(url,client_id);
   
   callback cb(cli,conn_option);
   cli.set_callback(cb);
   // cb.set_qos(1)
   //   .set_topic(topic);
   
auto tok = cli.connect(conn_option, nullptr, cb);
tok->wait();
           cli.subscribe("topic", 2);
          cli.subscribe("topic1", 2);
         //   cli_.subscribe("topic2", 2, nullptr, subListener_);
         //    cli_.subscribe("topic3", 2, nullptr, subListener_);
         //     cli_.subscribe("topic4", 2, nullptr, subListener_);
         //      cli_.subscribe("topic5", 2, nullptr, subListener_);
       while(true)
    {
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
   return 0;
}