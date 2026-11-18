/*
static FIFOs accessible from all processes

uses: libboost-dev library
*/

#include <boost/lockfree/spsc_queue.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "fifo.h"

using string = std::string;

struct Message {
    string topic;
    string payload;
    Message(const string& t1, const string& t2) : topic(t1), payload(t2) {}
};

constexpr size_t queue_size = 10000;
boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<queue_size>> mqttqueue;

void send_to_mqtt(const string& topic, const string& payload)
{
    if (!mqttqueue.push(Message(topic,payload)))
        printf("MQTT queue full, message dropped: %s\n", topic.c_str());
}

bool read_from_readbatt(string& topic, string& payload)
{
    Message message("", "");

    bool ret = mqttqueue.pop(message);
    if(ret) {
        topic = message.topic;
        payload = message.payload;
    }

    return ret;
}

void flush_mqtt_queue()
{
    Message message("", "");
    int count = 0;
    while (mqttqueue.pop(message)) count++;
    if (count > 0)
        printf("MQTT queue flushed (%d messages discarded)\n", count);
}