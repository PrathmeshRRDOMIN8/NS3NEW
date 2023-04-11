//Configure a star topology with 12 nodes and visualize the same using NetAnim
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-star.h"
#include "ns3/netanim-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  /*CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);*/
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  PointToPointStarHelper star(12, pointToPoint);
  
  InternetStackHelper stack;
  star.InstallStack (stack);
  
  star.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0", "255.255.255.0"));
  
  Address hubLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), 50000));
  PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", hubLocalAddress);
  
  ApplicationContainer hubApp = packetSinkHelper.Install (star.GetHub ());
  hubApp.Start (Seconds (1.0));
  hubApp.Stop (Seconds (10.0));
  
  OnOffHelper onOffHelper("ns3::TcpSocketFactory", Address());
  onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  
  ApplicationContainer spokeApps;
  for(int i = 0; i<(int)star.SpokeCount(); i++){
        AddressValue remoteAddress(InetSocketAddress(star.GetHubIpv4Address(i), 50000));
        onOffHelper.SetAttribute("Remote", remoteAddress);
        spokeApps.Add(onOffHelper.Install(star.GetSpokeNode(i)));
  }
  
  spokeApps.Start(Seconds(1.0));
  spokeApps.Stop(Seconds(10.0));
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  std::string animFile = "2.xml";
  AnimationInterface anim(animFile);        

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

