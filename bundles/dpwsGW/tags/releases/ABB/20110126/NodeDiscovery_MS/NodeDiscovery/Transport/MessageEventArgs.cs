
using System;
using System.Net;
using System.ServiceModel.Channels;

namespace NodeDiscovery
{
    /// <summary>
    /// 
    /// </summary>
    internal class MessageEventArgs : EventArgs
    {
        #region private fields

        private readonly Message message;
        private readonly EndPoint endPoint;

        #endregion

        #region properties

        /// <summary>
        /// Gets the message.
        /// </summary>
        /// <value>The message.</value>
        public Message Message
        {
            get { return message; }
        }

        /// <summary>
        /// Gets the end point.
        /// </summary>
        /// <value>The end point.</value>
        public EndPoint EndPoint
        {
            get { return endPoint; }
        }

        #endregion

        #region constants

        /// <summary>
        /// Initializes a new instance of the <see cref="MessageEventArgs"/> class.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="endPoint">The end point.</param>
        public MessageEventArgs(Message message, EndPoint endPoint)
        {
            this.message = message;
            this.endPoint = endPoint;
        }

        #endregion
    }
}
