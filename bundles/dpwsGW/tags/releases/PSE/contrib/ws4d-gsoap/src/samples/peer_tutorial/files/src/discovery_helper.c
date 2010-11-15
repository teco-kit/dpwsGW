#include "dpwsH.h"
#include "stddpws.h"
#include "discovery_helper.h"
#include <pthread.h>
#include <signal.h>

struct callbacks_s
{
  discovery_helper_addDevice addDevice;
  discovery_helper_invalidateDevice invalidateDevice;
} callbacks =
{
NULL, NULL};


struct soap discovery;
pthread_t discovery_th;

static int helper_running = 0;

static void
new_device (struct soap *soap, struct ws4d_epr *device)
{

  if (device && ws4d_epr_get_Addrs (device))
    {
      if (callbacks.addDevice)
        {
          callbacks.addDevice (device);
        }
    }
}

static void
invalidate_device (struct soap *soap, struct ws4d_epr *device)
{
  if (device && ws4d_epr_get_Addrs (device))
    {
      if (callbacks.invalidateDevice)
        {
          callbacks.invalidateDevice (device);
        }
    }
}

struct wsd_dis_hooks_t discovery_hooks = { new_device, invalidate_device };

static void
discovery_helper_exit ()
{
  /* clean up */
  soap_end (&discovery);

  soap_done (&discovery);

  pthread_exit (EXIT_SUCCESS);
}

static void
discovery_helper_loop ()
{
  int err;

  while (helper_running)
    {
      err = wsd_process (&discovery, 1000);
      if ((err != SOAP_EOF) && (err != SOAP_OK) && (err != SOAP_STOP))
        {
          soap_print_fault (&discovery, stderr);
        }
    }

  discovery_helper_exit ();
}

int
discovery_helper_start (struct dpws_s *dpws)
{
  int err;

  if (helper_running != 0)
    {
      return WS4D_ERR;
    }
  helper_running = 1;

  /* initialize client soap handle */
  soap_init (&discovery);
  soap_omode (&discovery, SOAP_XML_INDENT);

  err = dpws_init_implicit_discovery (dpws, &discovery, &discovery_hooks);
  if (err != SOAP_OK)
    {
      return err;
    }

  if (pthread_create
      (&discovery_th, NULL, (void *) &discovery_helper_loop, NULL))
    {
      return WS4D_ERR;
    }

  return WS4D_OK;
}

int
discovery_helper_stop ()
{
  if (helper_running != 1)
    {
      return WS4D_ERR;
    }
  helper_running = 0;
}

void
discovery_helper_kill ()
{
  pthread_kill (discovery_th, SIGABRT);
}

int
discovery_helper_addDeviceCB (discovery_helper_addDevice cb)
{
  callbacks.addDevice = cb;

  return WS4D_OK;
}

int
discovery_helper_invalidateDeviceCB (discovery_helper_invalidateDevice cb)
{
  callbacks.invalidateDevice = cb;
  return WS4D_OK;
}
