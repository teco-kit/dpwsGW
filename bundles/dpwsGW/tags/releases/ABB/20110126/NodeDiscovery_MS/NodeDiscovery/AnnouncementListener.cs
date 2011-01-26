using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NodeDiscovery
{
    public class AnnouncementListener : System.ServiceModel.Discovery.AnnouncementService
    {
        protected override IAsyncResult OnBeginOfflineAnnouncement(System.ServiceModel.Discovery.DiscoveryMessageSequence messageSequence, System.ServiceModel.Discovery.EndpointDiscoveryMetadata endpointDiscoveryMetadata, AsyncCallback callback, object state)
        {
            return base.OnBeginOfflineAnnouncement(messageSequence, endpointDiscoveryMetadata, callback, state);
        }

        protected override void OnEndOfflineAnnouncement(IAsyncResult result)
        {
            base.OnEndOfflineAnnouncement(result);
        }

        protected override IAsyncResult OnBeginOnlineAnnouncement(System.ServiceModel.Discovery.DiscoveryMessageSequence messageSequence, System.ServiceModel.Discovery.EndpointDiscoveryMetadata endpointDiscoveryMetadata, AsyncCallback callback, object state)
        {
            return base.OnBeginOnlineAnnouncement(messageSequence, endpointDiscoveryMetadata, callback, state);
        }

        protected override void OnEndOnlineAnnouncement(IAsyncResult result)
        {
            base.OnEndOnlineAnnouncement(result);
        }
    }
}
