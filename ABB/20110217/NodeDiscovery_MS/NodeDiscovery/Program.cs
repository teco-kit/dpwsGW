using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using System.Text;
using System.ServiceModel;
using System.ServiceModel.Discovery;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using NodeDiscovery.AccelerationModel;
using NodeDiscovery.DeviceInfoService;
using NodeDiscovery.DataLogging;
using System.IO;
using System.Xml;
using NodeDiscovery.WSEventing;
using System.Xml.Serialization;

namespace NodeDiscovery
{
    /// <summary>
    /// This is a client application that interacts with the AccelerationModel services modified from the original NodeDiscovery.
    /// This application uses specific ports to communicate between the Client and the Server. It is possible to check which ports are being used in App.Config.
    /// Make sure there is no Firewall blocking these Ports.
    /// The application uses the default IP-address to send the messages. If you have more than 1 network adapter, this application needs to be revised.
    /// 
    /// </summary>
    class Program
    {
        private static bool AnnouncementFound = false;
        private static EndpointAddress endpointAddress = null;
        private static EndpointAddress accelerationServiceEndPoint = null;
        private static EndpointAddress deviceInfoServiceEndPoint = null;
        private static EndpointAddress dataLoggingServiceEndPoint = null;

        /// <summary>
        /// Initial point of the program.
        /// </summary>
        /// <param name="args">Arguments.</param>
        public static void Main(string[] args)
        {
            // Check if the console application will continually run.
            bool continuousRun = false;
            if (args.Length > 0 && args[0].ToLower() == "-o")
            {
                continuousRun = (args.Length > 1 && args[1].ToLower() == "continous");
            }

            // Write the header.
            Console.WriteLine(
@"ABB  DECRC - SERVICE and usability LAB
Advanced Identification and Labeling for Service
DECRC 2011
DPWS Discovery Project,
Contact N. L. Fantana (nicolaie.fantana@de.abb.com)

 * Please start the MicroStrain gateway before selecting the options.
   The gateway requires the MicroStrain devices to be started and idle (steady blinking light).
   Additionally a running MicroStrain G-Link is required when using manual discovery (Option D) 
");
            // Go always into the menu, until the user selects to exit.
            while (true)
            {
                // Check the option.
                ConsoleKeyInfo ki = new ConsoleKeyInfo();
                Console.Write(
@"Select one option:
------------------------------
   Service Discovery:
------------------------------ 
   1-Wait for Announcement;
   2-Discover the service;
------------------------------
   Device Options:
------------------------------
   3-Get the device status;
   4-Stop the device;
------------------------------   
   Low Duty Cycle Streaming:
------------------------------ 
   5-Set the device to start streaming and subscribe to stream;
------------------------------
   Data Logging:
------------------------------
   6-Set the device to start logging;
   7-Set the device to send saved events and subscribe to stream;
   8-Get the number of saved sessions;
   9-Erase the data on the device;
------------------------------ 
   E-Exit
Select: ");
                ki = Console.ReadKey();
                Console.WriteLine();


                switch (ki.Key)
                {
                    // Start the announcement procedure.
                    case ConsoleKey.D1:
                        StartAnouncementProcess();
                        break;


                    // Call the GetValues method (with or without showing information on the console).
                    case ConsoleKey.D2:
                        // Finds a valid Endpoint for a Node.
                        endpointAddress = StartDiscoveryProcess();
                        if (endpointAddress != null)
                        {
                            // Get Metadata and the endpoint of the service.
                            GetServiceEndPoints(endpointAddress);

                            // Check if the invokation of the service will be called again.

                        }

                        break;
                    case ConsoleKey.D3:
                        {
                            if (deviceInfoServiceEndPoint != null)
                            {
                                invokeGetDeviceStatus(deviceInfoServiceEndPoint);
                            }
                            else
                            {
                                Console.WriteLine("Please discover a device first.");
                            }
                        } break;
                    case ConsoleKey.D4:
                        {
                            if (deviceInfoServiceEndPoint != null)
                            {
                                invokeStopDevice(deviceInfoServiceEndPoint);
                            }
                            else
                            {
                                Console.WriteLine("Please discover a device first.");
                            }
                        } break;
                    case ConsoleKey.D5:
                        {
                            if (accelerationServiceEndPoint != null)
                            {
                                invokeStartLDC(accelerationServiceEndPoint);
                                subscribeToLDC(accelerationServiceEndPoint);
                            }
                            else
                            {
                                Console.WriteLine("Please discover a device first.");
                            }
                        } break;

                    case ConsoleKey.D6:
                        {
                            if (dataLoggingServiceEndPoint != null)
                            {
                                invokeStartLogging(dataLoggingServiceEndPoint);
                            }
                            else
                            {
                                Console.WriteLine("Please discover a device first.");
                            }
                        } break;
                    case ConsoleKey.D7:
                        {
                            if (dataLoggingServiceEndPoint != null)
                            {
                                invokeStartDownload(dataLoggingServiceEndPoint);
                                subscribeToDownload(dataLoggingServiceEndPoint);
                            }
                            else
                            {
                                Console.WriteLine("Please discover a device first.");
                            }
                        } break;
                    case ConsoleKey.D8:
                        {
                            if (dataLoggingServiceEndPoint != null)
                            {
                                invokeGetSessions(dataLoggingServiceEndPoint);
                            }
                            else
                            {
                                Console.WriteLine("Please discover a device first.");
                            }
                        } break;
                    case ConsoleKey.D9:
                        {
                            if (dataLoggingServiceEndPoint != null)
                            {
                                invokeEraseDevice(dataLoggingServiceEndPoint);
                            }
                            else
                            {
                                Console.WriteLine("Please discover a device first.");
                            }
                        } break;

                    // Close the application.
                    case ConsoleKey.E:
                        return;

                    // Invalid key.
                    default:
                        Console.WriteLine("Invalid value.");
                        break;
                }
            }
        }

        private static void invokeEraseDevice(EndpointAddress hostedEndPoint)
        {
            using (DataLoggingServiceClient client =
               new DataLoggingServiceClient(new InstanceContext(new DiscoveryCallBack()), "DataLoggingService"))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = hostedEndPoint;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
                client.Open();

                client.Erase();

                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        private static void invokeGetSessions(EndpointAddress hostedEndPoint)
        {
            using (DataLoggingServiceClient client =
               new DataLoggingServiceClient(new InstanceContext(new DiscoveryCallBack()), "DataLoggingService"))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = hostedEndPoint;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
                client.Open();

                SessionInfo info = client.GetSessionCount();
                Console.WriteLine("Session Count: {0}", info.count);

                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        private static void subscribeToDownload(EndpointAddress hostedEndPoint)
        {
            Console.WriteLine("Starting subscription event...");

            //  endpointAddress = StartDiscoveryProcess(false);
            if (hostedEndPoint != null)
            {
                //Get Metadata of the address.
                //  EndpointAddress hostedEndPoint = GetMetaData(endpointAddress);

                InstanceContext ithis = new InstanceContext(new DiscoveryCallBack());

                using (DataLoggingServiceClient client = new DataLoggingServiceClient(ithis))
                {
                    client.Endpoint.Address = hostedEndPoint;
                    client.Open();

                    EndpointAddress callbackEndpoint = client.InnerDuplexChannel.LocalAddress;
                    EventSourceClient eventSource = new EventSourceClient(ithis, "WSEventing");
                    eventSource.Endpoint.Address = hostedEndPoint;
                    eventSource.Open();

                    Subscribe s = new Subscribe();
                    (s.Delivery = new DeliveryType()).Mode = "http://schemas.xmlsoap.org/ws/2004/08/eventing/DeliveryModes/Push";

                    XmlDocument doc = new XmlDocument();
                    using (XmlWriter writer = doc.CreateNavigator().AppendChild())
                    {
                        EndpointReferenceType notifyTo = new EndpointReferenceType();

                        (notifyTo.Address = new AttributedURI()).Value = callbackEndpoint.Uri.AbsoluteUri;

                        XmlRootAttribute notifyToElem = new XmlRootAttribute("NotifyTo");
                        notifyToElem.Namespace = "http://schemas.xmlsoap.org/ws/2004/08/eventing";

                        XmlDocument doc2 = new XmlDocument();
                        using (XmlWriter writer2 = doc2.CreateNavigator().AppendChild())
                        {
                            XmlRootAttribute ReferenceElement = new XmlRootAttribute("ReferenceElement");
                            foreach (AddressHeader h in callbackEndpoint.Headers)
                            {
                                h.WriteAddressHeader(writer2);
                            }

                            writer2.Close();
                            notifyTo.ReferenceParameters = new ReferenceParametersType();
                            notifyTo.ReferenceParameters.Any = notifyTo.ReferenceParameters.Any = doc2.ChildNodes.Cast<XmlElement>().ToArray<XmlElement>();
                        }

                        new XmlSerializer(notifyTo.GetType(), notifyToElem).Serialize(writer, notifyTo);
                    }

                    (s.Delivery.Any = new XmlElement[1])[0] = doc.DocumentElement;
                    (s.Filter = new FilterType()).Dialect = "http://schemas.xmlsoap.org/ws/2006/02/devprof/Action";
                    (s.Filter.Any = new System.Xml.XmlNode[1])[0] = new System.Xml.XmlDocument().CreateTextNode("http://www.teco.edu/DataLoggingService/DataLoggingServiceEventOut");

                    SubscribeResponse subscription;
                    try
                    {
                        Console.WriteLine("Subscribing to the event...");
                        //Console.ReadLine();
                        subscription = eventSource.SubscribeOp(s);
                    }
                    catch (TimeoutException t)
                    {
                        Console.WriteLine("Error reply time out: {0}!!", t.Message);
                        return;
                    }

                    String subscriptionId = null;
                    foreach (XmlNode xel in subscription.SubscriptionManager.ReferenceParameters.Any)
                    {
                        if (xel.LocalName.Equals("Identifier") && xel.NamespaceURI.Equals("http://schemas.xmlsoap.org/ws/2004/08/eventing"))
                            subscriptionId = xel.InnerText;
                    }

                    Console.WriteLine("Got subscription: {0}", subscriptionId);

                    Console.WriteLine("Press <ENTER> to unsubscribe.");
                    Console.ReadLine();

                    Console.WriteLine("Unsubscribing {0}", subscriptionId);
                    SubscriptionManagerClient subscriptionManager = new SubscriptionManagerClient("WSEventingUnsubscribe");
                    subscriptionManager.Endpoint.Address = hostedEndPoint;
                    subscriptionManager.UnsubscribeOp(subscriptionId, new Unsubscribe());
                    client.Close();
                }
            }
        }

        private static void invokeStartDownload(EndpointAddress hostedEndPoint)
        {

            using (DataLoggingServiceClient client =
               new DataLoggingServiceClient(new InstanceContext(new DiscoveryCallBack()), "DataLoggingService"))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = hostedEndPoint;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
                client.Open();

                client.StartDownload();

                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        private static void invokeStartLogging(EndpointAddress hostedEndPoint)
        {

            LoggingConfig config = new LoggingConfig();
            config.rate = LoggingConfigRate.Item256;
            config.duration = "30";
            using (DataLoggingServiceClient client =
               new DataLoggingServiceClient(new InstanceContext(new DiscoveryCallBack()), "DataLoggingService"))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = hostedEndPoint;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
                client.Open();

                client.StartLogging(config);

                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        private static void invokeStopDevice(EndpointAddress hostedEndPoint)
        {
            using (DeviceInfoServiceClient client = new DeviceInfoServiceClient("DeviceInfoService", hostedEndPoint))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = hostedEndPoint;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
                client.Open();

                client.StopNode();

                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        private static void invokeGetDeviceStatus(EndpointAddress hostedEndPoint)
        {
          
            using (DeviceInfoServiceClient client = new DeviceInfoServiceClient("DeviceInfoService",hostedEndPoint))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = hostedEndPoint;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
                client.Open();

                StatusMessage msg = client.GetNodeStatus();
                Console.WriteLine("Device Status: Desc: {0} Ready: {1}",msg.description,msg.ready);

                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        private static void invokeStartLDC(EndpointAddress hostedEndPoint)
        {
            LDCInfo info = new LDCInfo();
            info.rate = LDCInfoRate.Item5;
            using (AccelerationServiceClient client =
               new AccelerationServiceClient(new InstanceContext(new DiscoveryCallBack()), "AccelerationService"))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = hostedEndPoint;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
                client.Open();

                client.StartLDC(info);

                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        private static void subscribeToLDC(EndpointAddress hostedEndPoint)
        {
            Console.WriteLine("Starting subscription event...");

            //  endpointAddress = StartDiscoveryProcess(false);
            if (hostedEndPoint != null)
            {
                //Get Metadata of the address.
                //  EndpointAddress hostedEndPoint = GetMetaData(endpointAddress);

                InstanceContext ithis = new InstanceContext(new DiscoveryCallBack());

                using (AccelerationServiceClient client = new AccelerationServiceClient(ithis))
                {
                    client.Endpoint.Address = hostedEndPoint;
                    client.Open();

                    EndpointAddress callbackEndpoint = client.InnerDuplexChannel.LocalAddress;
                    EventSourceClient eventSource = new EventSourceClient(ithis, "WSEventing");
                    eventSource.Endpoint.Address = hostedEndPoint;
                    eventSource.Open();

                    Subscribe s = new Subscribe();
                    (s.Delivery = new DeliveryType()).Mode = "http://schemas.xmlsoap.org/ws/2004/08/eventing/DeliveryModes/Push";

                    XmlDocument doc = new XmlDocument();
                    using (XmlWriter writer = doc.CreateNavigator().AppendChild())
                    {
                        EndpointReferenceType notifyTo = new EndpointReferenceType();

                        (notifyTo.Address = new AttributedURI()).Value = callbackEndpoint.Uri.AbsoluteUri;

                        XmlRootAttribute notifyToElem = new XmlRootAttribute("NotifyTo");
                        notifyToElem.Namespace = "http://schemas.xmlsoap.org/ws/2004/08/eventing";

                        XmlDocument doc2 = new XmlDocument();
                        using (XmlWriter writer2 = doc2.CreateNavigator().AppendChild())
                        {
                            XmlRootAttribute ReferenceElement = new XmlRootAttribute("ReferenceElement");
                            foreach (AddressHeader h in callbackEndpoint.Headers)
                            {
                                h.WriteAddressHeader(writer2);
                            }

                            writer2.Close();
                            notifyTo.ReferenceParameters = new ReferenceParametersType();
                            notifyTo.ReferenceParameters.Any = notifyTo.ReferenceParameters.Any = doc2.ChildNodes.Cast<XmlElement>().ToArray<XmlElement>();
                        }

                        new XmlSerializer(notifyTo.GetType(), notifyToElem).Serialize(writer, notifyTo);
                    }

                    (s.Delivery.Any = new XmlElement[1])[0] = doc.DocumentElement;
                    (s.Filter = new FilterType()).Dialect = "http://schemas.xmlsoap.org/ws/2006/02/devprof/Action";
                    (s.Filter.Any = new System.Xml.XmlNode[1])[0] = new System.Xml.XmlDocument().CreateTextNode("http://www.teco.edu/AccelerationService/AccelerationServiceEventOut");

                    SubscribeResponse subscription;
                    try
                    {
                        Console.WriteLine("Subscribing to the event...");
                        //Console.ReadLine();
                        subscription = eventSource.SubscribeOp(s);
                    }
                    catch (TimeoutException t)
                    {
                        Console.WriteLine("Error reply time out: {0}!!", t.Message);
                        return;
                    }

                    String subscriptionId = null;
                    foreach (XmlNode xel in subscription.SubscriptionManager.ReferenceParameters.Any)
                    {
                        if (xel.LocalName.Equals("Identifier") && xel.NamespaceURI.Equals("http://schemas.xmlsoap.org/ws/2004/08/eventing"))
                            subscriptionId = xel.InnerText;
                    }

                    Console.WriteLine("Got subscription: {0}", subscriptionId);

                    Console.WriteLine("Press <ENTER> to unsubscribe.");
                    Console.ReadLine();

                    Console.WriteLine("Unsubscribing {0}", subscriptionId);
                    SubscriptionManagerClient subscriptionManager = new SubscriptionManagerClient("WSEventingUnsubscribe");
                    subscriptionManager.Endpoint.Address = hostedEndPoint;
                    subscriptionManager.UnsubscribeOp(subscriptionId, new Unsubscribe());
                    client.Close();
                }
            }
        }

        /// <summary>
        /// Starts the anouncement process, waiting for the Hello and Goodbye messages.
        /// For more information, see http://schemas.xmlsoap.org/ws/2005/04/discovery/
        /// </summary>
        private static void StartAnouncementProcess()
        {
            //MulticastListener _multicastListener = new MulticastListener();
            //_multicastListener.MessageArrived += new EventHandler<MessageEventArgs>(_multicastListener_MessageArrived);

            AnnouncementListener announcementService = new AnnouncementListener();
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

                Console.WriteLine("Please start a dummynode.");
                Console.Write("Waiting for an announcement (Enter-Exit) ...");
                Console.ReadLine();
            }
        }

        static void _multicastListener_MessageArrived(object sender, MessageEventArgs e)
        {
            Console.WriteLine("Message arrived by the multicast listener: ");
            Console.WriteLine(e.Message.ToString());
        }

        private static void OnOnlineAnnouncement(object sender, AnnouncementEventArgs e)
        {
            EndpointDiscoveryMetadata metadata = e.EndpointDiscoveryMetadata;

            // Check if service is SSimpDeviceType which is the Host service used 
            foreach (System.Xml.XmlQualifiedName type in metadata.ContractTypeNames)
            {
                if (type.Equals(new System.Xml.XmlQualifiedName("AccelerationDeviceType", "http://www.teco.edu/AccelerationService")))
                {
                    Console.WriteLine("New service found: !!");
                    AnnouncementFound = true;

                    endpointAddress = new EndpointAddress(metadata.ListenUris[0]);
                    GetServiceEndPoints(endpointAddress);
                    Console.WriteLine("Service can now be invoked");
                }
            }
        }

        private static void OnOfflineAnnouncement(object sender, AnnouncementEventArgs e)
        {
            EndpointDiscoveryMetadata metadata = e.EndpointDiscoveryMetadata;
            if (metadata.ListenUris.Count > 0 && metadata.ListenUris[0].Equals(endpointAddress.Uri))
            {
                Console.WriteLine("Service said goodbye!!");
                endpointAddress = null;
                accelerationServiceEndPoint = null;
                deviceInfoServiceEndPoint = null;
                dataLoggingServiceEndPoint = null;
            }
        }

        /// <summary>
        /// Search for the Nodes using the Probe and Resolve messages;
        /// For more information, see http://schemas.xmlsoap.org/ws/2005/04/discovery/
        /// </summary>
        /// <param name="contracts"></param>
        /// <param name="startProcesses">Start the initial processes necessary for the test.</param>
        /// <returns>A list of Endpoint addresses that have the SSimpleDevice service.</returns>
        private static EndpointAddress StartDiscoveryProcess()
        {

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
                    Console.Write("No AccelerationDeviceType services found... Do you want to run again (Y-yes, N-no)?");
                    ConsoleKeyInfo ky = Console.ReadKey();
                    if (ky.Key == ConsoleKey.Y)
                        runAgainFind = true;
                }
                else if (ret != null && ret.Length > 1)
                {
                    // More than 1 endpoint was found. Select one for tests.
                    Console.WriteLine("Select endpoint:");
                    for (int i = 0; i < ret.Length; i++)
                        Console.WriteLine(string.Format("\t{0} - {1}", i, ret[i].Uri));

                    string valueStr = string.Empty;
                    int quantity = -1;
                    // Check the quantity of samples to try the performance test.
                    while (!int.TryParse(valueStr, out quantity) || quantity < 0 || quantity >= ret.Length)
                    {
                        Console.Write("Select (C-Cancel): ");
                        valueStr = Console.ReadLine();
                        if (valueStr == "C" || valueStr == "c")
                            return null;
                    }
                    return ret[quantity];
                }
                else if (ret != null && ret.Length == 1)
                    return ret[0];
            }
            return null;
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

            Console.Write("Finding for all services in the network (empty criteria)...");
            FindResponse findResponseX = discoveryClient.Find(new FindCriteria());
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
                        //Console.WriteLine("Checking {0}@{1}", type, meta.Address);
                        if (type.Equals(new System.Xml.XmlQualifiedName("AccelerationDeviceType", "http://www.teco.edu/AccelerationModel")))
                        {
                            // Call the resolve function.
                            //Console.WriteLine("Found {0}@{1}",type,meta.Address);
                            ResolveResponse resolveResponse = discoveryClient.Resolve(new ResolveCriteria(meta.Address));
                            //Console.WriteLine("Resolved {0} to {1}", resolveResponse.EndpointDiscoveryMetadata.Address, resolveResponse.EndpointDiscoveryMetadata.ListenUris[0]);

                            ret.Add(meta.Address.Uri,
                                new EndpointAddress(resolveResponse.EndpointDiscoveryMetadata.ListenUris[0]));
                        }
                    }
                }
            }

            return ret.Values.ToArray();
        }

        /// <summary>
        /// Get all the information about the service in a current address.
        /// Send the http://schemas.xmlsoap.org/ws/2004/09/transfer/Get message and 
        /// receive the http://schemas.xmlsoap.org/ws/2004/09/transfer/GetResponse.
        /// </summary>
        /// <param name="endpointAddress">The address the service will be called.</param>
        public static void GetServiceEndPoints(EndpointAddress endpointAddress)
        {

            // Custom binding with TextMessageEncoding and HttpTransport and the specific soap and wsa versions shown here:
            CustomBinding binding = new CustomBinding(
                    new TextMessageEncodingBindingElement(MessageVersion.Soap12WSAddressingAugust2004, Encoding.UTF8),
                    new HttpTransportBindingElement());

            // A custom mex client deriving from MetadataExchangeClient. The motivation behind writing the custom client was to get at the ClientVia behavior for the MetadataExchangeClient. 
            // Followed the instructions here for this.
            // Gave the physical address in the Via for the custom client and the logical address as the EP address to be put into the ws-addressing To header as:
            MetadataExchangeClient mex = new MetadataExchangeClient(binding);
            MetadataSet metadataSet = mex.GetMetadata(endpointAddress);

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

                                            if (hosted.ServiceId.Equals("AccelerationService"))
                                            {
                                                accelerationServiceEndPoint = new EndpointAddress(hosted.EndpointReference[0].Address.Value);
                                                Console.WriteLine("Found endpoint for AccelerationService");
                                            }
                                            else if (hosted.ServiceId.Equals("DeviceInfoService"))
                                            {
                                                deviceInfoServiceEndPoint = new EndpointAddress(hosted.EndpointReference[0].Address.Value);
                                                Console.WriteLine("Found endpoint for DeviceInfoService");
                                            }
                                            else if (hosted.ServiceId.Equals("DataLoggingService"))
                                            {
                                                dataLoggingServiceEndPoint = new EndpointAddress(hosted.EndpointReference[0].Address.Value);
                                                Console.WriteLine("Found endpoint for DataLoggingService");
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
                        Console.WriteLine("Error on getting MetaData.");
                    }
                }
            }
        }

        /// <summary>
        /// Shows the values.
        /// </summary>
        /// <param name="response">The response.</param>
        public static void ShowValues(AccelerationServiceEvent response)
        {
            Console.WriteLine("Results:");
            if (response.series != null)
            {
                Console.WriteLine("Series:");
                Console.WriteLine("\tFragment count: {0}", response.series.count);
                if (response.series.timestampSpecified)
                {
                    Console.WriteLine("\tTimestamp: {0}", response.series.timestamp);
                }
                Console.WriteLine("\tDelta: {0}", response.series.delta);
                for (int i = 0; i < response.series.sample.Length; i++)
                {
                    Console.WriteLine("Sample:");
                    Console.WriteLine("\tDelta: {0}", response.series.sample[i].delta);
                    Console.WriteLine("\tAcceleration X: {0}", response.series.sample[i].accl.x);
                    Console.WriteLine("\tAcceleration Y: {0}", response.series.sample[i].accl.y);
                    Console.WriteLine("\tAcceleration Z: {0}", response.series.sample[i].accl.z);
                }
            }
        }
    }

