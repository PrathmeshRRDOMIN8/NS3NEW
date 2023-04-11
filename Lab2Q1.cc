//Configure the topology given below and visualize the same using NetAnim
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
#include "ns3/csma-helper.h"
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

  NodeContainer nodes, csmaNodes;
  nodes.Create (4);
  csmaNodes.Add(nodes.Get(1));
  csmaNodes.Add(nodes.Get(2));
  csmaNodes.Create(3);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices0, devices1;
  devices0 = pointToPoint.Install (nodes.Get(0), nodes.Get(1));
  devices1 = pointToPoint.Install (nodes.Get(1), nodes.Get(2));
  
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("10Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install(csmaNodes);
  
  InternetStackHelper stack;
  stack.Install (nodes.Get(0));
  stack.Install (nodes.Get(3));
  stack.Install (csmaNodes);

  Ipv4AddressHelper address0, address1, address2, address3;
  address0.SetBase ("10.1.1.0", "255.255.255.0");
  address1.SetBase ("10.1.2.0", "255.255.255.0");
  address2.SetBase ("10.1.3.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces0 = address0.Assign (devices0);
  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (csmaDevices);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  UdpEchoServerHelper echoServer0 (9);

  ApplicationContainer serverApps0 = echoServer0.Install (csmaNodes.Get (4));
  serverApps0.Start (Seconds (1.0));
  serverApps0.Stop (Seconds (5.0));

  UdpEchoClientHelper echoClient0 (interfaces2.GetAddress (4), 9);
  echoClient0.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient0.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient0.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps0 = echoClient0.Install (nodes.Get (0));
  clientApps0.Start (Seconds (1.0));
  clientApps0.Stop (Seconds (5.0));
  
  std::string animFile = "Lab2Q1.xml";
  AnimationInterface anim(animFile);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

