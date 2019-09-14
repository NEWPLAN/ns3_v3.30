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
Ptr<Socket> buildServer(void);
Ptr<Socket> buildClient(void);
void send(Ptr<Socket> sock);
#endif