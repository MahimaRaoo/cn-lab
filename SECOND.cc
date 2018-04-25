#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TrafficControlExample");


int
main (int argc, char *argv[])
{
  double simulationTime = 5; //seconds
  //std::string transportProt = "Tcp";
  //std::string socketType;

  CommandLine cmd;
  //cmd.AddValue ("transportProt", "Transport protocol to use: Tcp, Udp", transportProt);
  cmd.Parse (argc, argv);

 /* if (transportProt.compare ("Tcp") == 0)
    {
      socketType = "ns3::TcpSocketFactory";
    }
  else
    {
      socketType = "ns3::UdpSocketFactory";
    }*/

  NodeContainer nodes;
  nodes.Create (4);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));
  //pointToPoint.SetQueue ("ns3::DropTailQueue", "Mode", StringValue ("QUEUE_MODE_PACKETS"), "MaxPackets", UintegerValue (1));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes.Get(0),nodes.Get(2));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);


devices = pointToPoint.Install (nodes.Get(2),nodes.Get(3));
 
  address.SetBase ("10.1.2.0", "255.255.255.0");
 interfaces = address.Assign (devices);

NetDeviceContainer devices1;
  devices1 = pointToPoint.Install (nodes.Get(1),nodes.Get(2));

  Ipv4AddressHelper address1;
  address1.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);

devices1 = pointToPoint.Install (nodes.Get(2),nodes.Get(3)); 
  address1.SetBase ("10.1.4.0", "255.255.255.0");
 interfaces1 = address1.Assign (devices1);

Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  //Flow
  uint16_t port = 7;
  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", localAddress);
  ApplicationContainer sinkApp = packetSinkHelper.Install (nodes.Get (3));

  sinkApp.Start (Seconds (0.0));
  sinkApp.Stop (Seconds (5.0));

  uint32_t payloadSize = 1448;
  //Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  OnOffHelper onoff ("ns3::UdpSocketFactory", Ipv4Address::GetAny ());
  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  ApplicationContainer apps;

  InetSocketAddress rmt (interfaces.GetAddress (1), port);
  rmt.SetTos (0xb8);
  AddressValue remoteAddress (rmt);
  onoff.SetAttribute ("Remote", remoteAddress);
  apps.Add (onoff.Install (nodes.Get (2)));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (5.0));


uint16_t port1 = 9;
  Address localAddress1 (InetSocketAddress (Ipv4Address::GetAny (), port1));
  PacketSinkHelper packetSinkHelper1 ("ns3::TcpSocketFactory", localAddress1);
  ApplicationContainer sinkApp1 = packetSinkHelper1.Install (nodes.Get (3));

  sinkApp1.Start (Seconds (0.0));
  sinkApp1.Stop (Seconds (5.0));

  //uint32_t payloadSize = 1448;
  //Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  OnOffHelper onoff1 ("ns3::TcpSocketFactory", Ipv4Address::GetAny ());
  onoff1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoff1.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  onoff1.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  ApplicationContainer apps1;

  InetSocketAddress rmt1 (interfaces1.GetAddress (0), port);
  rmt1.SetTos (0xb8);
  AddressValue remoteAddress1 (rmt1);
  onoff1.SetAttribute ("Remote", remoteAddress1);
  apps1.Add (onoff.Install (nodes.Get (2)));
  apps1.Start (Seconds (1.0));
  apps1.Stop (Seconds (5.0));


  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  Simulator::Stop (Seconds (simulationTime + 5));
  Simulator::Run ();

  monitor->CheckForLostPackets();
  
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
 
for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
      NS_LOG_UNCOND("Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress);
      NS_LOG_UNCOND("Tx Packets = " << iter->second.txPackets);
      NS_LOG_UNCOND("Rx Packets = " << iter->second.rxPackets);
      NS_LOG_UNCOND("lostPackets Packets = " << iter->second.lostPackets);
      NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024  << " Kbps");
    }


  Simulator::Destroy ();

  
  return 0;
}
