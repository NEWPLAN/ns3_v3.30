//https://blog.csdn.net/weixin_42314534/article/details/88322853

#include <iostream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/csma-helper.h"

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("EleventhScriptExample");

#define DATALEN 500

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
	NS_LOG_INFO("aaaaaaaaaaa");
	NS_LOG_INFO(sock->GetErrno());
}

//Socket：服务端socket，unknownddress：客户端地址
static bool
ConnectionRequest(Ptr<Socket> Socket, const Address &unknownAddress)
{
	NS_LOG_INFO("ConnectionRequest");
	NS_LOG_INFO(Socket->GetSocketType());
	NS_LOG_INFO(Socket);
	NS_LOG_INFO(unknownAddress);
	return true;
}

//Socket: 新socket，unknownAddress：客户端地址
static void
NewConnectionCreated(Ptr<Socket> Socket, const Address &unknownAddress)
{
	NS_LOG_INFO("newConnectionCreated");
	NS_LOG_INFO(Socket->GetSocketType());
	NS_LOG_INFO(Socket);
	NS_LOG_INFO(unknownAddress);
	//这里新socket需要设置recv回调
	Socket->SetRecvCallback(MakeCallback(&recvCallback));
}

int main(int argc, char *argv[])
{
	LogComponentEnable("EleventhScriptExample", LOG_LEVEL_ALL);
	NodeContainer nodes;
	nodes.Create(5);

	InternetStackHelper stack;
	stack.Install(nodes);

	CsmaHelper csmaHelper;
	NetDeviceContainer csmaNetDevices = csmaHelper.Install(nodes);

	csmaHelper.EnablePcap("/home/newplan/project/ns/ns3_v3.30/ns-allinone-3.30/ns-3.30/scratch/haha", csmaNetDevices.Get(0), true);
	Ipv4AddressHelper addressHelper;
	addressHelper.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = addressHelper.Assign(csmaNetDevices);

	//server sockets
	TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
	Ptr<Socket> server = Socket::CreateSocket(nodes.Get(0), tid);
	server->SetAttribute("SegmentSize", UintegerValue(1400));
	InetSocketAddress addr = InetSocketAddress(Ipv4Address::GetAny(), 10086);
	NS_LOG_INFO(addr);
	server->Bind(addr);
	//注意这里
	server->Listen();
	NS_LOG_INFO(server);
	//注意这里
	server->SetAcceptCallback(MakeCallback(&ConnectionRequest), MakeCallback(&NewConnectionCreated));

	//client sockets
	Ptr<Socket> client = Socket::CreateSocket(nodes.Get(4), tid);
	client->SetAttribute("SegmentSize", UintegerValue(1400));
	InetSocketAddress serverAddr = InetSocketAddress(interfaces.GetAddress(0), 10086);
	client->Connect(serverAddr);
	NS_LOG_INFO(client);
	//	client->Send(Create<Packet>(500));

	//	client->Close();

	Simulator::Schedule(Seconds(1), &send, client);
	Simulator::Schedule(Seconds(3), &send, client);
	Simulator::Schedule(Seconds(5), &send, client);
	Simulator::Schedule(Seconds(7), &send, client);
	Simulator::Stop(Seconds(9));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}