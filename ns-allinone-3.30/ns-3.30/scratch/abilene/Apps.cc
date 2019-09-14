#include "App.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include "ns3/node-list.h"
#include <iostream>
#include <glog/logging.h>

using namespace ns3;
using namespace std;

#define DATALEN 500
NS_LOG_COMPONENT_DEFINE("APPEXAMPLE");
//回调函数
static void recvCallback(Ptr<Socket> sock)
{
    char databuf[DATALEN] = {0};

    Ptr<Packet> packet = sock->Recv();
    packet->CopyData((uint8_t *)databuf, DATALEN);
    printf("%s\n", databuf);
    cout << "pkt path: " << packet->GetPath() << ",size:" << packet->GetSize() << endl;
}
void send(Ptr<Socket> sock)
{
    const char *data = "hello world!";
    std::string path = "12344354";
    Ptr<Packet> pkt = Create<Packet>((const uint8_t *)data, DATALEN);
    pkt->SetPath(path);
    sock->Send(pkt);
    std::cout << "path" << std::endl;
    NS_LOG_INFO("aaaaaaaaaaa");
    NS_LOG_INFO(sock->GetErrno());
}

//Socket：服务端socket，unknownddress：客户端地址
static bool ConnectionRequest(Ptr<Socket> Socket, const Address &unknownAddress)
{
    NS_LOG_INFO("ConnectionRequest");
    NS_LOG_INFO(Socket->GetSocketType());
    NS_LOG_INFO(Socket);
    NS_LOG_INFO(unknownAddress);
    return true;
}

//Socket: 新socket，unknownAddress：客户端地址
static void NewConnectionCreated(Ptr<Socket> Socket, const Address &unknownAddress)
{
    LOG(INFO) << "Make new connections...";
    NS_LOG_INFO("newConnectionCreated");
    NS_LOG_INFO(Socket->GetSocketType());
    NS_LOG_INFO(Socket);
    NS_LOG_INFO(unknownAddress);
    //这里新socket需要设置recv回调
    Socket->SetRecvCallback(MakeCallback(&recvCallback));
}

Ptr<Socket> buildClient(void)
{
    //client sockets

    Ptr<Node> local_node = NodeList::GetNode(23);
    Ptr<Node> server_node = NodeList::GetNode(13);

    TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
    Ptr<Socket> client = Socket::CreateSocket(local_node, tid);
    //client->SetAttribute("SegmentSize", UintegerValue(1400));
    InetSocketAddress serverAddr = InetSocketAddress(server_node->GetObject<Ipv4L3Protocol>()->GetInterface(1)->GetAddress(0).GetLocal(), 10086);
    LOG(INFO) << server_node->GetObject<Ipv4L3Protocol>()->GetInterface(1)->GetAddress(0).GetLocal();
    if (0 != client->Connect(serverAddr))
    {
        LOG(INFO) << client->GetErrno();
    }
    return client;
}
Ptr<Socket> buildServer(void)
{
    //server sockets
    TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
    Ptr<Socket> server = Socket::CreateSocket(NodeList::GetNode(13), tid);
    //server->SetAttribute("SegmentSize", UintegerValue(1400));
    InetSocketAddress addr = InetSocketAddress(Ipv4Address::GetAny(), 10086);
    LOG(INFO) << addr << std::endl;
    if (0 != server->Bind(addr))
    {
        LOG(INFO) << server->GetErrno();
    }
    //注意这里
    if (0 != server->Listen())
    {
        LOG(INFO) << server->GetErrno();
    }
    //注意这里
    server->SetAcceptCallback(MakeCallback(&ConnectionRequest), MakeCallback(&NewConnectionCreated));
    return server;
}