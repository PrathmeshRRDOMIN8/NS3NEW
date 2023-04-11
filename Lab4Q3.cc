// Implement  multicasting  with  UDP  with  the  following  configurations  for  the  topology  given below:▪Multicast source is at node n0.▪Multicast forwarded by node n2 onto LAN1.▪Nodes n0, n1, n2, n3, n4 and n5 receive the multicast frame.▪Node n5 listens for the data

#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[]){
        
        Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue("Dix"));
        
        /*CommandLine cmd (__FILE__);
        cmd.Parse (argc, argv);*/
        
        NodeContainer c;
        c.Create(8);
        
        NodeContainer c0 = NodeContainer(c.Get(0), c.Get(1), c.Get(2));
        NodeContainer c1 = NodeContainer(c.Get(2), c.Get(3), c.Get(4));
        NodeContainer c2 = NodeContainer(c.Get(4), c.Get(5));
        NodeContainer c3 = NodeContainer(c.Get(5), c.Get(6), c.Get(7));
        
        PointToPointHelper p2p;
        p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
        p2p.SetChannelAttribute("Delay", StringValue("2ms"));
        
        CsmaHelper csma;
        csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
        csma.SetChannelAttribute("Delay", StringValue("2ms"));
        
        NetDeviceContainer nd0 = csma.Install(c0);
        NetDeviceContainer nd1 = csma.Install(c1);
        NetDeviceContainer nd2 = p2p.Install(c2);
        NetDeviceContainer nd3 = csma.Install(c3);
        
        InternetStackHelper internet;
        internet.Install(c);
        
        Ipv4AddressHelper ipv4Addr;
        ipv4Addr.SetBase("10.1.1.0", "255.255.255.0");
        ipv4Addr.Assign(nd0);
        ipv4Addr.SetBase("10.1.2.0", "255.255.255.0");
        ipv4Addr.Assign(nd1);
        ipv4Addr.SetBase("10.1.3.0", "255.255.255.0");
        ipv4Addr.Assign(nd2);
        ipv4Addr.SetBase("10.1.4.0", "255.255.255.0");
        ipv4Addr.Assign(nd3);
        
        Ipv4Address multicastSource("10.1.1.1");
        Ipv4Address multicastGroup("225.1.2.4");
        
        Ipv4StaticRoutingHelper multicast;
        Ptr<Node> multicastRouter = c.Get(2);
        Ptr<NetDevice> inputIf = nd0.Get(2);
        NetDeviceContainer outputDevices;
        outputDevices.Add(nd1.Get(0));
        multicast.AddMulticastRoute(multicastRouter, multicastSource, multicastGroup, inputIf, outputDevices);
        
        Ipv4StaticRoutingHelper multicast1;
        Ptr<Node> multicastRouter1 = c.Get(4);
        Ptr<NetDevice> inputIf1 = nd1.Get(2);
        NetDeviceContainer outputDevices1;
        outputDevices1.Add(nd2.Get(0));
        multicast1.AddMulticastRoute(multicastRouter1, multicastSource, multicastGroup, inputIf1, outputDevices1);
        
        Ptr<Node> sender = c.Get(0);
        Ptr<NetDevice> senderIf = nd0.Get(0);
        multicast.SetDefaultMulticastRoute(sender, senderIf);
        
        uint16_t multicastPort = 9;
        
        OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(multicastGroup, multicastPort)));
        onoff.SetConstantRate(DataRate("255b/s"));
        onoff.SetAttribute("PacketSize", UintegerValue(128));
        
        ApplicationContainer srcC = onoff.Install(c0.Get(0));
        srcC.Start(Seconds (1.0));
        srcC.Stop(Seconds (10.0));
        
        PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), multicastPort));
        
        ApplicationContainer sinkC = sink.Install (c0.Get(1));
        sinkC.Add(sink.Install(c0.Get(2)));
        sinkC.Add(sink.Install(c1.Get(1)));
        sinkC.Add(sink.Install(c1.Get(2)));
        sinkC.Add(sink.Install(c2.Get(1)));
        sinkC.Start(Seconds(1.0));
        sinkC.Stop(Seconds(10.0));
        
        /*AsciiTraceHelper ascii;
        csma.EnableAsciiAll (ascii.CreateFileStream ("11.tr"));
        csma.EnablePcapAll ("11", false);*/
        
        std::string animFile = "Lab4Q3.xml";
        AnimationInterface anim(animFile);
        
        Simulator::Run ();
        Simulator::Destroy ();

}

