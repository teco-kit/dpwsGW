﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.20506.1
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace NodeDiscovery.www.teco.edu {
    using System.Runtime.Serialization;
    using System;
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="ADXL210_Sample", Namespace="http://www.teco.edu/SensorValues")]
    [System.SerializableAttribute()]
    public partial class ADXL210_Sample : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private short xField;
        
        private short yField;
        
        private short zField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private sbyte indexField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public short x {
            get {
                return this.xField;
            }
            set {
                if ((this.xField.Equals(value) != true)) {
                    this.xField = value;
                    this.RaisePropertyChanged("x");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public short y {
            get {
                return this.yField;
            }
            set {
                if ((this.yField.Equals(value) != true)) {
                    this.yField = value;
                    this.RaisePropertyChanged("y");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public short z {
            get {
                return this.zField;
            }
            set {
                if ((this.zField.Equals(value) != true)) {
                    this.zField = value;
                    this.RaisePropertyChanged("z");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=3)]
        public sbyte index {
            get {
                return this.indexField;
            }
            set {
                if ((this.indexField.Equals(value) != true)) {
                    this.indexField = value;
                    this.RaisePropertyChanged("index");
                }
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
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="SP101_Sample", Namespace="http://www.teco.edu/SensorValues")]
    [System.SerializableAttribute()]
    public partial class SP101_Sample : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private byte volumeField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private sbyte indexField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public byte volume {
            get {
                return this.volumeField;
            }
            set {
                if ((this.volumeField.Equals(value) != true)) {
                    this.volumeField = value;
                    this.RaisePropertyChanged("volume");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=1)]
        public sbyte index {
            get {
                return this.indexField;
            }
            set {
                if ((this.indexField.Equals(value) != true)) {
                    this.indexField = value;
                    this.RaisePropertyChanged("index");
                }
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
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="TSL2550_Sample", Namespace="http://www.teco.edu/SensorValues")]
    [System.SerializableAttribute()]
    public partial class TSL2550_Sample : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private ushort daylightField;
        
        private ushort infraredField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private sbyte indexField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public ushort daylight {
            get {
                return this.daylightField;
            }
            set {
                if ((this.daylightField.Equals(value) != true)) {
                    this.daylightField = value;
                    this.RaisePropertyChanged("daylight");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public ushort infrared {
            get {
                return this.infraredField;
            }
            set {
                if ((this.infraredField.Equals(value) != true)) {
                    this.infraredField = value;
                    this.RaisePropertyChanged("infrared");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=2)]
        public sbyte index {
            get {
                return this.indexField;
            }
            set {
                if ((this.indexField.Equals(value) != true)) {
                    this.indexField = value;
                    this.RaisePropertyChanged("index");
                }
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
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="FSR152_Sample", Namespace="http://www.teco.edu/SensorValues")]
    [System.SerializableAttribute()]
    public partial class FSR152_Sample : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private byte valueField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private sbyte indexField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public byte value {
            get {
                return this.valueField;
            }
            set {
                if ((this.valueField.Equals(value) != true)) {
                    this.valueField = value;
                    this.RaisePropertyChanged("value");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=1)]
        public sbyte index {
            get {
                return this.indexField;
            }
            set {
                if ((this.indexField.Equals(value) != true)) {
                    this.indexField = value;
                    this.RaisePropertyChanged("index");
                }
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
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="TC74_Sample", Namespace="http://www.teco.edu/SensorValues")]
    [System.SerializableAttribute()]
    public partial class TC74_Sample : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private sbyte valueField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private sbyte indexField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public sbyte value {
            get {
                return this.valueField;
            }
            set {
                if ((this.valueField.Equals(value) != true)) {
                    this.valueField = value;
                    this.RaisePropertyChanged("value");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=1)]
        public sbyte index {
            get {
                return this.indexField;
            }
            set {
                if ((this.indexField.Equals(value) != true)) {
                    this.indexField = value;
                    this.RaisePropertyChanged("index");
                }
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
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="SensorConfigurationType", Namespace="http://www.teco.edu/SensorValues")]
    [System.SerializableAttribute()]
    public partial class SensorConfigurationType : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private NodeDiscovery.www.teco.edu.SSimpRateConfig AccelerationField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private NodeDiscovery.www.teco.edu.SSimpRateConfig AudioField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private NodeDiscovery.www.teco.edu.SSimpRateConfig LightField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private NodeDiscovery.www.teco.edu.SSimpRateConfig AmbientLightField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private NodeDiscovery.www.teco.edu.SSimpRateConfig ForceField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private NodeDiscovery.www.teco.edu.SSimpRateConfig TemperatureField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false)]
        public NodeDiscovery.www.teco.edu.SSimpRateConfig Acceleration {
            get {
                return this.AccelerationField;
            }
            set {
                if ((object.ReferenceEquals(this.AccelerationField, value) != true)) {
                    this.AccelerationField = value;
                    this.RaisePropertyChanged("Acceleration");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false)]
        public NodeDiscovery.www.teco.edu.SSimpRateConfig Audio {
            get {
                return this.AudioField;
            }
            set {
                if ((object.ReferenceEquals(this.AudioField, value) != true)) {
                    this.AudioField = value;
                    this.RaisePropertyChanged("Audio");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false)]
        public NodeDiscovery.www.teco.edu.SSimpRateConfig Light {
            get {
                return this.LightField;
            }
            set {
                if ((object.ReferenceEquals(this.LightField, value) != true)) {
                    this.LightField = value;
                    this.RaisePropertyChanged("Light");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=3)]
        public NodeDiscovery.www.teco.edu.SSimpRateConfig AmbientLight {
            get {
                return this.AmbientLightField;
            }
            set {
                if ((object.ReferenceEquals(this.AmbientLightField, value) != true)) {
                    this.AmbientLightField = value;
                    this.RaisePropertyChanged("AmbientLight");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=4)]
        public NodeDiscovery.www.teco.edu.SSimpRateConfig Force {
            get {
                return this.ForceField;
            }
            set {
                if ((object.ReferenceEquals(this.ForceField, value) != true)) {
                    this.ForceField = value;
                    this.RaisePropertyChanged("Force");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=5)]
        public NodeDiscovery.www.teco.edu.SSimpRateConfig Temperature {
            get {
                return this.TemperatureField;
            }
            set {
                if ((object.ReferenceEquals(this.TemperatureField, value) != true)) {
                    this.TemperatureField = value;
                    this.RaisePropertyChanged("Temperature");
                }
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
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="SSimpRateConfig", Namespace="http://www.teco.edu/SensorValues")]
    [System.SerializableAttribute()]
    public partial class SSimpRateConfig : object, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        private sbyte rateField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute(IsRequired=true)]
        public sbyte rate {
            get {
                return this.rateField;
            }
            set {
                if ((this.rateField.Equals(value) != true)) {
                    this.rateField = value;
                    this.RaisePropertyChanged("rate");
                }
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
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.ServiceContractAttribute(Namespace="http://www.teco.edu/SensorValues", ConfigurationName="www.teco.edu.SensorValues")]
    public interface SensorValues {
        
        [System.ServiceModel.OperationContractAttribute(Action="http://www.teco.edu/SensorValues/GetSensorValuesIn", ReplyAction="*")]
        NodeDiscovery.www.teco.edu.GetSensorValuesResponse GetSensorValues(NodeDiscovery.www.teco.edu.GetSensorValuesRequest request);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://www.teco.edu/SensorValues/SensorValues/ConfigRequest", ReplyAction="http://www.teco.edu/SensorValues/SensorValues/ConfigResponse")]
        NodeDiscovery.www.teco.edu.ConfigResponse Config(NodeDiscovery.www.teco.edu.ConfigRequest request);
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetSensorValuesRequest {
        
        public GetSensorValuesRequest() {
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class GetSensorValuesResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="Sample", Namespace="http://www.teco.edu/SensorValues", Order=0)]
        public NodeDiscovery.www.teco.edu.GetSensorValuesResponseBody Body;
        
        public GetSensorValuesResponse() {
        }
        
        public GetSensorValuesResponse(NodeDiscovery.www.teco.edu.GetSensorValuesResponseBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://www.teco.edu/SensorValues")]
    public partial class GetSensorValuesResponseBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=0)]
        public System.DateTime TimeStamp;
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=1)]
        public NodeDiscovery.www.teco.edu.ADXL210_Sample Accelleration;
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=2)]
        public NodeDiscovery.www.teco.edu.SP101_Sample Audio;
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=3)]
        public NodeDiscovery.www.teco.edu.TSL2550_Sample Light;
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=4)]
        public NodeDiscovery.www.teco.edu.FSR152_Sample Force;
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=5)]
        public NodeDiscovery.www.teco.edu.TC74_Sample Temperature;
        
        public GetSensorValuesResponseBody() {
        }
        
        public GetSensorValuesResponseBody(System.DateTime TimeStamp, NodeDiscovery.www.teco.edu.ADXL210_Sample Accelleration, NodeDiscovery.www.teco.edu.SP101_Sample Audio, NodeDiscovery.www.teco.edu.TSL2550_Sample Light, NodeDiscovery.www.teco.edu.FSR152_Sample Force, NodeDiscovery.www.teco.edu.TC74_Sample Temperature) {
            this.TimeStamp = TimeStamp;
            this.Accelleration = Accelleration;
            this.Audio = Audio;
            this.Light = Light;
            this.Force = Force;
            this.Temperature = Temperature;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class ConfigRequest {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="StatusControl", Namespace="http://www.teco.edu/SensorValues", Order=0)]
        public NodeDiscovery.www.teco.edu.ConfigRequestBody Body;
        
        public ConfigRequest() {
        }
        
        public ConfigRequest(NodeDiscovery.www.teco.edu.ConfigRequestBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://www.teco.edu/SensorValues")]
    public partial class ConfigRequestBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public NodeDiscovery.www.teco.edu.SensorConfigurationType SensorConfig;
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=1)]
        public System.DateTime NewTime;
        
        public ConfigRequestBody() {
        }
        
        public ConfigRequestBody(NodeDiscovery.www.teco.edu.SensorConfigurationType SensorConfig, System.DateTime NewTime) {
            this.SensorConfig = SensorConfig;
            this.NewTime = NewTime;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.MessageContractAttribute(IsWrapped=false)]
    public partial class ConfigResponse {
        
        [System.ServiceModel.MessageBodyMemberAttribute(Name="Status", Namespace="http://www.teco.edu/SensorValues", Order=0)]
        public NodeDiscovery.www.teco.edu.ConfigResponseBody Body;
        
        public ConfigResponse() {
        }
        
        public ConfigResponse(NodeDiscovery.www.teco.edu.ConfigResponseBody Body) {
            this.Body = Body;
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Namespace="http://www.teco.edu/SensorValues")]
    public partial class ConfigResponseBody {
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=0)]
        public NodeDiscovery.www.teco.edu.SensorConfigurationType SensorConfig;
        
        [System.Runtime.Serialization.DataMemberAttribute(EmitDefaultValue=false, Order=1)]
        public NodeDiscovery.www.teco.edu.SensorConfigurationType AllSensorConfig;
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=2)]
        public ushort BatteryVoltage;
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=3)]
        public System.DateTime CurrentTime;
        
        [System.Runtime.Serialization.DataMemberAttribute(Order=4)]
        public System.DateTime UpTime;
        
        public ConfigResponseBody() {
        }
        
        public ConfigResponseBody(NodeDiscovery.www.teco.edu.SensorConfigurationType SensorConfig, NodeDiscovery.www.teco.edu.SensorConfigurationType AllSensorConfig, ushort BatteryVoltage, System.DateTime CurrentTime, System.DateTime UpTime) {
            this.SensorConfig = SensorConfig;
            this.AllSensorConfig = AllSensorConfig;
            this.BatteryVoltage = BatteryVoltage;
            this.CurrentTime = CurrentTime;
            this.UpTime = UpTime;
        }
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public interface SensorValuesChannel : NodeDiscovery.www.teco.edu.SensorValues, System.ServiceModel.IClientChannel {
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class SensorValuesClient : System.ServiceModel.ClientBase<NodeDiscovery.www.teco.edu.SensorValues>, NodeDiscovery.www.teco.edu.SensorValues {
        
        public SensorValuesClient() {
        }
        
        public SensorValuesClient(string endpointConfigurationName) : 
                base(endpointConfigurationName) {
        }
        
        public SensorValuesClient(string endpointConfigurationName, string remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public SensorValuesClient(string endpointConfigurationName, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public SensorValuesClient(System.ServiceModel.Channels.Binding binding, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(binding, remoteAddress) {
        }
        
        public NodeDiscovery.www.teco.edu.GetSensorValuesResponse GetSensorValues(NodeDiscovery.www.teco.edu.GetSensorValuesRequest request) {
            return base.Channel.GetSensorValues(request);
        }
        
        public NodeDiscovery.www.teco.edu.ConfigResponse Config(NodeDiscovery.www.teco.edu.ConfigRequest request) {
            return base.Channel.Config(request);
        }
    }
}