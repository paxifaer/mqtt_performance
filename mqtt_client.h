/*
 * @Author: yongzheng.luo
 * @Date: 2024-08-09 19:46:40
 * @LastEidtors: yongzheng.luo
 * @LastEditTime: 2024-09-02 16:33:36
 * @Description: Freetech Co.
 *
 * Copyright (C) Freetech (2017-2035)
 */
/**
 * @file RouterJ3Node.h
 * @author yangsongsen (songsen.yang@freetech.com)
 * @brief
 * @version 1.0
 * @date 2023-10-31
 *
 * @copyright Copyright (c) 2023 Freetech Intelligent System CO.,LTD
 *
 */

#pragma once

// #include "ara/core/initialization.h"
// #include "ara/exec/execution_client.h"
// #include "ara/log/logger.h"
// #include "fas/utils/print_helper.h"
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

// #include "template/node.hpp"

#include "mqtt/async_client.h"
// #include "system_logger.h"

// #include "vin_utils.h"

// extern "C"
// {
// #include "ft_dsa.h"
// #include "ft_osa.h"
// }

// #include "fas/utils/print_helper.h"

#include "mqtt/async_client.h"

// class atomApptoCAN_node;

class MQTTNode;
// class callback;
// class action_listener;
using namespace std;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

#pragma once
class node
{
public:
    virtual void Start() = 0;
    virtual void Process() = 0;
};


class MQTTNode : public node
{

    /**
     * A base action listener.
     */
    class action_listener : public virtual mqtt::iaction_listener
    {
        std::string name_;
        MQTTNode &mqttnode_;

    protected:
        void on_failure(const mqtt::token &tok)
        {
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Delivery attempt failed for token: " << tok.get_message_id();
        }

        void on_success(const mqtt::token &tok)
        {
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Delivery attempt success for token: " << tok.get_message_id();
        }

    public:
        action_listener(MQTTNode &mqttnode) : mqttnode_(mqttnode) {};
    };

#if 0
    /**
     * A derived action listener for publish events.
     */
    class delivery_action_listener : public action_listener
    {
        atomic<bool> done_;

        void on_failure(const mqtt::token &tok) override
        {
            action_listener::on_failure(tok);
            done_ = true;
        }

        void on_success(const mqtt::token &tok) override
        {
            action_listener::on_success(tok);
            done_ = true;
        }

    public:
        delivery_action_listener() : done_(false) {}
        bool is_done() const
        {
            return done_;
        }
    };
#endif
    /**
     * Local callback & listener class for use with the client connection.
     * This is primarily intended to receive messages, but it will also monitor
     * the connection to the broker. If the connection is lost, it will attempt
     * to restore the connection and re-subscribe to the topic.
     */
    class callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener

    {
        // Counter for the number of connection retries
        int nretry_;
        MQTTNode &mqttnode_;

        // Re-connection failure
        void on_failure(const mqtt::token &tok)
        {
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Connection attempt failed for token: " << tok.get_message_id() << " Try " << ++nretry_;
        }

        // (Re)connection success
        // Either this or connected() can be used for callbacks.
        void on_success(const mqtt::token &tok)
        {
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Action_Listener success for token: " << tok.get_message_id();
        }

        // (Re)connection success
        void connected(const std::string &cause)
        {
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Connected. Reason: '" << cause << "'";
        }

        // Callback for when the connection is lost.
        // This will initiate the attempt to manually reconnect.
        void connection_lost(const std::string &cause)
        {
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Connection lost";
            // if (!cause.empty())
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Cause: " << cause;
        }

        void delivery_complete(mqtt::delivery_token_ptr token)
        {
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Delivery attempt complete.";
        }

        // Callback for when a message arrives.
        void message_arrived(mqtt::const_message_ptr msg)
        {
            auto topic = msg->get_topic();
            auto str = msg->to_string();
            // mqttnode_.mqtt_log().LogInfo() << "MQTT: Message arrived topic: '" << topic << "'QOS: '" << msg->get_qos() << "'retained: '" << msg->is_retained() << "'payload: '" << str << "'";

            // print properties
            // auto msgProperties = msg->get_properties();
            // if (!msgProperties.empty())
            // {
            //     if (msgProperties.contains(mqtt::property::MESSAGE_EXPIRY_INTERVAL))
            //     {
            //         int msgExpiryInterval = mqtt::get<int>(msgProperties, mqtt::property::MESSAGE_EXPIRY_INTERVAL);
            //         //mqttnode_.mqtt_log().LogInfo() << "MQTT: MessageExpiryInterval: '" << msgExpiryInterval << "'";
            //     }
            // }
            // here only subscribed topic possible
            mqttnode_.mqtt_event(topic, str);
        }

