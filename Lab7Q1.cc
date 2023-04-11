//Create  a  wireless  network  with  10  nodes  and  establish  TCP  and  UDP  communication. Compare  the  performances  of  the  communication  for  varied  bandwidth  and  application  layer datarate.

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

Ptr<PacketSink> sink;
uint64_t lastTotalRx = 0;

void CalculateThroughput(){
        Time now = Simulator::Now();
        double cur = (sink->GetTotalRx()-lastTotalRx)*(double)8/1e5;
        std::cout<<now.GetSeconds()<<"s: \t"<<cur<<" Mbit/s"<<std::endl;
        lastTotalRx = sink->GetTotalRx();
        Simulator::Schedule(MilliSeconds(100), &CalculateThroughput);
}

int main(int argc, char *argv[]){
	uint32_t payloadSize = 1472;
	std::string dataRate = "100Mbps";
	std::string tcpVariant = "TcpNewReno";
	std::string phyRate = "HtMcs7";
	double simulationTime = 10;
	
	tcpVariant = std::string("ns3::")+tcpVariant;
	if(tcpVariant.compare("ns3::TcpWestwoodPlus")==0){
		Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpWestwood::GetTypeId()));
		Config::SetDefault("ns3::TcpWestwood::ProtocolType", EnumValue(TcpWestwood::WESTWOODPLUS));
	}
	else{
		TypeId tcpTid;
		NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(tcpVariant, &tcpTid), "TypeId "<<tcpVariant<<" not found");
		Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TypeId::LookupByName(tcpVariant)));
	}
	
	Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));
	
	WifiMacHelper wifiMac;
	WifiHelper wifiHelper;
	wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211_10MHZ);
	
	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5e9));
	
	YansWifiPhyHelper wifiPhy;
	wifiPhy.SetChannel(wifiChannel.Create());
	/*wifiPhy.Set("TxPowerStart", DoubleValue(10.0));
	wifiPhy.Set("TxPowerEnd", DoubleValue(10.0));
	wifiPhy.Set("TxPowerLevels", UintegerValue(1));
	iifiPhy.Set("TxGain", DoubleValue(0));
	wifiPhy.Set("RxGain", DoubleValue(0));
	wifiPhy.Set("RxNoiseFigure", DoubleValue (10));
	wifiPhy.Set("CcaMode1Threshold", DoubleValue(-79));
	wifiPhy.Set("EnergyDetectionThreshold", DoubleValue(-79+3));*/
	wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
	wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue(phyRate), "ControlMode", StringValue("HtMcs0"));
	
	NodeContainer networkNodes;
	networkNodes.Create(11);
	Ptr<Node> apWifiNode = networkNodes.Get(0);
	Ptr<Node> staWifiNode0 = networkNodes.Get(1);
	Ptr<Node> staWifiNode1 = networkNodes.Get(2);
	Ptr<Node> staWifiNode2 = networkNodes.Get(3);
	Ptr<Node> staWifiNode3 = networkNodes.Get(4);
	Ptr<Node> staWifiNode4 = networkNodes.Get(5);
	Ptr<Node> staWifiNode5 = networkNodes.Get(6);
	Ptr<Node> staWifiNode6 = networkNodes.Get(7);
	Ptr<Node> staWifiNode7 = networkNodes.Get(8);
	Ptr<Node> staWifiNode8 = networkNodes.Get(9);
	Ptr<Node> staWifiNode9 = networkNodes.Get(10);
	
	Ssid ssid = Ssid("network");
	wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
	
	NetDeviceContainer apDevice;
	apDevice = wifiHelper.Install(wifiPhy, wifiMac, apWifiNode);
	
	wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue (ssid));
	
	NetDeviceContainer staDevices0, staDevices1, staDevices2, staDevices3, staDevices4, staDevices5, staDevices6, staDevices7, staDevices8, staDevices9;
	staDevices0 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode0);
	staDevices1 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode1);
	staDevices2 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode2);
	staDevices3 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode3);
	staDevices4 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode4);
	staDevices5 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode5);
	staDevices6 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode6);
	staDevices7 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode7);
	staDevices8 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode8);
	staDevices9 = wifiHelper.Install(wifiPhy, wifiMac, staWifiNode9);
	
	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
	positionAlloc->Add(Vector(0.0, 0.0, 0.0));
	positionAlloc->Add(Vector(1.0, 10.0, 0.0));
	positionAlloc->Add(Vector(2.0, 9.0, 0.0));
	positionAlloc->Add(Vector(3.0, 8.0, 0.0));
	positionAlloc->Add(Vector(4.0, 7.0, 0.0));
	positionAlloc->Add(Vector(5.0, 6.0, 0.0));
	positionAlloc->Add(Vector(6.0, 5.0, 0.0));
	positionAlloc->Add(Vector(7.0, 4.0, 0.0));
	positionAlloc->Add(Vector(8.0, 3.0, 0.0));
	positionAlloc->Add(Vector(9.0, 2.0, 0.0));
	positionAlloc->Add(Vector(10.0, 1.0, 0.0));
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(apWifiNode);
	mobility.Install(staWifiNode0);
	mobility.Install(staWifiNode1);
	mobility.Install(staWifiNode2);
	mobility.Install(staWifiNode3);
	mobility.Install(staWifiNode4);
	mobility.Install(staWifiNode5);
	mobility.Install(staWifiNode6);
	mobility.Install(staWifiNode7);
	mobility.Install(staWifiNode8);
	mobility.Install(staWifiNode9);
	
	InternetStackHelper stack;
	stack.Install(networkNodes);
	
	Ipv4AddressHelper address;
	address.SetBase("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer apInterface;
	apInterface = address.Assign(apDevice);
	Ipv4InterfaceContainer staInterface0, staInterface1, staInterface2, staInterface3, staInterface4, staInterface5, staInterface6, staInterface7, staInterface8, staInterface9;
	staInterface0 = address.Assign(staDevices0);
	staInterface1 = address.Assign(staDevices1);
	staInterface2 = address.Assign(staDevices2);
	staInterface3 = address.Assign(staDevices3);
	staInterface4 = address.Assign(staDevices4);
	staInterface5 = address.Assign(staDevices5);
	staInterface6 = address.Assign(staDevices6);
	staInterface7 = address.Assign(staDevices7);
	staInterface8 = address.Assign(staDevices8);
	staInterface9 = address.Assign(staDevices9);
	
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 9));
	ApplicationContainer sinkApp = sinkHelper.Install(apWifiNode);
	sink = StaticCast<PacketSink>(sinkApp.Get(0));
	
	OnOffHelper server("ns3::TcpSocketFactory", (InetSocketAddress(apInterface.GetAddress(0), 9)));
	server.SetAttribute("PacketSize", UintegerValue(payloadSize));
	server.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	server.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	server.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
	ApplicationContainer serverApp = server.Install(staWifiNode5);
	
	sinkApp.Start(Seconds(0.0));
	serverApp.Start(Seconds(1.0));
	Simulator::Schedule(Seconds(1.1), &CalculateThroughput);
	
	std::string animFile = "1.xml";
        AnimationInterface anim(animFile);
	
	Simulator::Stop(Seconds(simulationTime+1));
	Simulator::Run();
	
	double averageThroughput = ((sink->GetTotalRx()*8)/(1e6*simulationTime));
	
	Simulator::Destroy ();
	
	std::cout<<"\nAverage throughput: "<<averageThroughput<<" Mbit/s"<< std::endl;
	
	return 0;
}

