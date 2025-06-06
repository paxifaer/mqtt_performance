// async_publish.cpp
//
// This is a Paho MQTT C++ client, sample application.
//
// It's an example of how to send messages as an MQTT publisher using the
// C++ asynchronous client interface.
//
// The sample demonstrates:
//  - Connecting to an MQTT server/broker
//  - Using a connect timeout
//  - Publishing messages
//  - Default file persistence
//  - Last will and testament
//  - Using asynchronous tokens
//  - Implementing callbacks and action listeners
//

/*******************************************************************************
 * Copyright (c) 2013-2023 Frank Pagliughi <fpagliughi@mindspring.com>
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Frank Pagliughi - initial implementation and documentation
 *******************************************************************************/

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <queue>
#include "mqtt/async_client.h"

using namespace std;
using namespace std::chrono;

const string DFLT_SERVER_URI{"mqtt://localhost:1883"};
const string CLIENT_ID{"paho_cpp_async_publish"};

// const mqtt::persistence_type PERSIST_DIR{"./persist"};

const string TOPIC{"hello"};

const string PAYLOAD1{"Hello World!"};
const string PAYLOAD2{"Hi there!"};
const string PAYLOAD3{"Is anyone listening?"};
const string PAYLOAD4{"Someone is always listening."};

const string LWT_PAYLOAD{"Last will and testament."};

const int QOS = 1;

const auto TIMEOUT = std::chrono::seconds(10);

/////////////////////////////////////////////////////////////////////////////

/**
 * A callback class for use with the main MQTT client.
 */
class callback : public virtual mqtt::callback
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

/////////////////////////////////////////////////////////////////////////////

/**
 * A base action listener.
 */
class action_listener : public virtual mqtt::iaction_listener
{
protected:
    void on_failure(const mqtt::token& tok) override
    {
        cout << "\tListener failure for token: " << tok.get_message_id() << endl;
    }

    void on_success(const mqtt::token& tok) override
    {
        cout << "\tListener success for token: " << tok.get_message_id() << endl;
    }
};

/////////////////////////////////////////////////////////////////////////////

/**
 * A derived action listener for publish events.
 */
class delivery_action_listener : public action_listener
{
    atomic<bool> done_;

    void on_failure(const mqtt::token& tok) override
    {
        action_listener::on_failure(tok);
        done_ = true;
    }

    void on_success(const mqtt::token& tok) override
    {
        action_listener::on_success(tok);
        done_ = true;
    }

public:
    delivery_action_listener() : done_(false) {}
    bool is_done() const { return done_; }
};

/////////////////////////////////////////////////////////////////////////////

std::vector<std::string> mutiple_topic(int num)
{
   std::vector<std::string> vec;
   for(int i=0; i<num; i++)
   {
      vec.emplace_back("topic"+std::to_string(num));
   }
   return vec;
}

void mutiple_pub(mqtt::async_client &client)
{
   
}

int main(int argc, char* argv[])
{
    // A client that just publishes normally doesn't need a persistent
    // session or Client ID unless it's using persistence, then the local
    // library requires an ID to identify the persistence files.

    string serverURI = (argc > 1) ? string{argv[1]} : DFLT_SERVER_URI,
           clientID = (argc > 2) ? string{argv[2]} : CLIENT_ID;

    cout << "Initializing for server '" << serverURI << "'..." << endl;
    mqtt::async_client client(serverURI, clientID);

    callback cb;
    client.set_callback(cb);

    auto connOpts = mqtt::connect_options_builder()
                        .connect_timeout(5s)
                        .clean_session()
                        .will(mqtt::message(TOPIC, LWT_PAYLOAD, QOS, false))
                        .finalize();

    cout << "  ...OK" << endl;

    try {
        // cout << "\nConnecting..." << endl;
        // mqtt::token_ptr conntok = client.connect(connOpts);
        // cout << "Waiting for the connection..." << endl;
        // conntok->wait();
        // cout << "  ...OK" << endl;

        // // First use a message pointer.

        // cout << "\nSending message..." << endl;
        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, PAYLOAD1);
        pubmsg->set_qos(QOS);
        client.publish(pubmsg);
        // cout << "  ...OK" << endl;

        // // Now try with itemized spublish.

        // cout << "\nSending next message..." << endl;
        // mqtt::delivery_token_ptr pubtok;
        // pubtok = client.publish(TOPIC, PAYLOAD2, QOS, false);
        // cout << "  ...with token: " << pubtok->get_message_id() << endl;
        // cout << "  ...for message with " << pubtok->get_message()->get_payload().size()
        //      << " bytes" << endl;
        // pubtok->wait_for(TIMEOUT);
        // cout << "  ...OK" << endl;

        // // Now try with a listener

        // cout << "\nSending next message..." << endl;
        // action_listener listener;
        // pubmsg = mqtt::make_message(TOPIC, PAYLOAD3);
        // pubtok = client.publish(pubmsg, nullptr, listener);
        // pubtok->wait();
        // cout << "  ...OK" << endl;

        // // Finally try with a listener, but no token

        // cout << "\nSending final message..." << endl;
        // delivery_action_listener deliveryListener;
        // pubmsg = mqtt::make_message(TOPIC, PAYLOAD4);
        // client.publish(pubmsg, nullptr, deliveryListener);

        // while (!deliveryListener.is_done()) {
        //     this_thread::sleep_for(std::chrono::milliseconds(100));
        // }
        // cout << "OK" << endl;

        // // Double check that there are no pending tokens

        // auto toks = client.get_pending_delivery_tokens();
        // if (!toks.empty())
        //     cout << "Error: There are pending delivery tokens!" << endl;

        // // Disconnect
        // cout << "\nDisconnecting..." << endl;
        // client.disconnect()->wait();
        // cout << "  ...OK" << endl;
    }
    catch (const mqtt::exception& exc) {
        cerr << exc.what() << endl;
        return 1;
    }

    return 0;
}