    public:
        explicit callback(MQTTNode &mqttnode)
            : nretry_(0), /* connOpts_(connOpts), */ mqttnode_(mqttnode) //, subListener_()
        {};
    };

private:
    const string CONFIG_FILE{"/atom/etc/mqtt/mqtt_test_ssl.cfg"};
    const string SERVER_ADDRESS_SSL{"server_address_ssl"};
    const string SERVER_ADDRESS_NO_SSL{"server_address_no_ssl"};
    const string DFLT_CLIENT_ID{"ssl_subscribe_cpp"};
    const string KEY_STORE{"key_store"};
    const string TRUST_STORE{"trust_store"};
    const string SSL_ENABLE{"ssl_enable"};
    const string LWT_TOPIC{"events/disconnect"};
    const string LWT_PAYLOAD{"Last will and testament."};

    const string CLIENT_ID{"paho_cpp_ssl_async_publish_v5"};
    const string PERSIST_DIR{"/tmp/mqtt_persist"};

    const int QOS0 = 0;
    const int QOS1 = 1;
    const int QOS2 = 2;
    const int uniq_id_length = 10;

    string uniq_id;

    std::string generateRandomString(size_t length)
    {
        // Define a string of possible characters (symbols, digits, etc.)
        // const std::string characters = "!@#$%^&*()_-+=<>?/|,.;:{}[]~";
        const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopq"
                                       "rstuvwxyz0123456789";

        // Random engine and distribution
        std::random_device rd;                                         // Obtain random number from hardware
        std::mt19937 gen(rd());                                        // Seed the generator
        std::uniform_int_distribution<> dis(0, characters.size() - 1); // Define distribution

        std::string randomStr;
        for (size_t i = 0; i < length; ++i)
        {
            randomStr += characters[dis(gen)]; // Pick a random character from characters
        }
        return randomStr;
    }

public:
    MQTTNode()
    {
        uniq_id = generateRandomString(uniq_id_length);
    }

    ~MQTTNode()
    {
        stop_flag_ = true;
        if (handle_->joinable())
        {
            handle_->join();
        }
    }

