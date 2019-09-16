
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
#include "network.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ABIExample");

void buildApps(void)
{
  if (0)
  {
    Ptr<Node> node = NodeList::GetNode(13);
    Ptr<Ipv4L3Protocol> l3p = node->GetObject<Ipv4L3Protocol>();
    Ipv4Address ipAddress = l3p->GetInterface(1)->GetAddress(0).GetLocal();
    LOG(INFO) << "IP address for client: " << ipAddress;
  }

  /* Ptr<Socket> server = buildServer(13, 1006);
  Ptr<Socket> client = buildClient(23, 13, 1006);
  Ptr<Socket> server2 = buildServer(23, 1006);
  Ptr<Socket> client2 = buildClient(13, 23, 1006);

  Simulator::Schedule(Seconds(1), &send, client);
  */
  //Simulator::Schedule(Seconds(1.2), &send, client2);
  for (uint16_t portnum = 100; portnum < 105; portnum++)
  {
    for (uint32_t sid = 13; sid < 25; sid++)
    {
      //buildServer(sid, portnum);
      buildServerOnOff(sid, portnum);
      for (uint32_t did = 13; did < 25; did++)
      {
        if (sid == did)
          continue;
        Ptr<Socket> client = buildClient(did, sid, portnum);
        //buildClientOnOff(did, sid, portnum);
        Simulator::Schedule(Seconds(1), &send, client, 100);
      }
    }
  }
  //Simulator::Schedule(Seconds(3), &send, client);
  //Simulator::Schedule(Seconds(5), &send, client);
  //Simulator::Schedule(Seconds(7), &send, client);
  //Simulator::Schedule(Seconds(9), &send, client);
}
#include "util.h"
#include "flowGenerator.h"
int main(int argc, char *argv[])
{
  setupFLowGenerator();
  // while (1)
  // {
  //   LOG(INFO) << genFlowInterval() << ", " << genFlowSize();
  // }
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
  //enableRoutingSystem();
  buildApps();

  Simulator::Schedule(Seconds(0.2), &trackPackets, 0.1);

  Simulator::Schedule(Seconds(0.2), &trackTMs, 0.01);
  Simulator::Stop(Seconds(100000));

  /*
  // Trace routing tables
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>("scratch/data/abilene.routes", std::ios::out);
  g.PrintRoutingTableAllAt(Seconds(0), routingStream);
*/
  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");
}
