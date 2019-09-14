//https://blog.csdn.net/weixin_42314534/article/details/88322853

#include <iostream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/csma-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-helper.h"

#include "ns3/ipv4-global-routing-helper.h"
#include "glog/logging.h"

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TCPTestExample");

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
	LOG(INFO) << "Sent packets: " << path;
	LOG(INFO) << "Socket error: " << sock->GetErrno();
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
	LogComponentEnable("TCPTestExample", LOG_LEVEL_ALL);
	NodeContainer nodes;
	nodes.Create(5);

	InternetStackHelper stack;
	stack.Install(nodes);

	PointToPointHelper p2phelper; //new added
	NodeContainer n0n1 = NodeContainer(nodes.Get(0), nodes.Get(1));
	NetDeviceContainer d01 = p2phelper.Install(n0n1);
	NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
	NetDeviceContainer d02 = p2phelper.Install(n0n2);
	NodeContainer n1n3 = NodeContainer(nodes.Get(1), nodes.Get(3));
	NetDeviceContainer d13 = p2phelper.Install(n1n3);
	NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));
	NetDeviceContainer d23 = p2phelper.Install(n2n3);
	NodeContainer n3n4 = NodeContainer(nodes.Get(3), nodes.Get(4));
	NetDeviceContainer d34 = p2phelper.Install(n3n4);
	p2phelper.EnablePcap("/home/newplan/project/ns/ns3_v3.30/ns-allinone-3.30/ns-3.30/scratch/data/tcp", d34.Get(0), true);
	{
		d01.Get(0)->SetDeviceName(std::string("0--1"));
		d01.Get(1)->SetDeviceName(std::string("1--0"));

		d02.Get(0)->SetDeviceName(std::string("0--2"));
		d02.Get(1)->SetDeviceName(std::string("2--0"));

		d13.Get(0)->SetDeviceName(std::string("1--3"));
		d13.Get(1)->SetDeviceName(std::string("3--1"));

		d23.Get(0)->SetDeviceName(std::string("2--3"));
		d23.Get(1)->SetDeviceName(std::string("3--2"));

		d34.Get(0)->SetDeviceName(std::string("3--4"));
		d34.Get(1)->SetDeviceName(std::string("4--3"));
	}

	Ipv4AddressHelper ip01, ip02, ip13, ip23, ip34;
	ip01.SetBase("10.168.10.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ip01.Assign(d01);
	ip02.SetBase("10.168.11.0", "255.255.255.0");
	ip02.Assign(d02);
	ip13.SetBase("10.168.12.0", "255.255.255.0");
	ip13.Assign(d13);
	ip23.SetBase("10.168.13.0", "255.255.255.0");
	ip23.Assign(d23);
	ip34.SetBase("10.168.14.0", "255.255.255.0");
	ip34.Assign(d34);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	{
		// Trace routing tables
		Ipv4GlobalRoutingHelper g;
		Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>("/home/newplan/project/ns/ns3_v3.30/ns-allinone-3.30/ns-3.30/scratch/tcpexp.routes", std::ios::out);
		g.PrintRoutingTableAllAt(Seconds(0), routingStream);
	}
	ns3::PacketMetadata::Enable();

	/*
	CsmaHelper csmaHelper;
	NetDeviceContainer csmaNetDevices = csmaHelper.Install(nodes);

	csmaHelper.EnablePcap("/home/newplan/project/ns/ns3_v3.30/ns-allinone-3.30/ns-3.30/scratch/data/tcp", csmaNetDevices.Get(0), true);
	Ipv4AddressHelper addressHelper;
	addressHelper.SetBase("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = addressHelper.Assign(csmaNetDevices);
*/
	//server sockets
	TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
	Ptr<Socket> server = Socket::CreateSocket(nodes.Get(0), tid);
	server->SetAttribute("SegmentSize", UintegerValue(1400));
	InetSocketAddress addr = InetSocketAddress(Ipv4Address::GetAny(), 10086);
	//NS_LOG_INFO(addr);
	LOG(INFO) << server->Bind(addr);
	//注意这里
	LOG(INFO) << server->Listen();
	//NS_LOG_INFO(server);
	//注意这里
	server->SetAcceptCallback(MakeCallback(&ConnectionRequest), MakeCallback(&NewConnectionCreated));

	//client sockets
	Ptr<Socket> client = Socket::CreateSocket(nodes.Get(4), tid);
	client->SetAttribute("SegmentSize", UintegerValue(1400));
	InetSocketAddress serverAddr = InetSocketAddress(interfaces.GetAddress(0), 10086);
	LOG(INFO) << "intAddress: " << interfaces.GetAddress(0);
	if (0 != client->Connect(serverAddr))
	{
		LOG(INFO) << client->GetErrno();
	}

	{
		//Ask for ASCII and pcap traces of network traffic
		AsciiTraceHelper ascii;
		p2phelper.EnableAsciiAll(ascii.CreateFileStream("scratch/data/mytcp.tr"));
		p2phelper.EnablePcapAll("scratch/data/mytcp");
	}
	//NS_LOG_INFO(client);
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