//Create  a  bus  topology  which  has  5  nodes  and  sends  large  TCP  files.  Display  the  network statics  and  analyze  performances  for  different  bandwidth  (0.5,  1,  4,  10,  24,  and  100  Mbps) and delay (5, 10, 15, 20 and 25 ms). 

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main(int argc, char *argv[]){
        uint32_t nCsma = 5;
        uint32_t maxBytes = 1000;
        uint16_t port = 9;
        bool tracing = true;
        
        NodeContainer csmaNodes;
        csmaNodes.Create(nCsma);
        
        CsmaHelper csma;
        csma.SetChannelAttribute("DataRate", StringValue("500kbps"));
        csma.SetChannelAttribute("Delay", StringValue("5ms"));
        
        NetDeviceContainer csmaDevices;
        csmaDevices = csma.Install(csmaNodes);
        
        InternetStackHelper stack;
        stack.Install(csmaNodes);
        
        Ipv4AddressHelper address;
        address.SetBase("10.1.1.0", "255.255.255.0");
        
        Ipv4InterfaceContainer csmaInterfaces;
        csmaInterfaces = address.Assign(csmaDevices);
        
        Ipv4GlobalRoutingHelper::PopulateRoutingTables();
        
        BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(csmaInterfaces.GetAddress(4), port));
        source.SetAttribute("MaxBytes", UintegerValue(maxBytes));
        ApplicationContainer sourceApps = source.Install(csmaNodes.Get (0));
        sourceApps.Start(Seconds(0.0));
        sourceApps.Stop(Seconds(10.0));
        
        PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApps = sink.Install(csmaNodes.Get(4));
        sinkApps.Start(Seconds(0.0));
        sinkApps.Stop(Seconds (10.0));
        
        if (tracing){
        	AsciiTraceHelper ascii;
        	csma.EnableAsciiAll(ascii.CreateFileStream("Lab5Q1.tr"));
        }
        
        std::string animFile = "1.xml";
        AnimationInterface anim(animFile);
        
        Simulator::Run ();
        Simulator::Destroy ();
        
        Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(sinkApps.Get (0));
        std::cout<<"Total Bytes Received: "<<sink1->GetTotalRx()<< std::endl;
        return 0;
}

