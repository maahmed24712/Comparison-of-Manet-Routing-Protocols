#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/queue-disc.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/traffic-control-layer.h"
#include <iostream>
#include "ns3/tcp-socket-factory.h"
#include "ns3/tcp-socket.h"
#include "ns3/inet-socket-address.h"
#include "ns3/netanim-module.h"
using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE("MANET");
class manetNetwork{
public:
InternetStackHelper routingFunction(string route){
InternetStackHelper internet;
if (route == "aodv"){
AodvHelper aodv;
internet.SetRoutingHelper(aodv);
}
if (route == "olsr"){
OlsrHelper olsr;
internet.SetRoutingHelper(olsr);
}
if (route == "dsdv"){
DsdvHelper dsdv;
internet.SetRoutingHelper(dsdv);
}
return internet;
}
void createNetwork () {
double SentPackets = 0;
double ReceivedPackets = 0;
double AvgThroughput = 0;
double LostPackets = 0;
string routing;
double simulationTime = 500;
uint32_t processedFlows = 0;
uint32_t numNodes = 0;
uint64_t maxTrace = 50000;
Time::SetResolution(Time::NS);
//Step 1 - Creation of Nodes
cout << "Please enter the number of nodes: ";
cin >> numNodes;
if (numNodes<=1) {
// end the program
cout << "Ending simulation..." << endl;
exit(0);
}
NodeContainer nodes; //creating nodes
nodes.Create(numNodes);
// Create mobility models
MobilityHelper mobility;
mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
"X", StringValue("100.0"),
"Y", StringValue("100.0"),
"Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=100]"));
mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
mobility.Install(nodes);
// Get the positions of the nodes
for (uint32_t i = 0; i < nodes.GetN(); ++i) {
Ptr<Node> node = nodes.Get(i);
Vector pos = node->GetObject<MobilityModel>()->GetPosition();
std::cout << "Node " << node->GetId() << " position: " << pos.x << ", " << pos.y << std::endl;
}
// Create wireless network
WifiHelper wifi;
wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
"DataMode", StringValue("DsssRate1Mbps"),
"ControlMode", StringValue("DsssRate1Mbps"));
YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
YansWifiChannelHelper wChannel = YansWifiChannelHelper::Default();
wifiPhy.SetChannel (wChannel.Create ());
WifiMacHelper wifiMac;
wifiMac.SetType("ns3::AdhocWifiMac");
NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, nodes);
//Input which routing protocol
cout << "Enter the routing protocol (aodv,dsdv,olsr):";
cin >> routing;
InternetStackHelper internet;
if (routing == "aodv") {
AodvHelper aodv;
internet.SetRoutingHelper(aodv);
internet.Install(nodes);
cout << "The routing protocol is: aodv" << endl;
} else if (routing == "olsr") {
OlsrHelper olsr;
internet.SetRoutingHelper(olsr);
internet.Install(nodes);
cout << "The routing protocol is: olsr" << endl;
} else if (routing == "dsdv") {
DsdvHelper dsdv;
internet.SetRoutingHelper(dsdv);
internet.Install(nodes);
cout << "The routing protocol is: dsdv" << endl;
} else {
cout << "Invalid routing protocol!" << endl;
exit(1);
}
// Assign IP addresses
Ipv4AddressHelper ipAddrs;
ipAddrs.SetBase("192.168.1.0", "255.255.255.0");
Ipv4InterfaceContainer ifaces = ipAddrs.Assign(devices);
// Attach the FIFO queue to the device with a simple buffer
TrafficControlHelper tch;
tch.SetRootQueueDisc("ns3::FifoQueueDisc");
tch.Install(devices);
// Create applications
uint16_t port = 9;
UdpServerHelper server(port);
ApplicationContainer serverApp = server.Install(nodes);
serverApp.Start(Seconds(1.0));
serverApp.Stop(Seconds(simulationTime));
// Continuously sending packets from each node to a random node until simulation time is reached
uint64_t packetCount = 500;
double packetInterval = 0.001; // seconds
double delay = 0; // seconds
double delayInterval = 2.5; //seconds
for (uint64_t i = 0; i < nodes.GetN(); i++) {
Ptr<Node> sourceNode = nodes.Get(i);
uint64_t destIndex = rand() % nodes.GetN();
// Ensure that the node does not send a packet to itself
while (destIndex == i) {
destIndex = rand() % nodes.GetN();
}
Ptr<Node> destNode = nodes.Get(destIndex);
// Get the IP address of the destination node
Ipv4Address destAddr = destNode->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
UdpClientHelper client(destAddr, port);
client.SetAttribute("MaxPackets", UintegerValue(packetCount));
client.SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
client.SetAttribute("PacketSize", UintegerValue(32));
ApplicationContainer clientApp = client.Install(sourceNode);
clientApp.Start(Seconds(2.0 + delay)); // Start sending packets with a delay
delay+=delayInterval;
clientApp.Stop(Seconds(2.0 + delay)); // Stop sending packets after the delay
delay+=delayInterval;
}
//Install FlowMonitor onto the nodes
FlowMonitorHelper flow;
Ptr <FlowMonitor> monitor;
monitor = flow.InstallAll();
//Animation
AnimationInterface anim("pro4.xml");
anim.SetMaxPktsPerTraceFile(maxTrace);
// Run simulation
// Update simulation duration
Simulator::Stop(Seconds(simulationTime));
Simulator::Run();
Simulator::Destroy();
Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flow.GetClassifier ());
std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
{
Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);
if(processedFlows>numNodes){
break;
}
NS_LOG_UNCOND("----Flow ID:" <<iter->first);
NS_LOG_UNCOND("Source Address: " <<t.sourceAddress << " Destination Address: "<< t.destinationAddress);
NS_LOG_UNCOND("Sent Packets=" <<iter->second.txPackets);
NS_LOG_UNCOND("Received Packets =" <<iter->second.rxPackets);
NS_LOG_UNCOND("Lost Packets =" <<iter->second.txPackets-iter->second.rxPackets);
NS_LOG_UNCOND("Throughput =" <<iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024<<"Kbps");
SentPackets = SentPackets +(iter->second.txPackets);
ReceivedPackets = ReceivedPackets + (iter->second.rxPackets);
LostPackets = LostPackets + (iter->second.txPackets-iter->second.rxPackets);
AvgThroughput = AvgThroughput + (iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024);
processedFlows++;
}
AvgThroughput = AvgThroughput / processedFlows;
NS_LOG_UNCOND("--------Total Results of the simulation----------"<<std::endl);
NS_LOG_UNCOND("Total sent packets =" << SentPackets);
NS_LOG_UNCOND("Total Received Packets =" << ReceivedPackets);
NS_LOG_UNCOND("Total Lost Packets =" << LostPackets);
NS_LOG_UNCOND("Packet delivery rate = " << ((ReceivedPackets*100)/SentPackets) << "%");
NS_LOG_UNCOND("Average Throughput =" << AvgThroughput<< " Kbps");
}
};
int main(int argc, char* argv[]){
manetNetwork my;
my.createNetwork();
return 0;
}

