#include "network.h"
#include <vector>

std::vector<Ptr<NetDevice>> tracking_List;
std::vector<Ptr<NetDevice>> devList;

static void checkIp(void)
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

//static int abc = 1;

static void buildRouter(NodeContainer &routers)
{
    uint32_t from[] = {1, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 6, 8, 9, 10};
    uint32_t to[] = {2, 5, 6, 12, 6, 9, 7, 10, 11, 7, 8, 7, 10, 12, 11};
    for (uint32_t node_index = 0; node_index < 15; node_index++)
    {
        NodeContainer ctmp = NodeContainer(routers.Get(from[node_index]), routers.Get(to[node_index]));
        PointToPointHelper p2p;
        //if (node_index == 0)
        //    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
        //else
        p2p.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
        p2p.SetChannelAttribute("Delay", StringValue("1ms"));
        NetDeviceContainer devtemp = p2p.Install(ctmp);
        std::string ipaddr = "10.10.";
        ipaddr += std::string(std::to_string(10 + node_index));
        ipaddr += std::string(".0");
        Ipv4AddressHelper ipv4;
        ipv4.SetBase(ipaddr.c_str(), "255.255.255.0");
        ipv4.Assign(devtemp);
        std::string names = "scratch/data/router";
        names += std::to_string(from[node_index]) + std::string("--") + std::to_string(to[node_index]) + std::string(".");
        devtemp.Get(0)->SetDeviceName(std::to_string(from[node_index]) + std::string("--") + std::to_string(to[node_index]));
        devtemp.Get(1)->SetDeviceName(std::to_string(to[node_index]) + std::string("--") + std::to_string(from[node_index]));
        //p2p.EnablePcap(names.c_str(), devtemp.Get(0), true);
        devList.push_back(devtemp.Get(0));
        devList.push_back(devtemp.Get(1));
    }
}

static void buildHost(NodeContainer &hosts)
{
    uint32_t from[] = {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    uint32_t to[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    for (uint32_t node_index = 0; node_index < 12; node_index++)
    {
        NodeContainer ctmp = NodeContainer(hosts.Get(from[node_index]), hosts.Get(to[node_index]));
        PointToPointHelper p2p;
        p2p.SetDeviceAttribute("DataRate", StringValue("200Mbps"));
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
        devtemp.Get(0)->SetDeviceName(std::to_string(from[node_index]) + std::string("--") + std::to_string(to[node_index]));
        devtemp.Get(1)->SetDeviceName(std::to_string(to[node_index]) + std::string("--") + std::to_string(from[node_index]));
        //p2p.EnablePcap(names.c_str(), devtemp.Get(0), true);
        devList.push_back(devtemp.Get(0));
        devList.push_back(devtemp.Get(1));
        if (devtemp.Get(1)->GetNode()->GetId() < 13)
        { //tracking data
            LOG(INFO) << "Node: " << devtemp.Get(1)->GetNode()->GetId()
                      << ", dev INFO: " << devtemp.Get(1)->GetDeviceName();
            tracking_List.push_back(devtemp.Get(1));
            devtemp.Get(1)->track_setter(true);
        }
    }
}

static void preSetup(void)
{
    Config::SetDefault("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue(true));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1448));
}

static void enableRoutingSystem(void)
{
    // Create router nodes, initialize routing database and set up the routing
    // tables in the nodes.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    ns3::PacketMetadata::Enable();
}

static void afterSetup(void)
{
    // Create router nodes, initialize routing database and set up the routing
    // tables in the nodes.
    enableRoutingSystem();
}

NodeContainer buildTopo(void)
{
    preSetup();
    NodeContainer allnodes;
    allnodes.Create(12 + 1 + 12);
    //set up ip stacks
    InternetStackHelper internet;
    internet.Install(allnodes);
    //set up nodes
    buildRouter(allnodes);
    buildHost(allnodes);

    checkIp();
    afterSetup();

    return allnodes;
}

void trackPackets(float time_space)
{
    uint32_t num = devList.size();
    std::cout << "Sending Track: " << Simulator::Now().GetSeconds() << std::endl;
    for (uint32_t index = 0; index < num; index++)
    {
        if (devList[index]->GetOutBytes() == 0)
            continue;
        std::cout << "  [" << devList[index]->GetNode()->GetId() << "]" << devList[index]->GetDeviceName() << " : " << devList[index]->GetOutBytes() * 8 / 1000.0 / 1000 / time_space << " Mbps" << std::endl;
        devList[index]->ClearOutBytes();
    }
    /*
  std::cout << "Receiving Track: " << Simulator::Now().GetSeconds() << std::endl;
  for (uint32_t index = 0; index < num; index++)
  {
    if (devList[index]->GetInBytes() == 0)
      continue;
    std::cout << "  [" << devList[index]->GetNode()->GetId() << "]" << devList[index]->GetDeviceName() << " : " << devList[index]->GetInBytes() * 8 / 1000.0 / 1000 / time_space << " Mbps" << std::endl;
    devList[index]->ClearInBytes();
  }
  */
    Simulator::Schedule(Seconds(time_space), &trackPackets, time_space);
}
uint64_t indexnum_0 = 0;
void trackTMs(float time_space)
{

    for (uint32_t index_size = 0; index_size < tracking_List.size(); index_size++)
    {
        //std::cout << tracking_List[index_size]->GetDeviceName() << std::endl;
        std::string path = "scratch/data/TM.";
        tracking_List[index_size]->showTM(path, indexnum_0);
        tracking_List[index_size]->cleanCounts();
    }
    indexnum_0++;

    Simulator::Schedule(Seconds(time_space), &trackTMs, time_space);
}
