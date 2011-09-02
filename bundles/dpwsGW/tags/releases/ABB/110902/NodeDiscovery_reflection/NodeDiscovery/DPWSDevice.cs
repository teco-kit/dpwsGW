using System;
using System.Reflection;
using System.Collections.Generic;
using System.ServiceModel;
using NodeDiscovery.WSEventing;
using System.Xml;
using System.Xml.Serialization;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.Linq;
using System.Xml.Linq;

namespace edu.teco.DPWS
{
    /// <summary>
    /// A DPWS Device
    /// </summary>
    public abstract class Device
    {
        protected EndpointAddress _endpointAddress = null;

        protected List<Service> services = new List<Service>();

        protected bool _initialized = false;

        public bool Initialized
        {
            get { return _initialized; }
        }

        public EndpointAddress EndpointAddress
        {
            get { return _endpointAddress; }
            set { _endpointAddress = value; }
        }


        public abstract System.Xml.XmlQualifiedName GetDeviceType();

        public List<Service> GetServices()
        {
            return services;
        }

        public void Init()
        {
            if (Initialized)
                return;

            foreach (Service s in services)
            {
                s.Init();
            }

            _initialized = true;
        }


    }

    /// <summary>
    /// A DPWS Service
    /// </summary>
    public abstract class Service
    {
        protected EndpointAddress endpointAddress = null;

        protected List<MethodInfo> invokableMethods = null;

        protected List<MethodInfo> subscribableMethods = null;

        protected Type clientType = null;

        protected bool _initialized = false;

        protected MethodInfo clientOpenMethod = null;

        protected MethodInfo clientCloseMethod = null;

        protected PropertyInfo endpointField = null;

        protected PropertyInfo clientChannelField = null;

        public bool Initialized
        {
            get { return _initialized; }
        }

        public EndpointAddress HostedEndpoint
        {
            get { return endpointAddress; }
            set { endpointAddress = value; }
        }

        public abstract string GetServiceID();

        public abstract string GetConfigurationName();

        public void Init()
        {
            if (Initialized)
                return;

            invokableMethods = new List<MethodInfo>();
            Type type = this.GetType();
            foreach (MethodInfo method in type.GetMethods())
            {
                if (method.GetCustomAttributes(typeof(DPWSInvokeMethod), false).Length > 0)
                {
                    invokableMethods.Add(method);
                }
            }

            subscribableMethods = new List<MethodInfo>();

            foreach (MethodInfo method in type.GetMethods())
            {
                if (method.GetCustomAttributes(typeof(DPWSSubscription), false).Length > 0)
                {
                    subscribableMethods.Add(method);
                }
            }

            clientOpenMethod = clientType.GetMethod("Open");
            clientCloseMethod = clientType.GetMethod("Close");

            endpointField = clientType.GetProperty("Endpoint");

            clientChannelField = clientType.GetProperty("InnerDuplexChannel");

            _initialized = true;
        }

        /// <summary>
        /// Returns a list of executeable methods
        /// </summary>
        /// <returns>A typed list of methods</returns>
        public List<MethodInfo> GetInvokableMethods()
        {
            if (!Initialized)
                throw new InvalidOperationException("Service not initialized");

            return invokableMethods;
        }

        /// <summary>
        /// Get a list of subscription types supported by the service.
        /// Each subscription is described by the address in the method tag and the method executed after subscribing (e.g. a Start-method)
        /// </summary>
        /// <returns>A typed list of methods</returns>
        public List<MethodInfo> GetSubscribableMethods()
        {
            if (!Initialized)
                throw new InvalidOperationException("Service not initialized");
            return subscribableMethods;
        }

