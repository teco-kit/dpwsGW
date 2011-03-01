﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.1
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace NodeDiscovery.DataLogging {
    
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.ServiceContractAttribute(Namespace="http://www.teco.edu/DataLoggingService", ConfigurationName="DataLogging.DataLoggingService", CallbackContract=typeof(NodeDiscovery.DataLogging.DataLoggingServiceCallback))]
    public interface DataLoggingService {
        
        // CODEGEN: Generating message contract since the operation StartLogging is neither RPC nor document wrapped.
        [System.ServiceModel.OperationContractAttribute(Action="http://www.teco.edu/DataLoggingService/StartLoggingIn", ReplyAction="http://www.teco.edu/DataLoggingService/StartLoggingOut")]
        [System.ServiceModel.XmlSerializerFormatAttribute(SupportFaults=true)]
        NodeDiscovery.DataLogging.StartLoggingResponse StartLogging(NodeDiscovery.DataLogging.StartLoggingRequest request);
        
        // CODEGEN: Generating message contract since the operation StartDownload is neither RPC nor document wrapped.
        [System.ServiceModel.OperationContractAttribute(Action="http://www.teco.edu/DataLoggingService/StartDownloadIn", ReplyAction="http://www.teco.edu/DataLoggingService/StartDownloadOut")]
        [System.ServiceModel.XmlSerializerFormatAttribute(SupportFaults=true)]
        NodeDiscovery.DataLogging.StartDownloadResponse StartDownload(NodeDiscovery.DataLogging.StartDownloadRequest request);
        
        // CODEGEN: Generating message contract since the operation Erase is neither RPC nor document wrapped.
        [System.ServiceModel.OperationContractAttribute(Action="http://www.teco.edu/DataLoggingService/EraseIn", ReplyAction="http://www.teco.edu/DataLoggingService/EraseOut")]
        [System.ServiceModel.XmlSerializerFormatAttribute(SupportFaults=true)]
        NodeDiscovery.DataLogging.EraseResponse Erase(NodeDiscovery.DataLogging.EraseRequest request);
        
        // CODEGEN: Generating message contract since the operation GetSessionCount is neither RPC nor document wrapped.
        [System.ServiceModel.OperationContractAttribute(Action="http://www.teco.edu/DataLoggingService/GetSessionCountIn", ReplyAction="http://www.teco.edu/DataLoggingService/GetSessionCountOut")]
        [System.ServiceModel.XmlSerializerFormatAttribute(SupportFaults=true)]
        NodeDiscovery.DataLogging.GetSessionCountResponse GetSessionCount(NodeDiscovery.DataLogging.GetSessionCountRequest request);
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public interface DataLoggingServiceCallback {
        
        // CODEGEN: Generating message contract since the operation DataLoggingServiceEvent is neither RPC nor document wrapped.
        [System.ServiceModel.OperationContractAttribute(IsOneWay=true, Action="http://www.teco.edu/DataLoggingService/DataLoggingServiceEventOut")]
        [System.ServiceModel.XmlSerializerFormatAttribute(SupportFaults=true)]
        void DataLoggingServiceEvent(NodeDiscovery.DataLogging.DataLoggingServiceEvent request);
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "4.0.30319.1")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.teco.edu/DataLoggingService")]
    public partial class AccelerationSeries : object, System.ComponentModel.INotifyPropertyChanged {
        
        private string countField;
        
        private System.DateTime timestampField;
        
        private bool timestampFieldSpecified;
        
        private string deltaField;
        
        private AccelerationSample[] sampleField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(DataType="nonNegativeInteger", Order=0)]
        public string count {
            get {
                return this.countField;
            }
            set {
                this.countField = value;
                this.RaisePropertyChanged("count");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Order=1)]
        public System.DateTime timestamp {
            get {
                return this.timestampField;
            }
            set {
                this.timestampField = value;
                this.RaisePropertyChanged("timestamp");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool timestampSpecified {
            get {
                return this.timestampFieldSpecified;
            }
            set {
                this.timestampFieldSpecified = value;
                this.RaisePropertyChanged("timestampSpecified");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(DataType="duration", Order=2)]
        public string delta {
            get {
                return this.deltaField;
            }
            set {
                this.deltaField = value;
                this.RaisePropertyChanged("delta");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("sample", Order=3)]
        public AccelerationSample[] sample {
            get {
                return this.sampleField;
            }
            set {
                this.sampleField = value;
                this.RaisePropertyChanged("sample");
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "4.0.30319.1")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.teco.edu/DataLoggingService")]
    public partial class AccelerationSample : object, System.ComponentModel.INotifyPropertyChanged {
        
        private string deltaField;
        
        private ADXL210_Sample acclField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(DataType="duration", Order=0)]
        public string delta {
            get {
                return this.deltaField;
            }
            set {
                this.deltaField = value;
                this.RaisePropertyChanged("delta");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Order=1)]
        public ADXL210_Sample accl {
            get {
                return this.acclField;
            }
            set {
                this.acclField = value;
                this.RaisePropertyChanged("accl");
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "4.0.30319.1")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.teco.edu/DataLoggingService")]
    public partial class ADXL210_Sample : object, System.ComponentModel.INotifyPropertyChanged {
        
        private float xField;
        
        private float yField;
        
        private float zField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Order=0)]
        public float x {
            get {
                return this.xField;
            }
            set {
                this.xField = value;
                this.RaisePropertyChanged("x");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Order=1)]
        public float y {
            get {
                return this.yField;
            }
            set {
                this.yField = value;
                this.RaisePropertyChanged("y");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Order=2)]
        public float z {
            get {
                return this.zField;
            }
            set {
                this.zField = value;
                this.RaisePropertyChanged("z");
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "4.0.30319.1")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.teco.edu/DataLoggingService")]
    public partial class SessionInfo : object, System.ComponentModel.INotifyPropertyChanged {
        
        private string countField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(DataType="nonNegativeInteger", Order=0)]
        public string count {
            get {
                return this.countField;
            }
            set {
                this.countField = value;
                this.RaisePropertyChanged("count");
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "4.0.30319.1")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace="http://www.teco.edu/DataLoggingService")]
    public partial class LoggingConfig : object, System.ComponentModel.INotifyPropertyChanged {
        
        private LoggingConfigRate rateField;
        
        private string durationField;
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Order=0)]
        public LoggingConfigRate rate {
            get {
                return this.rateField;
            }
            set {
                this.rateField = value;
                this.RaisePropertyChanged("rate");
            }
        }
        
        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(DataType="nonNegativeInteger", Order=1)]
        public string duration {
            get {
                return this.durationField;
            }
            set {
                this.durationField = value;
                this.RaisePropertyChanged("duration");
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Xml", "4.0.30319.1")]
    [System.SerializableAttribute()]
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType=true, Namespace="http://www.teco.edu/DataLoggingService")]
    public enum LoggingConfigRate {
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("2048")]
        Item2048,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("1024")]
        Item1024,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("512")]
        Item512,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("256")]
        Item256,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("128")]
        Item128,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("64")]
        Item64,
        
        /// <remarks/>
        [System.Xml.Serialization.XmlEnumAttribute("32")]
        Item32,
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class DataLoggingServiceEvent {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.teco.edu/DataLoggingService", Order=0)]
        public NodeDiscovery.DataLogging.AccelerationSeries series;
        
        public DataLoggingServiceEvent() {
        }
        
        public DataLoggingServiceEvent(NodeDiscovery.DataLogging.AccelerationSeries series) {
            this.series = series;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class StartLoggingRequest {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.teco.edu/DataLoggingService", Order=0)]
        public NodeDiscovery.DataLogging.LoggingConfig loggingconfig;
        
        public StartLoggingRequest() {
        }
        
        public StartLoggingRequest(NodeDiscovery.DataLogging.LoggingConfig loggingconfig) {
            this.loggingconfig = loggingconfig;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class StartLoggingResponse {
        
        public StartLoggingResponse() {
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class StartDownloadRequest {
        
        public StartDownloadRequest() {
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class StartDownloadResponse {
        
        public StartDownloadResponse() {
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class EraseRequest {
        
        public EraseRequest() {
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class EraseResponse {
        
        public EraseResponse() {
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetSessionCountRequest {
        
        public GetSessionCountRequest() {
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetSessionCountResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Namespace="http://www.teco.edu/DataLoggingService", Order=0)]
        public NodeDiscovery.DataLogging.SessionInfo sessioninfo;
        
        public GetSessionCountResponse() {
        }
        
        public GetSessionCountResponse(NodeDiscovery.DataLogging.SessionInfo sessioninfo) {
            this.sessioninfo = sessioninfo;
        }
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public interface DataLoggingServiceChannel : NodeDiscovery.DataLogging.DataLoggingService, System.ServiceModel.IClientChannel {
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class DataLoggingServiceClient : System.ServiceModel.DuplexClientBase<NodeDiscovery.DataLogging.DataLoggingService>, NodeDiscovery.DataLogging.DataLoggingService {
        
        public DataLoggingServiceClient(System.ServiceModel.InstanceContext callbackInstance) : 
                base(callbackInstance) {
        }
        
        public DataLoggingServiceClient(System.ServiceModel.InstanceContext callbackInstance, string endpointConfigurationName) : 
                base(callbackInstance, endpointConfigurationName) {
        }
        
        public DataLoggingServiceClient(System.ServiceModel.InstanceContext callbackInstance, string endpointConfigurationName, string remoteAddress) : 
                base(callbackInstance, endpointConfigurationName, remoteAddress) {
        }
        
        public DataLoggingServiceClient(System.ServiceModel.InstanceContext callbackInstance, string endpointConfigurationName, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(callbackInstance, endpointConfigurationName, remoteAddress) {
        }
        
        public DataLoggingServiceClient(System.ServiceModel.InstanceContext callbackInstance, System.ServiceModel.Channels.Binding binding, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(callbackInstance, binding, remoteAddress) {
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        NodeDiscovery.DataLogging.StartLoggingResponse NodeDiscovery.DataLogging.DataLoggingService.StartLogging(NodeDiscovery.DataLogging.StartLoggingRequest request) {
            return base.Channel.StartLogging(request);
        }
        
        public void StartLogging(NodeDiscovery.DataLogging.LoggingConfig loggingconfig) {
            NodeDiscovery.DataLogging.StartLoggingRequest inValue = new NodeDiscovery.DataLogging.StartLoggingRequest();
            inValue.loggingconfig = loggingconfig;
            NodeDiscovery.DataLogging.StartLoggingResponse retVal = ((NodeDiscovery.DataLogging.DataLoggingService)(this)).StartLogging(inValue);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        NodeDiscovery.DataLogging.StartDownloadResponse NodeDiscovery.DataLogging.DataLoggingService.StartDownload(NodeDiscovery.DataLogging.StartDownloadRequest request) {
            return base.Channel.StartDownload(request);
        }
        
        public void StartDownload() {
            NodeDiscovery.DataLogging.StartDownloadRequest inValue = new NodeDiscovery.DataLogging.StartDownloadRequest();
            NodeDiscovery.DataLogging.StartDownloadResponse retVal = ((NodeDiscovery.DataLogging.DataLoggingService)(this)).StartDownload(inValue);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        NodeDiscovery.DataLogging.EraseResponse NodeDiscovery.DataLogging.DataLoggingService.Erase(NodeDiscovery.DataLogging.EraseRequest request) {
            return base.Channel.Erase(request);
        }
        
        public void Erase() {
            NodeDiscovery.DataLogging.EraseRequest inValue = new NodeDiscovery.DataLogging.EraseRequest();
            NodeDiscovery.DataLogging.EraseResponse retVal = ((NodeDiscovery.DataLogging.DataLoggingService)(this)).Erase(inValue);
        }
        
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Advanced)]
        NodeDiscovery.DataLogging.GetSessionCountResponse NodeDiscovery.DataLogging.DataLoggingService.GetSessionCount(NodeDiscovery.DataLogging.GetSessionCountRequest request) {
            return base.Channel.GetSessionCount(request);
        }
        
        public NodeDiscovery.DataLogging.SessionInfo GetSessionCount() {
            NodeDiscovery.DataLogging.GetSessionCountRequest inValue = new NodeDiscovery.DataLogging.GetSessionCountRequest();
            NodeDiscovery.DataLogging.GetSessionCountResponse retVal = ((NodeDiscovery.DataLogging.DataLoggingService)(this)).GetSessionCount(inValue);
            return retVal.sessioninfo;
        }
    }
}