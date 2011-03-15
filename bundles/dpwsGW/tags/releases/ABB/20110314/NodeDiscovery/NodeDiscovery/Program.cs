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

namespace NodeDiscovery
{
    /// <summary>
    /// This is a client application that interacts with the SSimpDevice services.
    /// This application uses specific ports to communicate between the Client and the Server. It is possible to check which ports are being used in App.Config.
    /// Make sure there is no Firewall blocking these Ports.
    /// The application uses the default IP-address to send the messages. If you have more than 1 network adapter, this application needs to be revised.
    /// 
    /// Known issues:
    ///  - The announcement functions (Hello/Goodbye) are not working. Possibly the problem is because the messages are coming with empty Scope;
    ///  - The Config method was updated manually, needs to be updated in the service side;
    /// </summary>
    class Program
    {
        private static bool AnnouncementFound = false;
        private static System.Diagnostics.Process UsbBridgeProcess = null;
        private static System.Diagnostics.Process SSimpDevice = null;
        private static EndpointAddress endpointAddress = null;
        private static EndpointAddress hostedEndPoint = null;
           
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
DECRC 2009
DPWS Discovery Project,
Contact N. L. Fantana (nicolaie.fantana@de.abb.com)

 * Please, start Gateway before select the options.
   Additionally a running dummynode is required when using manual discovery (Option D) 
");
            // Go always into the menu, until the user selects to exit.
            while (true)
            {
                // Check the option.
                ConsoleKeyInfo ki = new ConsoleKeyInfo();
                Console.Write(
@"Select one option: 
   A-Wait for Announcement;
   C-Configuration Sample;
   D-Discover the service;
   I-Invoke the service;
   S-Eventing Subscribe; 
   P-Performance Test; 
   E-Exit
Select: ");
                ki = Console.ReadKey();
                Console.WriteLine();


                switch (ki.Key)
                {
                    // Start the announcement procedure.
                    case ConsoleKey.A:
                        StartAnouncementProcess();
                        break;

                    // Call the Configuration Method.
                    case ConsoleKey.C:
                        if (hostedEndPoint != null)
                        {
                            ConfigureNodes(hostedEndPoint, false);
                        }
                        break;

                    // Call the GetValues method (with or without showing information on the console).
                    case ConsoleKey.D:
                        // Finds a valid Endpoint for a Node.
                        endpointAddress = StartDiscoveryProcess(false);
                        if (endpointAddress != null)
                        {
                            // Get Metadata and the endpoint of the service.
                            hostedEndPoint = GetMetaData(endpointAddress);

                            // Check if the invokation of the service will be called again.
                            
                        }
                        // Kill the current processes.
                        if (UsbBridgeProcess != null) UsbBridgeProcess.Kill();
                        if (SSimpDevice != null) SSimpDevice.Kill();

                        break;
                    case ConsoleKey.I:
                        if (hostedEndPoint != null)
                        {
                             //Invoke the service to get the values.
                             InvokeService(hostedEndPoint, false);
                        }
                        break;
                    // Make a performance test.
                    case ConsoleKey.P:
                       // endpointAddress = StartDiscoveryProcess(false);
                        if (hostedEndPoint != null)
                        {
                            // Get Metadata and the endpoint of the service.
                            //EndpointAddress hostedEndPoint = GetMetaData(endpointAddress);

                            bool runAgainPerformance = true;
                            while (runAgainPerformance)
                            {
                                runAgainPerformance = false;

                                string valueStr = string.Empty;
                                int quantity = 0;
                                int timeWait = -1;

                                // Check the quantity of samples to try the performance test.
                                while (!int.TryParse(valueStr, out quantity))
                                {
                                    Console.Write("Select the quantity of samples (C-Cancel): ");
                                    valueStr = Console.ReadLine();
                                    if (valueStr == "C" || valueStr == "c")
                                        break;
                                }
                                if (quantity <= 0)
                                    break;

                                // Get the time to wait between each sample invoke.
                                valueStr = string.Empty;
                                while (!int.TryParse(valueStr, out timeWait))
                                {
                                    Console.Write("Select the time to wait between samples in miliseconds (C-Cancel): ");
                                    valueStr = Console.ReadLine();
                                    if (valueStr == "C" || valueStr == "c")
                                        break;
                                }
                                if (timeWait < 0)
                                    break;

                                Console.Write("Reading Values. This may take some time...");
                                System.Diagnostics.Stopwatch stp1 = new System.Diagnostics.Stopwatch();
                                stp1.Start();
                                for (int i = 0; i < quantity; i++)
                                {
                                    // Invoke the service.
                                    InvokeService(hostedEndPoint, true);
                                    System.Threading.Thread.Sleep(timeWait);
                                }
                                stp1.Stop();
                                Console.WriteLine("done.");
                                Console.WriteLine("Time elapsed: {0}", stp1.ElapsedMilliseconds);

                                // Ask if the user wants to run again.
                                Console.Write("Run again (Y-yes, N-no)?");
                                ConsoleKeyInfo ky = Console.ReadKey();
                                if (ky.Key == ConsoleKey.Y)
                                    runAgainPerformance = true;
                                Console.WriteLine();
                            }
                        }
                        // Kill the current processes.
                        if (UsbBridgeProcess != null) UsbBridgeProcess.Kill();
                        if (SSimpDevice != null) SSimpDevice.Kill();

                        break;

                    // Initialize the Subcription event. 
                    case ConsoleKey.S:
                        Console.WriteLine("Starting subscription event...");

                      //  endpointAddress = StartDiscoveryProcess(false);
                        if (hostedEndPoint != null)
                        {
                            //Get Metadata of the address.
                          //  EndpointAddress hostedEndPoint = GetMetaData(endpointAddress);

                            InstanceContext ithis = new InstanceContext(new DiscoveryCallBack());
                            
                            
                          
                            // Create a client
                            using (SensorValuesClient client = new SensorValuesClient(ithis))
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
                                        foreach(AddressHeader h in callbackEndpoint.Headers)
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
                                (s.Filter.Any = new System.Xml.XmlNode[1])[0] = new System.Xml.XmlDocument().CreateTextNode("http://www.teco.edu/SensorValues/SensorValuesEventOut");

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
                                    hostedEndPoint = null;
                                    return;
                                }
                                catch(EndpointNotFoundException e)
                                {
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

                        break;

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

        /// <summary>
        /// Starts the anouncement process, waiting for the Hello and Goodbye messages.
        /// For more information, see http://schemas.xmlsoap.org/ws/2005/04/discovery/
        /// </summary>
        private static void StartAnouncementProcess()
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
              

                Console.WriteLine("Please start a dummynode.");
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
                if (type.Equals(new System.Xml.XmlQualifiedName("SSimpDeviceType", "http://www.teco.edu/SensorValues")))
                {
                    Console.WriteLine("New service found: !!");
                    AnnouncementFound = true;
                    
                    endpointAddress = new EndpointAddress(metadata.ListenUris[0]);
                    hostedEndPoint = GetMetaData(endpointAddress);
                    Console.WriteLine("Service can now be invoked");
                }
            }
        }

        private static void OnOfflineAnnouncement(object sender, AnnouncementEventArgs e)
        {
            Console.WriteLine("Bye!");
            EndpointDiscoveryMetadata metadata = e.EndpointDiscoveryMetadata;
            if (metadata!=null && metadata.ListenUris.Count>0 && metadata.ListenUris[0].Equals(endpointAddress.Uri))
            {
                Console.WriteLine("Service said goodbye!!");
                endpointAddress = null;
                hostedEndPoint = null;
            }
        }

        /// <summary>
        /// Search for the Nodes using the Probe and Resolve messages;
        /// For more information, see http://schemas.xmlsoap.org/ws/2005/04/discovery/
        /// </summary>
        /// <param name="contracts"></param>
        /// <param name="startProcesses">Start the initial processes necessary for the test.</param>
        /// <returns>A list of Endpoint addresses that have the SSimpleDevice service.</returns>
        private static EndpointAddress StartDiscoveryProcess(bool startProcesses)
        {
            if (startProcesses)
            {
                // Start USB Bridge and TimeServer Proces
                Console.WriteLine("Starting the cmd_usbbridge.exe process...");
                UsbBridgeProcess = Utils.StartNewProcess("Utils\\cmd_usbbridge.exe", string.Format("{0} {1}", 5555, 5556));
                Console.WriteLine("Starting the SSimpDevice.exe process...");
                SSimpDevice = Utils.StartNewProcess("Utils\\SSimpDevice.exe", " -i 127.0.0.1");
            }

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
                    Console.Write("No SSimpDeviceType services found... Do you wanna run again (Y-yes, N-no)?");
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
         ;
            Console.Write("Finding for all services in the network (empty criteria)...");

            FindResponse findResponseX = discoveryClient.Find(new FindCriteria() {Duration=new TimeSpan(0,0,2)});
            Console.WriteLine("{0} found.\n", findResponseX.Endpoints.Count);
            
            // Search for each endpoint found to resolve the address given.
            if (findResponseX.Endpoints.Count > 0)
            {
                foreach (EndpointDiscoveryMetadata meta in findResponseX.Endpoints)
                {
                    if (ret.ContainsKey(meta.Address.Uri))
                        continue;

                    // Get all contracts of the endpoints.
                    foreach(System.Xml.XmlQualifiedName type in meta.ContractTypeNames)
                    {
                        // Check if its a valid contract.
                        //Console.WriteLine("Checking {0}@{1}", type, meta.Address);
                        if(type.Equals(new System.Xml.XmlQualifiedName("SSimpDeviceType","http://www.teco.edu/SensorValues")))
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
        public static EndpointAddress GetMetaData(EndpointAddress endpointAddress)
        {
            EndpointAddress ret = null;

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
                                            ret = new EndpointAddress(hosted.EndpointReference[0].Address.Value);
                                            GetMetaData(ret);
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
            return ret;
        }

        /// <summary>
        /// Call the SensorValues service and show the results in the console window.
        /// </summary>
        /// <param name="endpointAddress">The address the service will be called.</param>
        public static SSimpSample InvokeService(EndpointAddress endpointAddress, bool silent)
        {
            // Create a client (get the settings in the app.config);
            using (SensorValuesClient client =
                new SensorValuesClient(new InstanceContext(new DiscoveryCallBack()), "SensorValues"))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = endpointAddress;
                client.Endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);

                ////Get the ClientBaseAddress, to change from Localhost to an specific IP Address;
                //CustomBinding binding = (CustomBinding)client.Endpoint.Binding;
                //Uri baseAddress = ((CompositeDuplexBindingElement)binding.Elements[0]).ClientBaseAddress;

                //if (baseAddress != null)
                //{
                //    // Get the Client Machine Name (NB. Use the IP address as can be problems using the name)
                //    string myClientMachineName = System.Net.Dns.GetHostName();
                //    System.Net.IPHostEntry myClientMachineAddressList = System.Net.Dns.GetHostEntry(myClientMachineName);
                //    string myClientMachineIP = myClientMachineName;
                //    foreach (System.Net.IPAddress ip in myClientMachineAddressList.AddressList)
                //    {
                //        // Make sure to get the IPv4 address;
                //        if (!(ip.IsIPv6LinkLocal || ip.IsIPv6Multicast || ip.IsIPv6SiteLocal || ip.IsIPv6Teredo))
                //        {
                //            myClientMachineIP = ip.ToString();
                //            break;
                //        }
                //    }
                //    ((CompositeDuplexBindingElement)binding.Elements[0]).ClientBaseAddress = new Uri(baseAddress.AbsoluteUri.Replace("localhost", myClientMachineIP));
                //}

                // Call the service.
                if (!silent)
                    Console.WriteLine("Invoking at {0}", endpointAddress);

                client.Open();
                SSimpSample response;

                do
                {
                    response = client.GetSensorValues();
                    
                    // Show the results in the console window.
                    if (!silent)
                    {
                        ShowValues(response);
                        Console.WriteLine("Read Again?");
                    }
                } while (!silent && (Console.ReadKey().Key == ConsoleKey.Y));
                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();

                // Return the Values;
                return response;
            }
        }

        /// <summary>
        /// Configures the nodes.
        /// </summary>
        /// <param name="endpointAddress">The endpoint address.</param>
        /// <param name="silent">if set to <c>true</c> [silent].</param>
        public static void ConfigureNodes(EndpointAddress endpointAddress, bool silent)
        {
            // Create a client (get the settings in the app.config);
            using (SensorValuesClient client =
                new SensorValuesClient(new InstanceContext(new DiscoveryCallBack()), "SensorValues"))
            {
                // Connect to the discovered service endpoint.
                client.Endpoint.Address = endpointAddress;

                // Call the service.
                if (!silent)
                    Console.WriteLine("Invoking Configuration at {0}", endpointAddress);

                SSimpControl control = new SSimpControl();
                
                
                if (!silent)
                {
                    Console.WriteLine("\nSet time (y/N)");
                    if (Console.ReadKey().Key == ConsoleKey.Y)
                    {
                        control.NewTime = DateTime.Now;
                        control.NewTimeSpecified = true;
                    };
                    Console.WriteLine("\nSet Sensors (y/N)");

                    if (Console.ReadKey().Key == ConsoleKey.Y)
                    {
                       control.SensorConfig = new SensorConfigurationType();
                       Console.WriteLine("\nAcclRate");
                       try
                       {
                           sbyte input = sbyte.Parse(Console.ReadLine());
                           if (input > 0)
                           {
                               control.SensorConfig.Acceleration = new SSimpRateConfig();
                               control.SensorConfig.Acceleration.rate = input;
                           }
                       }
                       catch (ArgumentException) { }
                    };
                }

                SSimpStatus response = client.Config(control);

                // Show the results in the console window.
                if (!silent)
                {
                    Console.WriteLine("Configuration set successful. Current Status:");
                    Console.WriteLine("(New) time: {0}",response.CurrentTime);
                    
                    Console.WriteLine("BatteryVoltage: {0}", response.BatteryVoltage);
                    Console.WriteLine("SensorConfigurarion: {0}", response.SensorConfig.ToString());
                    
                    
                    Console.WriteLine("SensorCapabilities: {0}",  response.AllSensorConfig.ToString());
                    Console.WriteLine("Uptime: {0}", response.UpTime);
                }
               
                // Closing the client gracefully closes the connection and cleans up resources.
                client.Close();
            }
        }

        /// <summary>
        /// Shows the values.
        /// </summary>
        /// <param name="response">The response.</param>
        public static void ShowValues(SSimpSample response)
        {
            Console.WriteLine("Results:");
            if (response.Accelleration != null)
            {
                Console.WriteLine("\tAcceleration X: {0}", response.Accelleration.x);
                Console.WriteLine("\tAcceleration Y: {0}", response.Accelleration.y);
                Console.WriteLine("\tAcceleration Z: {0}", response.Accelleration.z);
            }
         
            if (response.Audio != null)
                Console.WriteLine("\tAudio: {0}", response.Audio.volume);
            if (response.Force != null)
                Console.WriteLine("\tForce: {0}", response.Force.value);
            if (response.Light != null)
                Console.WriteLine("\tLight: {0}", response.Light.infrared);
            if (response.Temperature != null)
                Console.WriteLine("\tTemperature: {0}", response.Temperature.value);
        }
    }

    public class DiscoveryCallBack : EventSourceCallback, SensorValuesCallback
    {
        #region SensorValuesCallback Members

        public void SensorValuesEvent(SensorValuesEvent request)
        {
            Console.WriteLine(string.Format("Got response: {0}", request.Sample.TimeStamp));
            Program.ShowValues(request.Sample);
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
