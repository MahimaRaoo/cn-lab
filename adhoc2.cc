/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 The Boeing Company
 *
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
 *
 */

// 
// This script configures two nodes on an 802.11b physical layer, with
// 802.11b NICs in adhoc mode, and by default, sends one packet of 1000 
// (application) bytes to the other node.  The physical layer is configured
// to receive at a fixed RSS (regardless of the distance and transmit
// power); therefore, changing position of the nodes has no effect. 
//
// There are a number of command-line options available to control
// the default behavior.  The list of available command-line options
// can be listed with the following command:
// ./waf --run "wifi-simple-adhoc --help"
//
// For instance, for this configuration, the physical layer will
// stop successfully receiving packets when rss drops below -97 dBm.
// To see this effect, try running:
//
// ./waf --run "wifi-simple-adhoc --rss=-97 --numPackets=20"
// ./waf --run "wifi-simple-adhoc --rss=-98 --numPackets=20"
// ./waf --run "wifi-simple-adhoc --rss=-99 --numPackets=20"
//
// Note that all ns-3 attributes (not just the ones exposed in the below
// script) can be changed at command line; see the documentation.
//
// This script can also be helpful to put the Wifi layer into verbose
// logging mode; this command will turn on all wifi logging:
// 
// ./waf --run "wifi-simple-adhoc --verbose=1"
//
// When you are done, you will notice two pcap trace files in your directory.
// If you have tcpdump installed, you can try this:
//
// tcpdump -r wifi-simple-adhoc-0-0.pcap -nn -tt
//

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"


#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;

int main (int argc, char *argv[])
{
  std::string phyMode ("DsssRate1Mbps");
  double rss = -80;  // -dBm
  
  CommandLine cmd;

  cmd.Parse (argc, argv);
  
  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", 
                      StringValue (phyMode));

  NodeContainer c;
  c.Create (5);

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  // This is one parameter that matters when using FixedRssLossModel
  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (0) ); 
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  // The below FixedRssLossModel will cause the rss to be fixed regardless
  // of the distance between the two stations, and the transmit power
  wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue (rss));
  wifiPhy.SetChannel (wifiChannel.Create ());

  // Add a mac and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);

  // Note that with FixedRssLossModel, the positions below are not 
  // used for received signal strength. 
  MobilityHelper mobility;
mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

  mobility.Install (c);

  InternetStackHelper internet;
  internet.Install (c);

  Ipv4AddressHelper ipv4;
  
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);
Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  uint16_t port = 7;
  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", localAddress);
  ApplicationContainer sinkApp = packetSinkHelper.Install (c.Get (4));

  sinkApp.Start (Seconds (0.0));
  sinkApp.Stop (Seconds (10 + 0.1));

  uint32_t payloadSize = 1448;
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  OnOffHelper onoff ("ns3::UdpSocketFactory", Ipv4Address::GetAny ());
  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  ApplicationContainer apps;

  AddressValue remoteAddress (InetSocketAddress (i.GetAddress (1), port));
  onoff.SetAttribute ("Remote", remoteAddress);
  apps.Add (onoff.Install (c.Get (0)));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10+0.1));


  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  Simulator::Stop (Seconds (10));
AnimationInterface anim ("ex4.xml");
  Simulator::Run ();
monitor->CheckForLostPackets();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
for(std::map<FlowId, FlowMonitor::FlowStats> ::const_iterator i=stats.begin();i!=stats.end();++i)
{
Ipv4FlowClassifier::FiveTuple t=classifier->FindFlow(i->first);
NS_LOG_UNCOND("Flow ID :"<<i->first<<t.sourceAddress<<t.destinationAddress); 
NS_LOG_UNCOND("RX :"<<i->second.rxPackets); 
NS_LOG_UNCOND("TX :"<<i->second.txPackets); 
NS_LOG_UNCOND("Throughput :"<<i->second.rxBytes*8.0/(i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())/1024<<" kbps"); 
}



  Simulator::Destroy ();

  return 0;
}