    public class DiscoveryCallBack : EventSourceCallback, AccelerationServiceCallback, DataLoggingServiceCallback
    {
        #region AccelerationServiceCallback Members

        public void AccelerationServiceEvent(AccelerationServiceEvent request)
        {
            Console.WriteLine("Incoming event");
            Console.WriteLine("Results:");
            if (request.series != null)
            {
                Console.WriteLine("Series:");
                Console.WriteLine("\tFragment count: {0}", request.series.count);
                if (request.series.timestampSpecified)
                {
                    Console.WriteLine("\tTimestamp: {0}", request.series.timestamp);
                }
                Console.WriteLine("\tDelta: {0}", request.series.delta);
                for (int i = 0; i < request.series.sample.Length; i++)
                {
                    Console.WriteLine("Sample:");
                    Console.WriteLine("\tDelta: {0}", request.series.sample[i].delta);
                    Console.WriteLine("\tAcceleration X: {0}", request.series.sample[i].accl.x);
                    Console.WriteLine("\tAcceleration Y: {0}", request.series.sample[i].accl.y);
                    Console.WriteLine("\tAcceleration Z: {0}", request.series.sample[i].accl.z);
                }
            }
        }

        #endregion

        #region DataLoggingServiceCallback Members

        public void DataLoggingServiceEvent(DataLoggingServiceEvent request)
        {
            Console.WriteLine("Incoming event");
            Console.WriteLine("Results:");
            if (request.series != null)
            {
                Console.WriteLine("Series:");
                Console.WriteLine("\tFragment count: {0}", request.series.count);
                if (request.series.timestampSpecified)
                {
                    Console.WriteLine("\tTimestamp: {0}", request.series.timestamp);
                }
                Console.WriteLine("\tDelta: {0}", request.series.delta);
                for (int i = 0; i < request.series.sample.Length; i++)
                {
                    Console.WriteLine("Sample:");
                    Console.WriteLine("\tDelta: {0}", request.series.sample[i].delta);
                    Console.WriteLine("\tAcceleration X: {0}", request.series.sample[i].accl.x);
                    Console.WriteLine("\tAcceleration Y: {0}", request.series.sample[i].accl.y);
                    Console.WriteLine("\tAcceleration Z: {0}", request.series.sample[i].accl.z);
                }
            }
        }

        #endregion

        #region EventSourceCallback Members

        public void SubscriptionEnd(SubscriptionEnd1 request)
        {
            Console.WriteLine("Event closed.");
        }

        #endregion
    }
}