        /// <summary>
        /// Send the subscription message to the endpoint
        /// </summary>
        /// <param name="eventSource"></param>
        /// <param name="callbackEndpoint"></param>
        /// <param name="eventOut"></param>
        /// <returns></returns>
        /// 
        protected String SubscribeMsg(EventSourceClient eventSource, EndpointAddress callbackEndpoint, String eventOut)
        {
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
            (s.Filter.Any = new System.Xml.XmlNode[1])[0] = new System.Xml.XmlDocument().CreateTextNode(eventOut);

            SubscribeResponse subscription;

            Console.WriteLine("Subscribing to the event...");
            //Console.ReadLine();
            subscription = eventSource.SubscribeOp(s);


            String subscriptionId = null;
            foreach (XmlNode xel in subscription.SubscriptionManager.ReferenceParameters.Any)
            {
                if (xel.LocalName.Equals("Identifier") && xel.NamespaceURI.Equals("http://schemas.xmlsoap.org/ws/2004/08/eventing"))
                    subscriptionId = xel.InnerText;
            }
            return subscriptionId;
        }

        protected void UnsubscribeMsg(String subscriptionId)
        {
            SubscriptionManagerClient subscriptionManager = new SubscriptionManagerClient("WSEventingUnsubscribe");
            subscriptionManager.Endpoint.Address = endpointAddress;
            subscriptionManager.UnsubscribeOp(subscriptionId, new Unsubscribe());
        }

        /// <summary>
        /// Invoke a method obtained by calling GetInvokableMethods
        /// </summary>
        /// <param name="method">One of the methods supplied by GetInvokableMethods</param>
        public void InvokeMethod(MethodInfo method)
        {
            if (!Initialized)
                throw new InvalidOperationException("Service not initialized");

            /// Open connection
            Object client = null;
            try
            {
                client = Activator.CreateInstance(clientType, new InstanceContext(this), GetConfigurationName());
            } catch (Exception e)
            { 
                /// If client does not support subscription
                client = Activator.CreateInstance(clientType);
            }
            ServiceEndpoint endpoint = (ServiceEndpoint)endpointField.GetValue(client,null);
            endpoint.Address = endpointAddress;
            endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);
            //endpointField.SetValue(client, endpoint, null);
            clientOpenMethod.Invoke(client, null);

            /// Invoke 
            object[] args = new object[] {client};
            try
            {
                method.Invoke(this, args);
            } catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            /// Close connection
            clientCloseMethod.Invoke(client, null);
        }

        /// <summary>
        /// Subscribe to the service
        /// Messages are sent to service message listener until enter is pressed.
        /// </summary>
        /// <param name="method">A method from GetSubscribableMethods</param>
        public void SubscribeMethod(MethodInfo method)
        {
            if (!Initialized)
                throw new InvalidOperationException("Service not initialized");

            Object[] subAttributes = method.GetCustomAttributes(typeof(DPWSSubscription), true);
            if (subAttributes.Length < 1)
                throw new ArgumentException("Not a subscribable method");

            String eventOut = ((DPWSSubscription)subAttributes[0]).EventOut;

            /// Open connection
            InstanceContext instanceContext = new InstanceContext(this);
            Object client = Activator.CreateInstance(clientType, instanceContext, GetConfigurationName());
            ServiceEndpoint endpoint = (ServiceEndpoint)endpointField.GetValue(client,null);
            endpoint.Address = endpointAddress;
            endpoint.Binding.ReceiveTimeout = new TimeSpan(0, 0, 2);

            clientOpenMethod.Invoke(client, null);


            IDuplexContextChannel innerDuplexChannel = (IDuplexContextChannel)clientChannelField.GetValue(client,null);
            EndpointAddress callbackEndpoint = innerDuplexChannel.LocalAddress;
            EventSourceClient eventSource = new EventSourceClient(instanceContext, "WSEventing");

            eventSource.Endpoint.Address = endpointAddress;
            eventSource.Open();
            String subscriptionId = SubscribeMsg(eventSource, callbackEndpoint, eventOut);

            /// Invoke 

            Console.WriteLine("Got subscription: {0}", subscriptionId);

            object[] args = new object[] { client };
            try
            {
                method.Invoke(this, args);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            Console.WriteLine("Press <ENTER> to unsubscribe.");
            Console.ReadLine();

            Console.WriteLine("Unsubscribing {0}", subscriptionId);

            /// Close connection
            UnsubscribeMsg(subscriptionId);
            clientCloseMethod.Invoke(client, null);
        }
    }

