// simulacion-wifi.cc  —  Compatible con ns-3.44
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/csma-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  // ======= Parámetros por línea de comandos =======
  double duration = 600.0;               // s
  std::string rateStr = "3Mbps";         // solo para UDP
  uint32_t packetSize = 1000;            // bytes (app)
  double distance = 5.0;                 // m STA-AP
  std::string dataMode = "HtMcs5";       // modo de datos Wi-Fi
  bool useTcp = true;                    // true=TCP BulkSend, false=UDP OnOff
  bool bursty = false;                   // UDP ráfagas ON/OFF

  CommandLine cmd;
  cmd.AddValue ("duration", "Simulation time (s)", duration);
  cmd.AddValue ("rate", "Application data rate (e.g., 3Mbps) [UDP only]", rateStr);
  cmd.AddValue ("packetSize", "Application packet size (bytes)", packetSize);
  cmd.AddValue ("distance", "Distance STA-AP (m)", distance);
  cmd.AddValue ("dataMode", "Wifi DataMode (e.g., HtMcs5)", dataMode);
  cmd.AddValue ("tcp", "Use TCP (1) or UDP (0)", useTcp);
  cmd.AddValue ("bursty", "Bursty traffic (On/Off) for UDP (1/0)", bursty);
  cmd.Parse (argc, argv);

  // ======= Nodos: STA (cliente), AP, y servidor por CSMA =======
  NodeContainer wifiStaNode; wifiStaNode.Create (1);
  NodeContainer wifiApNode;  wifiApNode.Create (1);
  NodeContainer csmaNodes;   // AP + servidor cableado
  csmaNodes.Add (wifiApNode.Get (0));
  csmaNodes.Create (1); // server

  // ======= Canal y PHY Wi-Fi (ns-3.44) =======
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy;                     // en ns-3.44 no existe ::Default()
  phy.SetChannel (channel.Create ());

  // ======= Estándar y control de tasa =======
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211n);   // 2.4 GHz/5 GHz según PHY por defecto
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue (dataMode),
                                "ControlMode", StringValue ("OfdmRate6Mbps"));

  // ======= MAC (STA y AP con mismo SSID) =======
  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns3-ssid");

  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
  NetDeviceContainer staDevice = wifi.Install (phy, mac, wifiStaNode);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));
  NetDeviceContainer apDevice = wifi.Install (phy, mac, wifiApNode);

  // ======= Movilidad: fija a distancia especificada =======
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiStaNode);
  mobility.Install (wifiApNode);

  Ptr<MobilityModel> staMob = wifiStaNode.Get (0)->GetObject<MobilityModel> ();
  Ptr<MobilityModel> apMob  = wifiApNode.Get (0)->GetObject<MobilityModel> ();
  staMob->SetPosition (Vector (0.0, 0.0, 1.0));
  apMob->SetPosition  (Vector (distance, 0.0, 1.0));

  // ======= Red cableada: AP ↔ servidor =======
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate ("100Mbps")));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
  NetDeviceContainer csmaDevices = csma.Install (csmaNodes); // (AP + Server)

  // ======= Pila IP =======
  InternetStackHelper stack;
  stack.Install (wifiStaNode);
  stack.Install (wifiApNode);
  stack.Install (csmaNodes.Get (1)); // servidor

  // Direcciones IP
  Ipv4AddressHelper address;

  // Red Wi-Fi (AP + STA)
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staIf = address.Assign (staDevice);
  Ipv4InterfaceContainer apIf  = address.Assign (apDevice);

  // Red CSMA (AP + Server)
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaIf = address.Assign (csmaDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // ======= Aplicaciones =======
  uint16_t port = 5001;
  ApplicationContainer clientApp, serverApp;

  if (useTcp)
  {
    // Servidor TCP
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory",
                                 InetSocketAddress (Ipv4Address::GetAny (), port));
    serverApp = sinkHelper.Install (csmaNodes.Get (1)); // server
    serverApp.Start (Seconds (0.0));
    serverApp.Stop  (Seconds (duration + 2.0));

    // Cliente TCP (flujo grande)
    BulkSendHelper bulk ("ns3::TcpSocketFactory",
                         InetSocketAddress (csmaIf.GetAddress (1), port));
    bulk.SetAttribute ("MaxBytes", UintegerValue (0));          // sin límite
    bulk.SetAttribute ("SendSize", UintegerValue (packetSize)); // tamaño de unidad app
    clientApp = bulk.Install (wifiStaNode.Get (0));
    clientApp.Start (Seconds (1.0));
    clientApp.Stop  (Seconds (duration));
  }
  else
  {
    // Servidor UDP
    PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory",
                                 InetSocketAddress (Ipv4Address::GetAny (), port));
    serverApp = sinkHelper.Install (csmaNodes.Get (1)); // server
    serverApp.Start (Seconds (0.0));
    serverApp.Stop  (Seconds (duration + 2.0));

    // Cliente UDP
    Address sink (InetSocketAddress (csmaIf.GetAddress (1), port));
    OnOffHelper onoff ("ns3::UdpSocketFactory", sink);
    onoff.SetAttribute ("PacketSize", UintegerValue (packetSize));
    onoff.SetAttribute ("DataRate", DataRateValue (DataRate (rateStr)));

    if (bursty)
    {
      onoff.SetAttribute ("OnTime",
        StringValue ("ns3::ExponentialRandomVariable[Mean=0.5]"));
      onoff.SetAttribute ("OffTime",
        StringValue ("ns3::ExponentialRandomVariable[Mean=0.5]"));
    }
    else
    {
      onoff.SetAttribute ("OnTime",
        StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
      onoff.SetAttribute ("OffTime",
        StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    }

    clientApp = onoff.Install (wifiStaNode.Get (0));
    clientApp.Start (Seconds (1.0));
    clientApp.Stop  (Seconds (duration));
  }

  // ======= FlowMonitor para métricas =======
  FlowMonitorHelper flowmonHelper;
  Ptr<FlowMonitor> flowmon = flowmonHelper.InstallAll ();


  Simulator::Stop (Seconds (duration + 2.0));
  Simulator::Run ();

  // ======= Resumen de resultados =======
  flowmon->CheckForLostPackets ();
  auto stats = flowmon->GetFlowStats ();

  double totalRxBits = 0.0;
  double sumDelayMs = 0.0;
  uint64_t rxPackets = 0;

  for (const auto &kv : stats)
  {
    const auto &st = kv.second;
    totalRxBits += (double) st.rxBytes * 8.0;
    rxPackets   += st.rxPackets;
    if (st.rxPackets > 0)
      sumDelayMs += 1000.0 * st.delaySum.GetSeconds ();
  }

  double simTime = duration; // s
  double throughputMbps = (totalRxBits / simTime) / 1e6;
  double avgDelayMs = (rxPackets > 0) ? (sumDelayMs / rxPackets) : 0.0;

  std::cout << "=== Resultados ns-3 ===\n";
  std::cout << "Throughput medio: " << throughputMbps << " Mbps\n";
  std::cout << "Retardo medio (one-way): " << avgDelayMs << " ms\n";
  std::cout << "Tamaño de paquete (app): " << packetSize << " bytes\n";
  std::cout << "Modo WiFi: " << dataMode << "  Distancia: " << distance << " m\n";
  std::cout << "Transporte: " << (useTcp ? "TCP" : "UDP")
            << (bursty ? " (bursty)\n" : "\n");

  Simulator::Destroy ();
  return 0;
}

