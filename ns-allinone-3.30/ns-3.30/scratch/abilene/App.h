#ifndef MY_APP_H__
#define MY_APP_H__
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/applications-module.h"
#include "ns3/node-container.h"
using namespace ns3;
Ptr<Socket> buildServer(uint32_t nodeid = 13, uint16_t port = 10086);
Ptr<Socket> buildClient(uint32_t clientid = 23, uint32_t serverid = 23, uint16_t port = 10086);
void send(Ptr<Socket> sock, uint32_t nums);

void buildServerOnOff(uint32_t nodeid = 13, uint16_t port = 10086);
void buildClientOnOff(uint32_t clientid = 23, uint32_t serverid = 23, uint16_t port = 10086);
#endif