    /// <summary>
    /// Denotes a method as an invokable DPWS service method
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public class DPWSInvokeMethod : System.Attribute
    {
        protected string _description;

        public string Description
        {
            get { return _description; }
        }

        public DPWSInvokeMethod(string description)
        {
            _description = description;
        }
    }

    /// <summary>
    /// Denotes a method as a DPWS service subscription
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public class DPWSSubscription : System.Attribute
    {
        protected string _eventOut;

        public string EventOut
        { get { return _eventOut; } }

        protected string _description;

        public string Description
        {
            get { return _description; }
        }

        public DPWSSubscription(string description, string eventOut)
        {
            _description = description;
            _eventOut = eventOut;
        }
    }

    namespace SSimp
    {

        /// <summary>
        /// Wrapper for the TecO SSimp device
        /// </summary>
        public class SSimpDevice : Device
        {
            protected System.Xml.XmlQualifiedName _name = new System.Xml.XmlQualifiedName("SSimpDeviceType", "http://www.teco.edu/SensorValues");

            public SSimpDevice()
            {
                services.Add(new SSimpService());
            }

            public override System.Xml.XmlQualifiedName GetDeviceType()
            {
                return _name;
            }
        }

        /// <summary>
        /// Wrapper for the TecO SSimp SensorValues service
        /// </summary>
        public class SSimpService : Service, EventSourceCallback, NodeDiscovery.Gateway.SensorValuesCallback
        {
            public SSimpService()
            {
                clientType = typeof(NodeDiscovery.Gateway.SensorValuesClient);
            }

            public override string GetServiceID()
            {
                return "SensorValues";
            }

            public override string GetConfigurationName()
            {
                return "SensorValues";
            }

            [DPWSInvokeMethod("Read sensor values")]
            public void ReadSensorValues(Object client)
            {
                NodeDiscovery.Gateway.SensorValuesClient ssclient = (NodeDiscovery.Gateway.SensorValuesClient)client;
                NodeDiscovery.Gateway.SSimpSample response;

                do
                {
                    response = ssclient.GetSensorValues();

                    // Show the results in the console window.

                    ShowValues(response);
                    Console.WriteLine("Read Again?");

                } while (Console.ReadLine().ToLower().Equals("y"));
            }

            [DPWSInvokeMethod("Configure the service")]
            public void Configure(Object client)
            {
                NodeDiscovery.Gateway.SensorValuesClient ssclient = (NodeDiscovery.Gateway.SensorValuesClient)client;

                NodeDiscovery.Gateway.SSimpControl control = new NodeDiscovery.Gateway.SSimpControl();



                Console.WriteLine("\nSet time (y/N)");
                if  (Console.ReadLine().ToLower().Equals("y"))
                {
                    control.NewTime = DateTime.Now;
                    control.NewTimeSpecified = true;
                };
                Console.WriteLine("\nSet Sensors (y/N)");

                if  (Console.ReadLine().ToLower().Equals("y"))
                {
                    control.SensorConfig = new NodeDiscovery.Gateway.SensorConfigurationType();
                    Console.WriteLine("\nAcclRate");
                    try
                    {
                        sbyte input = sbyte.Parse(Console.ReadLine());
                        if (input > 0)
                        {
                            control.SensorConfig.Acceleration = new NodeDiscovery.Gateway.SSimpRateConfig();
                            control.SensorConfig.Acceleration.rate = input;
                        }
                    }
                    catch (ArgumentException) { }
                };


                NodeDiscovery.Gateway.SSimpStatus response = ssclient.Config(control);

                // Show the results in the console window.

                Console.WriteLine("Configuration set successful. Current Status:");
                Console.WriteLine("(New) time: {0}", response.CurrentTime);

                Console.WriteLine("BatteryVoltage: {0}", response.BatteryVoltage);
                Console.WriteLine("SensorConfigurarion: {0}", response.SensorConfig.ToString());


                Console.WriteLine("SensorCapabilities: {0}", response.AllSensorConfig.ToString());
                Console.WriteLine("Uptime: {0}", response.UpTime);

            }

            [DPWSSubscription("Subscribe to the service", "http://www.teco.edu/SensorValues/SensorValuesEventOut")]
            public void Subscribe(Object client)
            {
            }

