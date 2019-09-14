
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv4-global-routing.h"

#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/applications-module.h"
#include "ns3/node-container.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include "ns3/node-list.h"

#include <glog/logging.h>
#include "App.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ABIExample");

void checkIp(void)
{
  //freopen("/dev/tty", "w", stdout);
  // LOG(INFO) << "check ip:\n";
  for (uint32_t i = 1; i < NodeList::GetNNodes(); i++)
  {
    Ptr<Node> node = NodeList::GetNode(i);
    Ptr<Ipv4L3Protocol> l3p = node->GetObject<Ipv4L3Protocol>();
    std::cout << "Node: " << i << " DevNums: " << l3p->GetNInterfaces() - 1 << std::endl;
    for (uint32_t j = 1; j < l3p->GetNInterfaces(); j++)
    {
      Ipv4Address ipAddress = l3p->GetInterface(j)->GetAddress(0).GetLocal();
      std::cout << " Dev: " << j << " ip: " << ipAddress << "\n";
    }
  }
}

int abc = 1;

void buildRouter(NodeContainer &routers)
{
  uint32_t from[] = {1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6, 8, 9, 10};
  uint32_t to[] = {2, 5, 6, 12, 6, 9, 7, 10, 11, 7, 8, 7, 10, 12, 11};
  for (uint32_t node_index = 0; node_index < 15; node_index++)
  {
    NodeContainer ctmp = NodeContainer(routers.Get(from[node_index]), routers.Get(to[node_index]));
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer devtemp = p2p.Install(ctmp);
    std::string ipaddr = "10.10.";
    ipaddr += std::string(std::to_string(10 + node_index));
    ipaddr += std::string(".0");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase(ipaddr.c_str(), "255.255.255.0");
    ipv4.Assign(devtemp);
    std::string names = "scratch/data/router";
    names += std::to_string(from[node_index]) + std::string("--") + std::to_string(to[node_index]) + std::string(".");
    p2p.EnablePcap(names.c_str(), devtemp.Get(0), true);
  }
}

void buildHost(NodeContainer &hosts)
{
  uint32_t from[] = {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
  uint32_t to[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  for (uint32_t node_index = 0; node_index < 12; node_index++)
  {
    NodeContainer ctmp = NodeContainer(hosts.Get(from[node_index]), hosts.Get(to[node_index]));
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("1ms"));
    NetDeviceContainer devtemp = p2p.Install(ctmp);
    std::string ipaddr = "10.10.";
    ipaddr += std::string(std::to_string(100 + node_index));
    ipaddr += std::string(".0");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase(ipaddr.c_str(), "255.255.255.0");
    ipv4.Assign(devtemp);
    std::string names = "scratch/data/host";
    names += std::to_string(from[node_index]) + std::string("--") + std::to_string(to[node_index]) + std::string(".");
    p2p.EnablePcap(names.c_str(), devtemp.Get(0), true);
  }
}

NodeContainer buildTopo(void)
{
  NodeContainer allnodes;
  allnodes.Create(12 + 1 + 12);
  //set up ip stacks
  InternetStackHelper internet;
  internet.Install(allnodes);
  //set up nodes
  buildRouter(allnodes);
  buildHost(allnodes);

  checkIp();

  return allnodes;
}

void buildApps(void)
{
  if (0)
  {
    Ptr<Node> node = NodeList::GetNode(13);
    Ptr<Ipv4L3Protocol> l3p = node->GetObject<Ipv4L3Protocol>();
    Ipv4Address ipAddress = l3p->GetInterface(1)->GetAddress(0).GetLocal();
    LOG(INFO) << "IP address for client: " << ipAddress;
  }
  Ptr<Socket> server = buildServer();
  Ptr<Socket> client = buildClient();

  Simulator::Schedule(Seconds(1), &send, client);
  //Simulator::Schedule(Seconds(3), &send, client);
  //Simulator::Schedule(Seconds(5), &send, client);
  //Simulator::Schedule(Seconds(7), &send, client);
  //Simulator::Schedule(Seconds(9), &send, client);
}

void enableRoutingSystem(void)
{
  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
  ns3::PacketMetadata::Enable();
}

int main(int argc, char *argv[])
{
  LogComponentEnable("ABIExample", LOG_LEVEL_ALL);
  LogComponentEnable("APPEXAMPLE", LOG_LEVEL_ALL);
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  Config::SetDefault("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue(true));

  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd;
  cmd.Parse(argc, argv);
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));

  NodeContainer cc = buildTopo();
  LOG(INFO) << cc.GetN();

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  enableRoutingSystem();
  buildApps();

  Simulator::Stop(Seconds(10));

  // Trace routing tables
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>("scratch/data/abilene.routes", std::ios::out);
  g.PrintRoutingTableAllAt(Seconds(0), routingStream);

  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");
}
