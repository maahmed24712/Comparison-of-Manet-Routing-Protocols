# NS3 Project: Performance and Analysis of MANET networks under varying conditions

This NS3 project focuses on analyzing the performance metrics of a mobile ad hoc network (MANET) by varying the number of nodes and utilizing different routing protocols. The project allows users to evaluate key performance indicators such as packet delivery rate, throughput, and packet loss.

## Prerequisites

Before running the project, ensure that you have the following:

1) NS3 installed on your system
2) C++ compiler compatible with NS3
3) Required NS3 modules as shown in my header files inside my code
4) Use the NS3 version of NS-3.29, to avoid any unnecessary errors

## How to Run

1) Clone the repository to your local machine.
2) Compile and build the project using the appropriate NS3 commands for your system.
3) Execute the generated executable

## Usage
1) Upon running the program, you will be prompted to enter the number of nodes in the network.
2) Provide the desired number of nodes.
3) Next, enter the routing protocol to be used (aodv, dsdv, or olsr).
4) The simulation will start, and the program will output the results, including sent packets, received packets, lost packets, packet delivery rate, and average throughput.
5) The simulation will also generate an animation file named "pro4.xml" that can be visualized using NetAnim. The name of the xml file can be of any choice.

## Description

The NS3 project aims to study the performance characteristics of a mobile ad hoc network (MANET) by modifying the number of nodes and utilizing different routing protocols. By varying these parameters, the project enables users to evaluate and compare the network's performance metrics under different scenarios.

The project utilizes the NS3 simulation framework, a widely used platform for network research and analysis. It leverages the following NS3 modules: Core, Point-to-Point, Network, Applications, Mobility, Internet, YANS-Wifi, Flow Monitor, AODV, OLSR, DSDV, DSR, Traffic Control, and NetAnim.

Upon executing the program, the user is prompted to input the desired number of nodes in the network and select a routing protocol (AODV, DSDV, or OLSR). The simulation then creates a MANET with the specified number of nodes, assigns IP addresses, and deploys the chosen routing protocol. It also sets up a UDP server and initiates UDP 