            /// <summary>
            /// Shows the values.
            /// </summary>
            /// <param name="response">The response.</param>
            public void ShowValues(NodeDiscovery.Gateway.SSimpSample response)
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

            #region SensorValuesCallback Members

            public void SensorValuesEvent(NodeDiscovery.Gateway.SensorValuesEvent request)
            {
                Console.WriteLine(string.Format("Got response: {0}", request.Sample.TimeStamp));
                ShowValues(request.Sample);
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
    namespace MStr
    {
        namespace GLink {
        /// <summary>
        /// Wrapper for the MicroStrain device
        /// </summary>
        public class MStrDevice : Device
        {
            protected System.Xml.XmlQualifiedName _name = new System.Xml.XmlQualifiedName("AccelerationDeviceType", "http://www.teco.edu/AccelerationModel");

            public MStrDevice()
            {
                services.Add(new AccelerationService());
                services.Add(new DataLoggingService());
                services.Add(new DeviceInfoService());
            }

            public override System.Xml.XmlQualifiedName GetDeviceType()
            {
                return _name;
            }
        }

        /// <summary>
        /// Wrapper for the MStr Acceleration service
        /// </summary>
        public class AccelerationService : Service, EventSourceCallback, NodeDiscovery.AccelerationModel.AccelerationServiceCallback
        {
            public AccelerationService()
            {
                clientType = typeof(NodeDiscovery.AccelerationModel.AccelerationServiceClient);
            }

            public override string GetServiceID()
            {
                return "AccelerationService";
            }

            public override string GetConfigurationName()
            {
                return "AccelerationService";
            }

            [DPWSSubscription("Subscribe to the service", "http://www.teco.edu/AccelerationService/AccelerationServiceEventOut")]
            public void Subscribe(Object client)
            {
                NodeDiscovery.AccelerationModel.AccelerationServiceClient asclient = (NodeDiscovery.AccelerationModel.AccelerationServiceClient)client;
                NodeDiscovery.AccelerationModel.LDCInfo info = new NodeDiscovery.AccelerationModel.LDCInfo();
                info.rate = NodeDiscovery.AccelerationModel.LDCInfoRate.Item5;
                info.duration = "30";
                asclient.StartLDC(info);
            }



            #region AccelerationServiceCallback Members

            public void AccelerationServiceEvent(NodeDiscovery.AccelerationModel.AccelerationServiceEvent request)
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

        /// <summary>
        /// Wrapper for the MStr DeviceInfoService service
        /// </summary>
        public class DeviceInfoService : Service, EventSourceCallback
        {
            public DeviceInfoService()
            {
                clientType = typeof(NodeDiscovery.DeviceInfoService.DeviceInfoServiceClient);
            }

            public override string GetServiceID()
            {
                return "DeviceInfoService";
            }

            public override string GetConfigurationName()
            {
                return "DeviceInfoService";
            }

            [DPWSInvokeMethod("Get the device status")]
            public void GetDeviceStatus(Object client)
            {
                NodeDiscovery.DeviceInfoService.DeviceInfoServiceClient dsclient = (NodeDiscovery.DeviceInfoService.DeviceInfoServiceClient)client;


                NodeDiscovery.DeviceInfoService.StatusMessage msg = dsclient.GetNodeStatus();
                Console.WriteLine("Device Status: Desc: {0} Ready: {1}", msg.description, msg.ready);
            }

            [DPWSInvokeMethod("Stop the device")]
            public void StopNode(Object client)
            {
                NodeDiscovery.DeviceInfoService.DeviceInfoServiceClient dsclient = (NodeDiscovery.DeviceInfoService.DeviceInfoServiceClient)client;
                dsclient.StopNode();

            }

            #region EventSourceCallback Members

            public void SubscriptionEnd(SubscriptionEnd1 request)
            {
                Console.WriteLine("Event closed.");
            }

            #endregion
        }

        /// <summary>
        /// Wrapper for the MStr DataLogging service
        /// </summary>
        public class DataLoggingService : Service, EventSourceCallback, NodeDiscovery.DataLogging.DataLoggingServiceCallback
        {
            public DataLoggingService()
            {
                clientType = typeof(NodeDiscovery.DataLogging.DataLoggingServiceClient);
            }

            public override string GetServiceID()
            {
                return "DataLoggingService";
            }

            public override string GetConfigurationName()
            {
                return "DataLoggingService";
            }

            [DPWSInvokeMethod("Start logging to the node")]
            public void StartLogging(Object client)
            {
                NodeDiscovery.DataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.DataLogging.DataLoggingServiceClient)client;
                NodeDiscovery.DataLogging.LoggingConfig config = new NodeDiscovery.DataLogging.LoggingConfig();
                config.rate = NodeDiscovery.DataLogging.LoggingConfigRate.Item256;
                config.duration = "30";
                dlclient.StartLogging(config);

            }

            [DPWSInvokeMethod("Get the number of sessions stored on the node")]
            public void GetSessions(Object client)
            {
                NodeDiscovery.DataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.DataLogging.DataLoggingServiceClient)client;
                NodeDiscovery.DataLogging.SessionInfo info = dlclient.GetSessionCount();
                Console.WriteLine("Session Count: {0}", info.count);

            }

            [DPWSInvokeMethod("Erase sessions stored on the node")]
            public void Erase(Object client)
            {
                NodeDiscovery.DataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.DataLogging.DataLoggingServiceClient)client;
                dlclient.Erase();

            }

            [DPWSSubscription("Subscribe to the service", "http://www.teco.edu/DataLoggingService/DataLoggingServiceEventOut")]
            public void Subscribe(Object client)
            {
                NodeDiscovery.DataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.DataLogging.DataLoggingServiceClient)client;
                dlclient.StartDownload();
            }



            #region DataLoggingServiceCallback Members

            public void DataLoggingServiceEvent(NodeDiscovery.DataLogging.DataLoggingServiceEvent request)
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
        namespace VLink
        {
            /// <summary>
            /// Wrapper for the MicroStrain device
            /// </summary>
            public class MStrDevice : Device
            {
                protected System.Xml.XmlQualifiedName _name = new System.Xml.XmlQualifiedName("VLinkDeviceType", "http://www.teco.edu/VLinkModel");

                public MStrDevice()
                {
                    services.Add(new StreamingService());
                    services.Add(new DataLoggingService());
                    services.Add(new DeviceInfoService());
                }

                public override System.Xml.XmlQualifiedName GetDeviceType()
                {
                    return _name;
                }
            }

            /// <summary>
            /// Wrapper for the MStr VLink Streaming service
            /// </summary>
            public class StreamingService : Service, EventSourceCallback, NodeDiscovery.VLinkStreaming.StreamingServiceCallback
            {
                public StreamingService()
                {
                    clientType = typeof(NodeDiscovery.VLinkStreaming.StreamingServiceClient);
                }

                public override string GetServiceID()
                {
                    return "StreamingService";
                }

                public override string GetConfigurationName()
                {
                    return "StreamingService";
                }

                [DPWSSubscription("Subscribe to the service", "http://www.teco.edu/VLink/StreamingService/StreamingServiceEventOut")]
                public void Subscribe(Object client)
                {
                    NodeDiscovery.VLinkStreaming.StreamingServiceClient asclient = (NodeDiscovery.VLinkStreaming.StreamingServiceClient)client;
                    NodeDiscovery.VLinkStreaming.LDCInfo info = new NodeDiscovery.VLinkStreaming.LDCInfo();
                    info.rate = NodeDiscovery.VLinkStreaming.LDCInfoRate.Item5;
                    info.duration = "30";
                    asclient.StartLDC(info);
                }



                #region StreamingServiceCallback Members

                public void StreamingServiceEvent(NodeDiscovery.VLinkStreaming.StreamingServiceEvent request)
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
                            if(request.series.sample[i].differential1Specified)
                                Console.WriteLine("\tDifferential 1: {0}", request.series.sample[i].differential1);
                            if (request.series.sample[i].differential2Specified)
                                Console.WriteLine("\tDifferential 2: {0}", request.series.sample[i].differential2);
                            if (request.series.sample[i].differential3Specified)
                                Console.WriteLine("\tDifferential 3: {0}", request.series.sample[i].differential3);
                            if (request.series.sample[i].differential4Specified)
                                Console.WriteLine("\tDifferential 4: {0}", request.series.sample[i].differential4);
                            if(request.series.sample[i].single1Specified)
                                Console.WriteLine("\tSingle 1: {0}", request.series.sample[i].single1);
                            if (request.series.sample[i].single2Specified)
                                Console.WriteLine("\tSingle 2: {0}", request.series.sample[i].single2);
                            if (request.series.sample[i].single3Specified)
                                Console.WriteLine("\tSingle 3: {0}", request.series.sample[i].single3);
                            if (request.series.sample[i].temperatureSpecified)
                                Console.WriteLine("\tTemperature: {0}", request.series.sample[i].temperature);
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

            /// <summary>
            /// Wrapper for the MStr DeviceInfoService service
            /// </summary>
            public class DeviceInfoService : Service, EventSourceCallback
            {
                public DeviceInfoService()
                {
                    clientType = typeof(NodeDiscovery.VLinkDeviceInfoService.DeviceInfoServiceClient);
                }

                public override string GetServiceID()
                {
                    return "DeviceInfoService";
                }

                public override string GetConfigurationName()
                {
                    return "DeviceInfoService1";
                }

                [DPWSInvokeMethod("Get the device status")]
                public void GetDeviceStatus(Object client)
                {
                    NodeDiscovery.VLinkDeviceInfoService.DeviceInfoServiceClient dsclient = (NodeDiscovery.VLinkDeviceInfoService.DeviceInfoServiceClient)client;


                    NodeDiscovery.VLinkDeviceInfoService.StatusMessage msg = dsclient.GetNodeStatus();
                    Console.WriteLine("Device Status: Desc: {0} Ready: {1}", msg.description, msg.ready);
                }

                [DPWSInvokeMethod("Stop the device")]
                public void StopNode(Object client)
                {
                    NodeDiscovery.VLinkDeviceInfoService.DeviceInfoServiceClient dsclient = (NodeDiscovery.VLinkDeviceInfoService.DeviceInfoServiceClient)client;
                    dsclient.StopNode();

                }

                #region EventSourceCallback Members

                public void SubscriptionEnd(SubscriptionEnd1 request)
                {
                    Console.WriteLine("Event closed.");
                }

                #endregion
            }

            /// <summary>
            /// Wrapper for the MStr DataLogging service
            /// </summary>
            public class DataLoggingService : Service, EventSourceCallback, NodeDiscovery.VLinkDataLogging.DataLoggingServiceCallback
            {
                public DataLoggingService()
                {
                    clientType = typeof(NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient);
                }

                public override string GetServiceID()
                {
                    return "DataLoggingService";
                }

                public override string GetConfigurationName()
                {
                    return "DataLoggingService1";
                }

                [DPWSInvokeMethod("Start logging to the node")]
                public void StartLogging(Object client)
                {
                    NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient)client;
                    NodeDiscovery.VLinkDataLogging.LoggingConfig config = new NodeDiscovery.VLinkDataLogging.LoggingConfig();
                    config.rate = NodeDiscovery.VLinkDataLogging.LoggingConfigRate.Item256;
                    config.duration = "30";
                    dlclient.StartLogging(config);

                }

                [DPWSInvokeMethod("Get the number of sessions stored on the node")]
                public void GetSessions(Object client)
                {
                    NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient)client;
                    NodeDiscovery.VLinkDataLogging.SessionInfo info = dlclient.GetSessionCount();
                    Console.WriteLine("Session Count: {0}", info.count);

                }

                [DPWSInvokeMethod("Erase sessions stored on the node")]
                public void Erase(Object client)
                {
                    NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient)client;
                    dlclient.Erase();

                }

                [DPWSSubscription("Subscribe to the service", "http://www.teco.edu/VLink/DataLoggingService/DataLoggingServiceEventOut")]
                public void Subscribe(Object client)
                {
                    NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient dlclient = (NodeDiscovery.VLinkDataLogging.DataLoggingServiceClient)client;
                    dlclient.StartDownload();
                }



                #region DataLoggingServiceCallback Members

                public void DataLoggingServiceEvent(NodeDiscovery.VLinkDataLogging.DataLoggingServiceEvent request)
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
                            if (request.series.sample[i].differential1Specified)
                                Console.WriteLine("\tDifferential 1: {0}", request.series.sample[i].differential1);
                            if (request.series.sample[i].differential2Specified)
                                Console.WriteLine("\tDifferential 2: {0}", request.series.sample[i].differential2);
                            if (request.series.sample[i].differential3Specified)
                                Console.WriteLine("\tDifferential 3: {0}", request.series.sample[i].differential3);
                            if (request.series.sample[i].differential4Specified)
                                Console.WriteLine("\tDifferential 4: {0}", request.series.sample[i].differential4);
                            if (request.series.sample[i].single1Specified)
                                Console.WriteLine("\tSingle 1: {0}", request.series.sample[i].single1);
                            if (request.series.sample[i].single2Specified)
                                Console.WriteLine("\tSingle 2: {0}", request.series.sample[i].single2);
                            if (request.series.sample[i].single3Specified)
                                Console.WriteLine("\tSingle 3: {0}", request.series.sample[i].single3);
                            if (request.series.sample[i].temperatureSpecified)
                                Console.WriteLine("\tTemperature: {0}", request.series.sample[i].temperature);
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
    }

