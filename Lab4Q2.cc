//Simulate the broadcasting with TCP for a star topology with 8 nodes

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-star.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[]){
	
	/*CommandLine cmd (__FILE__);
	cmd.Parse (argc, argv);*/
	
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	p2p.SetChannelAttribute("Delay", StringValue("2ms"));
	
	PointToPointStarHelper star(12, p2p);
	
	InternetStackHelper stack;
	star.InstallStack(stack);
	
	star.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0", "255.255.255.0"));
	
	uint16_t port = 9;
        
        OnOffHelper onoff("ns3::TcpSocketFactory", Address(InetSocketAddress(Ipv4Address("255.255.255.255"), port)));
        onoff.SetConstantRate(DataRate("500kb/s"));
        
        ApplicationContainer app = onoff.Install(star.GetHub());
        app.Start(Seconds(1.0));
        app.Stop(Seconds(10.0));
        
        PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
        app = sink.Install(star.GetSpokeNode(0));
        for(int i = 1; i<(int)star.SpokeCount(); i++){
        	app.Add(sink.Install(star.GetSpokeNode(i)));
        }
        app.Start(Seconds(1.0));
        app.Stop(Seconds(10.0));
        
        std::string animFile = "Lab4Q2.xml";
        AnimationInterface anim(animFile);
        
        Simulator::Run ();
        Simulator::Destroy ();
}