    void Start()
    {
        // auto config = mqtt_config_file(CONFIG_FILE);
        // for (const auto &pair : config)
        // {
        //     logger_.LogDebug() << "MQTT:" << pair.first << " = " << pair.second;
        // }
        // if (config.find(TRUST_STORE) == config.end() || config.find(KEY_STORE) == config.end() || config.find(SSL_ENABLE) == config.end() || config.find(SERVER_ADDRESS_NO_SSL) == config.end() ||
        //         config.find(SERVER_ADDRESS_SSL) == config.end())
        // {
        //     logger_.LogInfo() << "MQTT: The config is not complete";
        //     return;
        // }
        std::string server_address_cur;
        std::string trust_store = config[TRUST_STORE];
        std::string key_store = config[KEY_STORE];
        std::string server_address_ssl = config[SERVER_ADDRESS_SSL];
        std::string server_address_not_ssl = config[SERVER_ADDRESS_NO_SSL];
        std::string ssl_enable = config[SSL_ENABLE];
        bool sslEnable = true;
        if (ssl_enable == "true")
        {
            server_address_cur = server_address_ssl;
            sslEnable = true;
        }
        else if (ssl_enable == "false")
        {
            server_address_cur = server_address_not_ssl;
            sslEnable = false;
        }
        else
        {
            server_address_cur = server_address_ssl;
            // logger_.LogInfo() << "MQTT: ssl_enable param set error ,use default ssl_enable: true";
        }

        if (sslEnable)
        {
            // check tls trust or key exist
            {
                ifstream tstore(trust_store);
                if (!tstore)
                {
                    // logger_.LogInfo() << "MQTT: The trust store file does not exist: '" << trust_store
                    //                   << "'.  Get a copy from "
                    //                   "\"paho.mqtt.c/test/ssl/test-root-ca.crt\"";
                    return;
                }

                ifstream kstore(key_store);
                if (!kstore)
                {
                    // logger_.LogInfo() << "MQTT: The key store file does not exist: '" << key_store
                    //                   << "'.  Get a copy from "
                    //                   "\"paho.mqtt.c/test/ssl/client.pem\"";
                    return;
                }
            }
        }

        // A client that just publishes normally doesn't need a persistent
        // session or Client ID unless it's using persistence, then the local
        // library requires an ID to identify the persistence files.
        // string	address  = (argc > 1) ? string(argv[1]) : server_address_cur,
        // 		clientID = (argc > 2) ? string(argv[2]) : CLIENT_ID;

        string address = server_address_cur, clientID = CLIENT_ID + "_" + uniq_id;

        logger_.LogDebug() << "MQTT: Initializing for server '" << address << "'...";

        // Create a client using MQTT v5
        mqtt::create_options createOpts(MQTTVERSION_5);
        // mqtt::async_client client(address, clientID, createOpts, PERSIST_DIR);
        mqtt_client = std::make_unique<mqtt::async_client>(address, clientID, createOpts, PERSIST_DIR + "_" + uniq_id);
        // mqtt_client = &client;
        //  won't use will mesage
        //  auto willmsg = mqtt::message(LWT_TOPIC, LWT_PAYLOAD, QOS2, true);
        mqtt_opts = mqtt::connect_options_builder()
                    .keep_alive_interval(std::chrono::seconds(2))
                    .clean_start(false)
                    .user_name("testuser")
                    .password("testpassword")
                    //.will(std::move(willmsg))
                    .finalize();

        if (sslEnable)
        {
            // Build the ssl options, including SSL and a LWT message.
            auto sslopts = mqtt::ssl_options_builder().trust_store(trust_store).key_store(key_store).error_handler([](const std::string &msg)
            {
                cout << "MQTT: SSL Error: " << msg << endl;
            }).finalize();

            mqtt_opts.set_ssl(std::move(sslopts));
        }

        // callback cb(*this);
        mqtt_client->set_callback(cb);
        logger_.LogDebug() << "MQTT: Callback set OK";

        if (!handle_)
        {
            handle_ = std::unique_ptr<std::thread>(new std::thread(
                [&]()
            {
                try {
                    logger_.LogInfo() << "[MQTTNode] Thread started";
                    while (!this->stop_flag_)
                    {
                        Process();
                    }

                    try
                    {
                        // Disconnect
                        logger_.LogDebug() << "MQTT: Disconnecting...";
                        if (mqtt_client)
                            if (mqtt_client->is_connected())
                                mqtt_client->disconnect()->wait();
                        logger_.LogDebug() << "MQTT:  ...OK";
                    }
                    catch (const mqtt::exception &exc)
                    {
                        logger_.LogDebug() << "MQTT: " << exc.what();
                    }
                    logger_.LogInfo() << "[MQTTNode] Thread exiting";
                } catch (const std::exception& e) {
                    logger_.LogInfo() << "[MQTTNode] Exception: " << e.what();
                } catch (...) {
                    logger_.LogInfo() << "[MQTTNode] Unknown exception";
                }
            }));
        }
        logger_.LogDebug() << "MQTT: start";
    }

    void Process()
    {
        // static bool mqtt_connected{false};

        if (mqtt_client->is_connected() == false)
        {
            mqtt_connected = false;
            try
            {
                logger_.LogDebug() << "MQTT : Connecting...";
                mqtt::token_ptr conntok = mqtt_client->connect(mqtt_opts);
                logger_.LogDebug() << "MQTT: Waiting for the connection...";
                conntok->wait();
                auto rsp = conntok->get_connect_response();

                // Make sure we were granted a v5 connection.
                if (rsp.get_mqtt_version() < MQTTVERSION_5)
                {
                    logger_.LogDebug() << "MQTT: Did not get an MQTT v5 "
                                       "connection. Disconnecting ..";
                    mqtt_client->disconnect()->wait();
                }
                logger_.LogDebug() << "MQTT: .. OK";
            }
            catch (const mqtt::exception &exc)
            {
                logger_.LogDebug() << ": " << exc.what();
            }
        }

        if (mqtt_client->is_connected() == true)
        {
            if (mqtt_connected == false)
            {
                // resubscribe here
                std::lock_guard<std::mutex> lock(mutex_);
                mqtt_connected = true;
                for (const auto &n : mqtt_handler_Map)
                {
                    const std::string vin_placeholder = "{VIN000000000000}";
                    std::string topic_name = n.first;
                    size_t pos = topic_name.find(vin_placeholder);
                    if(pos != std::string::npos)
                    {
                        ReplaceVINInString(topic_name, vin_, pos);
                    }
                    logger_.LogDebug() << "MQTT: Subscribe to " << n.first;
                    mqtt_client->subscribe(n.first, QOS2 /* subListener_ */);
                }
            }
        }
        OSA_sleepInMsec(1000);
    }