    namespace SSimpFH
    {

        /// <summary>
        /// Wrapper for the FH WirelessSensorNode device
        /// </summary>
        public class WirelessSensorNode : Device
        {
            protected System.Xml.XmlQualifiedName _name = new System.Xml.XmlQualifiedName("WirelessSensorNode", "http://www.iis.fraunhofer.de/kom/abt/wsn/");

            public WirelessSensorNode()
            {
                services.Add(new SensorValuesService());
            }

            public override System.Xml.XmlQualifiedName GetDeviceType()
            {
                return _name;
            }
        }

        /// <summary>
        /// Wrapper for the FH WirelessSensorNode SensorValues service
        /// </summary>
        public class SensorValuesService : Service, EventSourceCallback
        {
            public SensorValuesService()
            {
                clientType = typeof(NodeDiscovery.SensorValuesFH.SensorValuesClient);
            }

            public override string GetServiceID()
            {
                return "http://localhost:8080/0-00000000-0000-0000000000123-00/SensorValues";
            }

            public override string GetConfigurationName()
            {
                return "SensorValuesBinding";
            }

            [DPWSInvokeMethod("Read sensor values")]
            public void ReadSensorValues(Object client)
            {
                NodeDiscovery.SensorValuesFH.SensorValuesClient ssclient = (NodeDiscovery.SensorValuesFH.SensorValuesClient)client;
                NodeDiscovery.SensorValuesFH.Series response;

                do
                {
                    response = ssclient.GetSensorValues();

                    // Show the results in the console window.

                    ShowValues(response);
                    Console.WriteLine("Read Again?");

                } while (Console.ReadLine().ToLower().Equals("y")); 
            }
            
            /// <summary>
            /// Shows the values.
            /// </summary>
            /// <param name="response">The response.</param>
            public void ShowValues(NodeDiscovery.SensorValuesFH.Series response)
            {
                Console.WriteLine("Results:");
                if (response.fragment != null)
                {
                    if (response.fragment.countSpecified)
                    {
                        Console.WriteLine("Fragment: Count {0} Delta {1}", response.fragment.count, response.fragment.delta_t);
                    }
                    else
                    {
                        Console.WriteLine("Fragment: Delta {0}", response.fragment.delta_t);
                    }
                }

                if(response.timeStampSpecified)
                {
                    Console.WriteLine("Timestamp: {0}",response.timeStamp);
                }
                foreach(NodeDiscovery.SensorValuesFH.Sample sample in response.sample)
                {
                    Console.WriteLine("Sample: Delta {0} Temperature {1}",sample.delta_t,sample.Temperature.value);
                }
                
            }

            #region EventSourceCallback Members

            public void SubscriptionEnd(SubscriptionEnd1 request)
            {
                Console.WriteLine("Event closed.");
            }

            #endregion
        }
    }


}
