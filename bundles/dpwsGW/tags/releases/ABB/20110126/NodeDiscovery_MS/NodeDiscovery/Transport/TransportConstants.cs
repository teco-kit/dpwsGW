namespace NodeDiscovery
{
    /// <summary>
    /// 
    /// </summary>
    internal static class TransportConstants
    {
        #region const fields

        /// <summary>
        /// The Multicast UDP port.
        /// </summary>
        public const int MulticastPort = 3702;
        /// <summary>
        /// The Multicast IPV4 addresss.
        /// </summary>
        public const string MulticastAddress = "239.255.255.250";
        /// <summary>
        /// The Multicast IPV6 addresss.
        /// </summary>
        public const string MulticastAddressIPV6 = "FF02::C";

        /// <summary>
        /// The maximum size of the header.
        /// </summary>
        public const int MaxHeaderSize = 4096;

        /// <summary>
        /// Maximum size of the soap message.
        /// </summary>
        public const int MaxMessageSize = 10240;
        
        #endregion
    }
}
