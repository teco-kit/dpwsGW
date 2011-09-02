using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Discovery;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using NodeDiscovery.Gateway;
using System.IO;
using System.Xml;
using NodeDiscovery.WSEventing;
using System.Xml.Serialization;
using System.Reflection;

namespace NodeDiscovery
{

    class Program
    {

        protected static List<edu.teco.DPWS.Device> knownDevices = new List<edu.teco.DPWS.Device>();

        protected static Dictionary<System.Xml.XmlQualifiedName, Type> deviceTypes = new Dictionary<System.Xml.XmlQualifiedName, Type>();

        /// <summary>
        /// Initial point of the program.
        /// </summary>
        /// <param name="args">Arguments.</param>
        public static void Main(string[] args)
        {

            Init();

            // Write the header.
            Console.WriteLine(
@"ABB  DECRC - SERVICE and usability LAB
Advanced Identification and Labeling for Service
DECRC 2011
KIT, TecO 2011
DPWS Discovery Project,
Contact N. L. Fantana (nicolaie.fantana@de.abb.com)

 * Please, start a gateway before select the options.
   Additionally when using manual discovery the node must be running 
");
            // Go always into the menu, until the user selects to exit.
            while (true)
            {
                int index = 3;
                // Check the option.
                Console.WriteLine("-----------------");
                Console.WriteLine("1. Wait for Announcement");
                Console.WriteLine("2. Discover services");

                Console.WriteLine("-----------------");
                Console.WriteLine("Discovered devices:");
                foreach (edu.teco.DPWS.Device device in knownDevices)
                {
                    Console.WriteLine("{0}. Type {1} {2}:{3} at {4}", index, device.GetType().Name, device.GetDeviceType().Namespace, device.GetDeviceType().Name, device.EndpointAddress);
                    index++;
                }
                Console.WriteLine("-----------------");
                Console.WriteLine("0. Exit");
                Console.WriteLine("-----------------");
                Console.WriteLine();

                string line = Console.ReadLine();

                if (line.Equals("1"))
                {
                    StartAnnouncementProcess();
                }
                else if (line.Equals("2"))
                {
                    StartDiscoveryProcess();
                }
                else if (line.Equals("0"))
                {
                    return;
                }
                else
                {
                    // Switch to Device menu
                    int selection = 0;
                    if (int.TryParse(line, out selection) && selection > 2 && selection <= knownDevices.Count+2)
                    {
                        bool done = false;
                        Console.WriteLine();
                        edu.teco.DPWS.Device device = knownDevices[selection - 3];
                        while (!done)
                        {

                            Console.WriteLine("-----------------");
                            Console.WriteLine("Current device: {0} {1}:{2} at {3}", device.GetType().Name, device.GetDeviceType().Namespace, device.GetDeviceType().Name, device.EndpointAddress);
                            Console.WriteLine("-----------------");
                            Console.WriteLine("Services:");
                            index = 1;
                            foreach (edu.teco.DPWS.Service service in device.GetServices())
                            {
                                Console.WriteLine("Service name: {0}", service.GetType().Name);
                                Console.WriteLine("Invokable methods:");
                                foreach (MethodInfo method in service.GetInvokableMethods())
                                {
                                    Object[] subAttributes = method.GetCustomAttributes(typeof(edu.teco.DPWS.DPWSInvokeMethod), true);
                                    if (subAttributes.Length < 1)
                                        continue;

                                    Console.WriteLine("{0}. {1}", index, ((edu.teco.DPWS.DPWSInvokeMethod)subAttributes[0]).Description);
                                    index++;
                                }
                                Console.WriteLine("Subscribable methods:");
                                foreach (MethodInfo method in service.GetSubscribableMethods())
                                {
                                    Object[] subAttributes = method.GetCustomAttributes(typeof(edu.teco.DPWS.DPWSSubscription), true);
                                    if (subAttributes.Length < 1)
                                        continue;

                                    Console.WriteLine("{0}. {1}", index, ((edu.teco.DPWS.DPWSSubscription)subAttributes[0]).Description);
                                    index++;
                                }

                                Console.WriteLine();
                            }
                            Console.WriteLine("-----------------");
                            Console.WriteLine("0. Return");

                            string deviceLine = Console.ReadLine();

                            if (deviceLine.Equals("0"))
                            {
                                done = true;
                            }
                            else
                            {
                                int serviceSelection = 0;
                                if (int.TryParse(deviceLine, out serviceSelection) && serviceSelection > 0 && serviceSelection < index)
                                {
                                    foreach (edu.teco.DPWS.Service service in device.GetServices())
                                    {
                                        foreach (MethodInfo method in service.GetInvokableMethods())
                                        {
                                            serviceSelection--;
                                            if (serviceSelection == 0)
                                            {
                                                service.InvokeMethod(method);
                                                break;
                                            }
                                        }
                                        if (serviceSelection == 0)
                                            break;
                                        foreach (MethodInfo method in service.GetSubscribableMethods())
                                        {
                                            serviceSelection--;
                                            if (serviceSelection == 0)
                                            {
                                                service.SubscribeMethod(method);
                                            }
                                        }
                                        if (serviceSelection == 0)
                                            break;

                                    }
                                }
                                else { Console.WriteLine("Invalid input"); }
                            }
                        }
                    }
                    else { Console.WriteLine("Invalid input"); }
                }
            }
        }

        private static void Init()
        {
            // Add SSimp Device to known devices
            deviceTypes.Add(new System.Xml.XmlQualifiedName("SSimpDeviceType", "http://www.teco.edu/SensorValues"), typeof(edu.teco.DPWS.SSimp.SSimpDevice));
            deviceTypes.Add(new System.Xml.XmlQualifiedName("AccelerationDeviceType", "http://www.teco.edu/AccelerationModel"), typeof(edu.teco.DPWS.MStr.GLink.MStrDevice));
            deviceTypes.Add(new System.Xml.XmlQualifiedName("VLinkDeviceType", "http://www.teco.edu/VLinkModel"), typeof(edu.teco.DPWS.MStr.VLink.MStrDevice));
            deviceTypes.Add(new System.Xml.XmlQualifiedName("WirelessSensorNode", "http://www.iis.fraunhofer.de/kom/abt/wsn/"), typeof(edu.teco.DPWS.SSimpFH.WirelessSensorNode));
        }

        /// <summary>
        /// Starts the anouncement process, waiting for the Hello and Goodbye messages.
        /// For more information, see http://schemas.xmlsoap.org/ws/2005/04/discovery/
        /// </summary>
        private static void StartAnnouncementProcess()
        {

            AnnouncementService announcementService = new AnnouncementService();
            // Add event handlers

            announcementService.OnlineAnnouncementReceived += new EventHandler<AnnouncementEventArgs>(OnOnlineAnnouncement);
            announcementService.OfflineAnnouncementReceived += new EventHandler<AnnouncementEventArgs>(OnOfflineAnnouncement);

            // Create the service host with a singleton
            using (ServiceHost announcementServiceHost = new ServiceHost(announcementService))
            {
                // Add the announcement endpoint
                UdpAnnouncementEndpoint udp = new UdpAnnouncementEndpoint(DiscoveryVersion.WSDiscoveryApril2005);
                announcementServiceHost.AddServiceEndpoint(udp);

                // Open the host async
                announcementServiceHost.Open();


                Console.Write("Waiting for an announcement (Enter-Exit) ...");
                Console.ReadLine();
            }
        }



        private static void OnOnlineAnnouncement(object sender, AnnouncementEventArgs e)
        {
            Console.WriteLine("Hello!");
            EndpointDiscoveryMetadata metadata = e.EndpointDiscoveryMetadata;

            // Check if service is SSimpDeviceType which is the Host service used 
            foreach (System.Xml.XmlQualifiedName type in metadata.ContractTypeNames)
            {
                foreach(System.Xml.XmlQualifiedName name in deviceTypes.Keys)
                {
                    if (type.Equals(name))
                    {
                        Console.WriteLine("New device found: {0}:{1} at {2}", name.Namespace, name.Name, metadata.ListenUris[0]);
                        edu.teco.DPWS.Device device = (edu.teco.DPWS.Device)Activator.CreateInstance(deviceTypes[name]);
                        device.EndpointAddress = new EndpointAddress(metadata.ListenUris[0]);
                        GetMetaData(device);
                        device.Init();
                        knownDevices.Add(device);
                        Console.WriteLine("Service can now be invoked");
                        break;
                    }
                }
                
            }
        }

        private static void OnOfflineAnnouncement(object sender, AnnouncementEventArgs e)
        {
            Console.WriteLine("Bye!");
            EndpointDiscoveryMetadata metadata = e.EndpointDiscoveryMetadata;
            if (metadata != null && metadata.ListenUris.Count > 0)
            {
                foreach(edu.teco.DPWS.Device device in knownDevices)
                {
                    if (device.EndpointAddress.Uri.Equals(metadata.ListenUris[0]))
                    {
                        knownDevices.Remove(device);
                        Console.WriteLine("Device removed {0}:{1} at {2}", device.GetDeviceType().Namespace,device.GetDeviceType().Name,metadata.ListenUris[0]);
                        break;
                    }
                }
                
            }
        }

        /// <summary>
        /// Search for the Nodes using the Probe and Resolve messages;
        /// Registers discovered devices and resolves service endpoints
        /// For more information, see http://schemas.xmlsoap.org/ws/2005/04/discovery/
        /// </summary>
        private static void StartDiscoveryProcess()
        {
            knownDevices.Clear();

            // Check if the application will run the FindServiceAddress again.
            bool runAgainFind = true;
            while (runAgainFind)
            {
                runAgainFind = false;

                // Find the addresses;
                EndpointAddress[] ret = FindServiceAddress();

                if (ret == null || ret.Length == 0)
                {
                    // No services was found. Check if the user wants to run again.
                    Console.Write("No known devices found... Repeat (Y-yes, N-no)?");
                    ConsoleKeyInfo ky = Console.ReadKey();
                    if (ky.Key == ConsoleKey.Y)
                        runAgainFind = true;
                }
            }
        }

        /// <summary>
        /// Try to find the physical address of a service using discovery functions.
        /// Uses the http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe message and
        /// http://schemas.xmlsoap.org/ws/2005/04/discovery/Resolve message.
        /// </summary>
        /// <returns>The endpoint of the service, null otherwise.</return>
        public static EndpointAddress[] FindServiceAddress()
        {
            Dictionary<Uri, EndpointAddress> ret = new Dictionary<Uri, EndpointAddress>();

            // Creating DiscoveryClient class.
            DiscoveryClient discoveryClient = new DiscoveryClient(new UdpDiscoveryEndpoint(DiscoveryVersion.WSDiscoveryApril2005));
            ;
            Console.Write("Finding for all services in the network (empty criteria)...");

            FindResponse findResponseX = discoveryClient.Find(new FindCriteria() { Duration = new TimeSpan(0, 0, 2) });
            Console.WriteLine("{0} found.\n", findResponseX.Endpoints.Count);

            // Search for each endpoint found to resolve the address given.
            if (findResponseX.Endpoints.Count > 0)
            {
                foreach (EndpointDiscoveryMetadata meta in findResponseX.Endpoints)
                {
                    if (ret.ContainsKey(meta.Address.Uri))
                        continue;

                    // Get all contracts of the endpoints.
                    foreach (System.Xml.XmlQualifiedName type in meta.ContractTypeNames)
                    {
                        // Check if its a valid contract.
                        foreach (System.Xml.XmlQualifiedName name in deviceTypes.Keys)
                        {
                            if (type.Equals(name))
                            {
                                ResolveResponse resolveResponse = discoveryClient.Resolve(new ResolveCriteria(meta.Address));

                                Console.WriteLine("New device found: {0}:{1} at {2}", name.Namespace, name.Name, resolveResponse.EndpointDiscoveryMetadata.ListenUris[0]);
                                edu.teco.DPWS.Device device = (edu.teco.DPWS.Device)Activator.CreateInstance(deviceTypes[name]);
                                device.EndpointAddress = new EndpointAddress(resolveResponse.EndpointDiscoveryMetadata.ListenUris[0]);
                                GetMetaData(device);
                                device.Init();
                                knownDevices.Add(device);
                                ret.Add(meta.Address.Uri, new EndpointAddress(resolveResponse.EndpointDiscoveryMetadata.ListenUris[0]));
                                Console.WriteLine("Service can now be invoked");
                                break;
                            }
                        }
                    }
                }
            }

            return ret.Values.ToArray();
        }

        /// <summary>
        /// Get all the information about the services in a current address.
        /// Send the http://schemas.xmlsoap.org/ws/2004/09/transfer/Get message and 
        /// receive the http://schemas.xmlsoap.org/ws/2004/09/transfer/GetResponse.
        /// </summary>
        /// <param name="device">The device containing the services.</param>
        public static void GetMetaData(edu.teco.DPWS.Device device)
        {


            // Custom binding with TextMessageEncoding and HttpTransport and the specific soap and wsa versions shown here:
            CustomBinding binding = new CustomBinding(
                    new TextMessageEncodingBindingElement(MessageVersion.Soap12WSAddressingAugust2004, Encoding.UTF8),
                    new HttpTransportBindingElement());

            // A custom mex client deriving from MetadataExchangeClient. The motivation behind writing the custom client was to get at the ClientVia behavior for the MetadataExchangeClient. 
            // Followed the instructions here for this.
            // Gave the physical address in the Via for the custom client and the logical address as the EP address to be put into the ws-addressing To header as:
            MetadataExchangeClient mex = new MetadataExchangeClient(binding);
            MetadataSet metadataSet = mex.GetMetadata(device.EndpointAddress);

            // Check for the metadata set size. 
            System.Collections.ObjectModel.Collection<MetadataSection> documentCollection = metadataSet.MetadataSections;
            if (documentCollection != null)
            {
                //Get the WSDL from each MetadataSection of the metadata set
                foreach (MetadataSection section in documentCollection)
                {
                    try
                    {
                        if (section.Metadata is System.Xml.XmlElement)
                        {
                            System.Xml.XmlElement element = (System.Xml.XmlElement)section.Metadata;
                            if (element.Name == "wsdp:Relationship")
                            {
                                Relationship relationship = (Relationship)Utils.FromXML(element.OuterXml, typeof(Relationship), string.Empty);

                                foreach (System.Xml.XmlElement innerElement in relationship.Any)
                                {
                                    if (innerElement.Name == "wsdp:Host")
                                    {
                                        HostServiceType host = (HostServiceType)Utils.FromXML(innerElement.OuterXml, typeof(HostServiceType),
                                            string.Empty);
                                    }
                                    else if (innerElement.Name == "wsdp:Hosted")
                                    {
                                        HostServiceType hosted = (HostServiceType)Utils.FromXML(innerElement.OuterXml, typeof(HostServiceType),
                                            "http://schemas.xmlsoap.org/ws/2006/02/devprof", new System.Xml.Serialization.XmlRootAttribute("Hosted"));

                                        if (hosted.EndpointReference != null && hosted.EndpointReference.Length > 0)
                                        {
                                            Console.WriteLine("Service ID: {0}", hosted.ServiceId);

                                            foreach (edu.teco.DPWS.Service service in device.GetServices())
                                            {
                                                if (hosted.ServiceId.Equals(service.GetServiceID()))
                                                {
                                                    service.HostedEndpoint = new EndpointAddress(hosted.EndpointReference[0].Address.Value);
                                                    Console.WriteLine("Found endpoint for {0}", service.GetServiceID());
                                                }
                                            }

                                        }
                                    }
                                }
                            }
                            else if (element.Name == "wsdp:ThisModel")
                            {
                                ThisModelType thisModel = (ThisModelType)Utils.FromXML(element.OuterXml, typeof(ThisModelType), string.Empty);
                            }
                            else if (element.Name == "wsdp:ThisDevice")
                            {
                                ThisDeviceType thisDevice = (ThisDeviceType)Utils.FromXML(element.OuterXml, typeof(ThisDeviceType), string.Empty);
                                Console.WriteLine(string.Format("FirmwareVersion: {0}", thisDevice.FirmwareVersion));
                                Console.WriteLine(string.Format("FriendlyName: {0}", thisDevice.FriendlyName));
                                Console.WriteLine(string.Format("SerialNumber: {0}", thisDevice.SerialNumber));
                            }
                        }
                        else if (section.Metadata is System.Web.Services.Description.ServiceDescription)
                        {
                            System.Web.Services.Description.ServiceDescription sd = (System.Web.Services.Description.ServiceDescription)section.Metadata;
                            Console.WriteLine(string.Format("ServiceDescription name: {0}", sd.Name));
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine("Error on getting MetaData.:{0}", ex.Message);
                    }
                }
            }

        }

        
    }
}