    void mqtt_subscibe(std::string topic_name, std::function<void(std::string)> topic_handler)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        logger_.LogInfo() << "MQTT: subscribing topic_name : '" << topic_name << "'";
        mqtt_handler_Map[topic_name].push_back(topic_handler);
        if (mqtt_client)
        {
            if (mqtt_client->is_connected() == true)
                mqtt_client->disconnect();
        }
    }

    void mqtt_publish(std::string topic_name, std::string value)
    {
        if (!mqtt_client)
        {
            return;
        }
        if (mqtt_connected == false)
        {
            return;
        }
        if (vin_ == "")
        {
            logger_.LogInfo() << "MQTT: VIN is not set, dropping message for topic: '" << topic_name << "'";
            return;
        }
        const std::string vin_placeholder = "{VIN000000000000}";
        size_t pos = topic_name.find(vin_placeholder);
        if(pos != std::string::npos){
            ReplaceVINInString(topic_name, vin_, pos);
            // topic_name.replace(pos, vin_placeholder.length(), vin_);
        }
        logger_.LogInfo() << "MQTT: topic_name : '" << topic_name << "', '" << value << "'";
        try
        {

            // Now try with a listener
            logger_.LogDebug() << "MQTT: Sending message...";
            mqtt::message_ptr pubmsg = mqtt::make_message(topic_name, value);
            if (topic_name.find("debug") != std::string::npos)
            {
                pubmsg->set_qos(QOS0);
            }
            else
            {
                pubmsg->set_qos(QOS2);
            }
            mqtt_client->publish(pubmsg, nullptr, pubListener_);
            logger_.LogDebug() << "MQTT:  ...OK";
        }

        catch (const mqtt::exception &exc)
        {
            logger_.LogDebug() << "MQTT: " << exc.what();
        }
    }

    void mqtt_event(std::string topic, std::string value)
    {
        const std::string vin_placeholder = "{VIN000000000000}";
        size_t pos = topic.find(vin_);
        if(pos != std::string::npos){
            ReplaceVINInString(topic, vin_placeholder, pos);
        }
        logger_.LogDebug() << "MQTT: event triggered : '" << topic << "'";
        if (mqtt_handler_Map.count(topic))
        {
            for (auto &handler : mqtt_handler_Map[topic])
            {
                handler(value); // Call each function in the vector
            }
            logger_.LogDebug() << "MQTT: handled ok.";
        }
    }

    ara::log::Logger &mqtt_log()
    {
        return logger_;
    }

    std::map<std::string, std::string> mqtt_config_file(const std::string &filename)
    {
        std::map<std::string, std::string> config;
        std::ifstream file(filename);

        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                // Remove Spaces at the beginning and end of a line
                line.erase(0, line.find_first_not_of(" \t"));
                line.erase(line.find_last_not_of(" \t") + 1);

                // Ignore empty lines and comment lines
                if (line.empty() || line[0] == '#')
                {
                    continue;
                }

                // Find the position of the equal sign
                size_t equalSignPos = line.find('=');
                if (equalSignPos != std::string::npos)
                {
                    std::string key = line.substr(0, equalSignPos);
                    std::string value = line.substr(equalSignPos + 1);
                    config[key] = value;
                }
            }
            file.close();
        }
        else
        {
            logger_.LogInfo() << ": Can not open file : " << filename;
        }

        return config;
    }

    void set_vin(std::string vin){
        vin_ = vin;
        mqtt_connected = false;
        logger_.LogInfo() << ": Vin is set to : " << vin_;
    }

    std::string get_vin(){
        return vin_;
    }

    void Stop();

private:
    // ADASMainAppNode &parent;
    ara::log::Logger &logger_{ara::log::CreateLogger("MQTT", "MQTT", ara::log::LogLevel::kDebug)};
    std::unique_ptr<std::thread> handle_{nullptr};
    bool stop_flag_{false};

    std::unique_ptr<mqtt::async_client> mqtt_client;

    std::mutex mutex_;
    std::map<std::string, std::vector<std::function<void(std::string)>>> mqtt_handler_Map{};
    action_listener subListener_{*this};
    mqtt::connect_options mqtt_opts;
    bool mqtt_connected{false};
    callback cb{*this};

    // Listener as a member
    class MyPublishListener : public virtual mqtt::iaction_listener {
    public: // Make sure methods are public if accessed from MqttManager
        void on_success(const mqtt::token& tok) override {
            std::cout << "✅ Publish success (token: " << tok.get_message_id() << ")\n";
        }

        void on_failure(const mqtt::token& tok) override {
            std::cerr << "❌ Publish failed (token: " << tok.get_message_id() << ")\n";
            // You might want to log tok.get_reason_code() or tok.get_error_str()
        }
    };
    MyPublishListener pubListener_;

    std::string vin_;
};
