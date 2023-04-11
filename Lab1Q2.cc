//Create a 4 node topology with the following configurations:▪Between n0 and n2 : data rate is 10 Mbps, queue type is droptail, delay is 2ms.▪Between n1 and n2 : data rate is 8 Mbps, queue type is droptail, delay is 3ms.▪Between n2 and n3 : data rate is 10 Mbps, queue type is droptail, delay is 4ms, Maximum queue size as 3 packets.Show the results of the simulation on the console.Use command line arguments to vary the MaxPacket as 16, Interval as 5.0 and packet size as 1024.

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

NS_LOG_COMPONENT_DEFINE ("Lab1 Question2");

using namespace ns3;

int main()
{
	Time::SetResolution (Time::NS);
	LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
	Ptr<Node> n0 = CreateObject<Node>();
	Ptr<Node> n1 = CreateObject<Node>();
	Ptr<Node> n2 = CreateObject<Node>();
	Ptr<Node> n3 = CreateObject<Node>();

	NodeContainer net1(n0,n2);
	NodeContainer net2(n1,n2);
	NodeContainer net3(n2,n3);

	NodeContainer all(n0,n1,n2,n3);

	PointToPointHelper p2p_1;
	p2p_1.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
	p2p_1.SetChannelAttribute("Delay",StringValue("2ms"));
	p2p_1.SetQueue ("ns3::DropTailQueue","MaxSize", StringValue ("50p"));
	NetDeviceContainer dev1 = p2p_1.Install(net1);

	PointToPointHelper p2p_2;
	p2p_2.SetDeviceAttribute("DataRate",StringValue("8Mbps"));
	p2p_2.SetChannelAttribute("Delay",StringValue("3ms"));
	p2p_2.SetQueue ("ns3::DropTailQueue","MaxSize", StringValue ("50p"));
	NetDeviceContainer dev2 = p2p_2.Install(net2);

	PointToPointHelper p2p_3;
	p2p_3.SetDeviceAttribute("DataRate",StringValue("10Mbps"));
	p2p_3.SetChannelAttribute("Delay",StringValue("4ms"));
	p2p_3.SetQueue ("ns3::DropTailQueue","MaxSize", StringValue ("50p"));
	NetDeviceContainer dev3 = p2p_3.Install(net3);

	InternetStackHelper inet;
	inet.Install(all);

	Ipv4AddressHelper addr1;
	addr1.SetBase("192.168.1.0","255.255.255.0");
	Ipv4InterfaceContainer if1 = addr1.Assign(dev1);

	Ipv4AddressHelper addr2;
	addr2.SetBase("192.168.2.0","255.255.255.0");
	Ipv4InterfaceContainer if2 = addr2.Assign(dev2);

	Ipv4AddressHelper addr3;
	addr3.SetBase("192.168.3.0","255.255.255.0");
	Ipv4InterfaceContainer if3 = addr3.Assign(dev3);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

 	UdpEchoServerHelper echoServer (9);
	ApplicationContainer serverApps = echoServer.Install (n1);
	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (10.0));

	UdpEchoClientHelper echoClient (if2.GetAddress(0),9);
	echoClient.SetAttribute ("MaxPackets", UintegerValue (16));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (5.0)));
	echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

	ApplicationContainer clientApps = echoClient.Install (n3);
	clientApps.Start (Seconds (1.0));
	clientApps.Stop (Seconds (10.0));
	
	Simulator::Run();
}
