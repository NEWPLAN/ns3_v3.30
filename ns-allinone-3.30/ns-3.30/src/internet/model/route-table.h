#ifndef __MARS_ROUTE_TABLE_H__
#define __MARS_ROUTE_TABLE_H__
namespace ns3
{
class RoutingTable
{
public:
    RoutingTable();
    ~RoutingTable();
    static void InitializeRoutingTable();
};
} // namespace ns3
#endif