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
#include "util.h"
#include "flowGenerator.h"

using namespace ns3;
using namespace std;

#define DATALEN 100000
NS_LOG_COMPONENT_DEFINE("APPEXAMPLE");
//回调函数
uint64_t nums = 0;
static void recvCallback(Ptr<Socket> sock)
{
    //char databuf[DATALEN] = {0};

    Ptr<Packet> packet = sock->Recv();
    nums += packet->GetSize();
    //packet->CopyData((uint8_t *)databuf, DATALEN);
    //printf("%s\n", databuf);
    LOG_EVERY_N(INFO, 1000) << "RECV: size:" << nums << endl;
}

void send(Ptr<Socket> sock, uint32_t nums)
{
    LOG_EVERY_N(INFO, 100) << "Send flow Size: " << nums;
    //const char *data = "hello world!";
    //std::string path = "12344354";
    //if (maxnum < sock->GetTxAvailable())
    //    maxnum = sock->GetTxAvailable();
    //LOG(INFO) << "can sent numbers: " << sock->GetTxAvailable() << ", MAX: " << maxnum;
    //GetTxAvailable
    //Ptr<Packet> pkt = Create<Packet>((const uint8_t *)data, DATALEN);
    Ptr<Packet> pkt = Create<Packet>(nums);
    //pkt->SetPath(path);
    sock->Send(pkt);
    //std::cout << "path" << std::endl;
    //NS_LOG_INFO("aaaaaaaaaaa");
    //NS_LOG_INFO(sock->GetErrno());
}
/*
void writeUntilBufferFull(Ptr<Socket> localSocket, uint32_t txSpace)
{
    while (currentTxBytes < totalTxBytes && localSocket->GetTxAvailable() > 0)
    {
        uint32_t left = totalTxBytes - currentTxBytes;
        uint32_t dataOffset = currentTxBytes % writeSize;
        uint32_t toWrite = writeSize - dataOffset;
        toWrite = std::min(toWrite, left);
        toWrite = std::min(toWrite, localSocket->GetTxAvailable());
        int amountSent = localSocket->Send(&data[dataOffset], toWrite, 0);
        if (amountSent < 0)
        {
            // we will be called again when new tx space becomes available.
            return;
        }
        currentTxBytes += amountSent;
    }
    localSocket->Close();
}
*/

static void sleepForATime(Ptr<Socket> sock)
{
    send(sock, genFlowSize());
}

static void sendCallback(Ptr<Socket> sock, uint32_t nums)
{
    LOG_EVERY_N(INFO, 1000) << nums << ", Send callback: " << Simulator::Now().GetSeconds();
    //send(sock, GenFlowSize());
    Simulator::Schedule(Seconds(genFlowInterval() / 1000), &sleepForATime, sock);
}

//Socket：服务端socket，unknownddress：客户端地址
static bool ConnectionRequest(Ptr<Socket> Socket, const Address &unknownAddress)
{
    LOG(INFO) << "ConnectionRequest " << unknownAddress;
    //NS_LOG_INFO("ConnectionRequest");
    //NS_LOG_INFO(Socket->GetSocketType());
    //NS_LOG_INFO(Socket);
    //NS_LOG_INFO(unknownAddress);
    return true;
}

//Socket: 新socket，unknownAddress：客户端地址
static void NewConnectionCreated(Ptr<Socket> Socket, const Address &unknownAddress)
{
    LOG(INFO) << "newConnectionCreated from Server: " << unknownAddress;
    //NS_LOG_INFO("newConnectionCreated");
    //NS_LOG_INFO(Socket->GetSocketType());
    //NS_LOG_INFO(Socket);
    //NS_LOG_INFO(unknownAddress);
    //这里新socket需要设置recv回调
    Socket->SetRecvCallback(MakeCallback(&recvCallback));
}

Ptr<Socket> buildClient(uint32_t clientid, uint32_t serverid, uint16_t port)
{
    //client sockets

    Ptr<Node> local_node = NodeList::GetNode(clientid);
    Ptr<Node> server_node = NodeList::GetNode(serverid);

    TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
    Ptr<Socket> client = Socket::CreateSocket(local_node, tid);
    //client->SetAttribute("SegmentSize", UintegerValue(1400));
    InetSocketAddress serverAddr = InetSocketAddress(server_node->GetObject<Ipv4L3Protocol>()->GetInterface(1)->GetAddress(0).GetLocal(), port);
    LOG(INFO) << server_node->GetObject<Ipv4L3Protocol>()->GetInterface(1)->GetAddress(0).GetLocal();
    if (0 != client->Connect(serverAddr))
    {
        LOG(INFO) << client->GetErrno();
    }
    client->SetSendCallback(MakeCallback(&sendCallback));
    return client;
}
Ptr<Socket> buildServer(uint32_t nodeid, uint16_t port)
{
    //server sockets
    TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
    Ptr<Socket> server = Socket::CreateSocket(NodeList::GetNode(nodeid), tid);
    //server->SetAttribute("SegmentSize", UintegerValue(1400));
    InetSocketAddress addr = InetSocketAddress(Ipv4Address::GetAny(), port);
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

void buildClientOnOff(uint32_t clientid, uint32_t serverid, uint16_t port)
{
    // Create the OnOff application to send TCP datagrams of size
    // 210 bytes at a rate of 448 Kb/s
    LOG(INFO) << "Create Applications.";

    OnOffHelper onoff("ns3::TcpSocketFactory",
                      InetSocketAddress(NodeList::GetNode(serverid)->GetObject<Ipv4L3Protocol>()->GetInterface(1)->GetAddress(0).GetLocal(), port));
    onoff.SetConstantRate(DataRate("20kbps"));
    onoff.SetAttribute("PacketSize", UintegerValue(1000));

    ApplicationContainer apps = onoff.Install(NodeList::GetNode(clientid));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));
}

void buildServerOnOff(uint32_t nodeid, uint16_t port)
{
    LOG(INFO) << "Create Server Port " << port;
    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps = sink.Install(NodeList::GetNode(nodeid));
    apps.Start(Seconds(0));
    //apps.Stop(Seconds(10.0));
}