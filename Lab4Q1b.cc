//For the topology given below simulate broadcasting using UDP protocol.
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[]){
        
        /*CommandLine cmd (__FILE__);
        cmd.Parse (argc, argv);*/
        
        NodeContainer c;
        c.Create(5);
        
        NodeContainer c0 = NodeContainer(c.Get(0), c.Get(1));
        NodeContainer c1 = NodeContainer(c.Get(1), c.Get(2), c.Get(3), c.Get(4));
        
        PointToPointHelper p2p;
        p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
        p2p.SetChannelAttribute("Delay", StringValue("2ms"));
        
        CsmaHelper csma;
        csma.SetChannelAttribute("DataRate", StringValue("10Mbps"));
        csma.SetChannelAttribute("Delay", StringValue("2ms"));
        
        NetDeviceContainer nd0 = p2p.Install(c0);
        NetDeviceContainer nd1 = csma.Install(c1);
        
        InternetStackHelper internet;
        internet.Install(c);
        
        Ipv4AddressHelper ipv4Addr;
        ipv4Addr.SetBase("10.1.1.0", "255.255.255.0");
        ipv4Addr.Assign(nd0);
        ipv4Addr.SetBase("10.1.2.0", "255.255.255.0");
        ipv4Addr.Assign(nd1);
        
        uint16_t port = 9;
        
        OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address("255.255.255.255"), port)));
        onoff.SetConstantRate(DataRate("500kb/s"));
        
        ApplicationContainer app = onoff.Install(c1.Get(0));
        app.Start(Seconds(1.0));
        app.Stop(Seconds(10.0));
        
        PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
        app = sink.Install(c1.Get(1));
        app.Add(sink.Install(c1.Get(2)));
        app.Add(sink.Install(c1.Get(3)));
        app.Start(Seconds(1.0));
        app.Stop(Seconds(10.0));
        
        /*AsciiTraceHelper ascii;
        csma.EnableAsciiAll (ascii.CreateFileStream ("csma-broadcast.tr"));
        csma.EnablePcapAll ("csma-broadcast", false);*/
        
        std::string animFile = "Lab4Q1b.xml";
        AnimationInterface anim(animFile);
        
        Simulator::Run ();
        Simulator::Destroy ();
}

