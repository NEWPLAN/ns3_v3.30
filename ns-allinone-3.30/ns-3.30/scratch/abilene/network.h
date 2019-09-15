#ifndef __NETWORK_H__H_NEWPLAN
#define __NETWORK_H__H_NEWPLAN

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
using namespace ns3;
NodeContainer buildTopo(void);
void trackPackets(float time_space = 1);
//void enableRoutingSystem(void);

#endif