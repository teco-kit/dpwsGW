using System;
using System.IO;
using System.Net.Sockets;
using System.Net;
using System.ServiceModel.Channels;
using System.Text;
using System.Threading;
using System.Xml;

namespace NodeDiscovery
{
    /// <summary>
    /// MulticastListener class
    /// </summary>
    internal class MulticastListener : IDisposable
    {
        #region events

        /// <summary>
        /// Occurs when a message has arrived.
        /// </summary>
        public event EventHandler<MessageEventArgs> MessageArrived;

        #endregion

        #region private fields

        private static readonly IPAddress multicastAddress = IPAddress.Parse(TransportConstants.MulticastAddress);
        private readonly Thread _threadListener;
        private readonly Socket socket;
        private bool _disposing;

        #endregion

        #region constrcutor

        /// <summary>
        /// Initializes a new instance of the <see cref="MulticastListener"/> class.
        /// </summary>
        public MulticastListener()
        {
            // Create the Socket
            socket = new Socket(AddressFamily.InterNetwork,
                                     SocketType.Dgram,
                                     ProtocolType.Udp);

            // Set the reuse address option
            socket.SetSocketOption(SocketOptionLevel.Socket,
                                 SocketOptionName.ReuseAddress, 1);

            // Create an IPEndPoint and bind to it
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, TransportConstants.MulticastPort);
            socket.Bind(ipep);

            try
            {
                // Add membership in the multicast group
                socket.SetSocketOption(SocketOptionLevel.IP,
                                     SocketOptionName.AddMembership,
                                     new MulticastOption(multicastAddress, IPAddress.Any));

            }
            catch (SocketException sex)
            {
                throw new InvalidOperationException("Network does not support multicast communication, check your network settings", sex);
            }

            //Start listening thread
            _threadListener = new Thread(Listener)
                                  {
                                      IsBackground = true,
                                      Name = "MulticastListener"
                                  };
            _threadListener.Start();
        }

        #endregion

        #region methods

        /// <summary>
        /// Listeners the specified sock.
        /// </summary>
        private void Listener()
        {
            while (true)
            {
                byte[] recData = new byte[TransportConstants.MaxMessageSize];
                EndPoint tempReceivePoint = new IPEndPoint(IPAddress.Any, 0);

                try
                {
                    // Receive the multicast packets
                    int length = socket.ReceiveFrom(recData, 0, TransportConstants.MaxMessageSize,
                                                                     SocketFlags.None,
                                                                     ref tempReceivePoint);

                    ReceiveData(tempReceivePoint, recData, 0, length);
                }
                catch (SocketException)
                {
                    if (_disposing)
                    {
                        return;
                    }
                }
            }
        }

        /// <summary>
        /// Receives the data.
        /// </summary>
        /// <param name="endPoint">The end point.</param>
        /// <param name="data">The data.</param>
        /// <param name="offset">The offset.</param>
        /// <param name="length">The length.</param>
        protected void ReceiveData(EndPoint endPoint, byte[] data, int offset, int length)
        {
            try
            {
                string envelope = Encoding.ASCII.GetString(data, offset, length);

                StringReader stringReader = new StringReader(envelope);
                XmlTextReader reader = new XmlTextReader(stringReader);
                Message message = Message.CreateMessage(reader, TransportConstants.MaxHeaderSize, MessageVersion.Soap12WSAddressingAugust2004);

                OnMessageArrived(new MessageEventArgs(message, endPoint));
            }
            catch (Exception ex)
            {
                //TCO TODO Logging
            }
        }

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            if (!_disposing)
            {
                _disposing = true;

                // Drop membership
                socket.SetSocketOption(SocketOptionLevel.IP,
                                     SocketOptionName.DropMembership,
                                     new MulticastOption(multicastAddress,
                                                         IPAddress.Any));
                // Close the socket
                socket.Close();
            }
        }

        /// <summary>
        /// Raises the <see cref="MessageArrived"/> event.
        /// </summary>
        /// <param name="eventArgs">The <see cref="System.EventArgs"/> instance containing the event data.</param>
        protected void OnMessageArrived(MessageEventArgs eventArgs)
        {
            if (MessageArrived != null)
            {
                MessageArrived(this, eventArgs);
            }
        }

        #endregion
    }
}