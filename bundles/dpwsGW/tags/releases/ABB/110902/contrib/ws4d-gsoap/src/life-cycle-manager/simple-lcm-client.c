//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//
// author: Christian Fabian                                                              //
// date  : 26.07.07                                                                              //
// description: This is a simple client for operating    //
//                              with the life-cycle-manager (lcm). It    //
//                              ist possible to use every method of the  //
//                              lcm.                                                                     //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++// 
 
//++++++++++++++++++++//
//----- includes -----//
//++++++++++++++++++++//
  
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "lcm.nsmap"
#include "dpws_client.h"
#include "fileio.h"
#include "lcm-defs.h"
#include <time.h>
#include <sys/stat.h>
#include "dpws_device.h"
#include "lcmH.h"
#include "lcm_wsdl.h" 
#include "ctype.h"
#include "ziplib.h"
#include "zlib.h"
  
#ifdef WIN32
#include "winbase.h"
#include "windows.h"
#else /*  */
#include <string.h>
#include <assert.h>
#endif /*  */
  
//+++++++++++++++++++++++//
//----- definitions -----//
//+++++++++++++++++++++++//
  
#define MAX_NUM_ARGS 5
  
//+++++++++++++++++++++//
//----- variables -----//
//+++++++++++++++++++++//
char *host = NULL;
char MsgId[DPWS_MSGID_SIZE];
unsigned int CRCTab[256];
static unsigned char buffer[10000000]; // rund 10MB
ws4d_qnamelist type_list;
ws4d_qnamelist service_list;
ws4d_alloc_list alist;
struct soap discovery, client;
struct dpws_s dpws;
struct ws4d_qname *service_type;
struct wsa_eprlist_elem *type = NULL;
struct wsa_eprlist_elem *service = NULL;

//test
// wsa_eprfixedlist target_cache;
// struct wsa_eprlist_elem *device = NULL;
  
//+++++++++++++++++++++++++++++//
//----- enums and strucks -----//
//+++++++++++++++++++++++++++++//
enum cmdloop_cmds 
{ PRTHELP =
    0, INSTALL, UNINSTALL, START, STOP, RESOLVE, STATE, ALL, UPDATE, EXITCMD 
};
struct cmdloop_commands 
{
  char *str;                   // the string 
  int cmdnum;                   // the command
  int numargs;                  // the number of arguments
  char *args;                   // the args
  char *help;                   //help for the commands
} cmdloop_commands;
static struct cmdloop_commands cmdloop_cmdlist[] = { 
    {"Help", PRTHELP, 0, "", "Print this help info"}, 
  {"install", INSTALL, 0, "", "innstall a service "}, 
  {"uninstall", UNINSTALL, 1, "", "uninstall a service ( serviceID )"}, 
  {"start", START, 1, "", "start a service ( serviceID )"}, 
  {"stop", STOP, 1, "", "stop a service ( serviceID )"}, 
  {"resolve", RESOLVE, 1, "", "resolve dependencies ( serviceD )"}, 
  {"state", STATE, 1, "", "print state of a service ( serviceID )"}, 
  {"all", ALL, 0, "", "print all services ( none )"}, 
  {"update", UPDATE, 1, "", "update a service ( serviceID )"}, 
  {"Exit", EXITCMD, 0, "", "Exits the lcm-client application"} 
};


//+++++++++++++++++++++//
//----- functions -----//
//+++++++++++++++++++++//
  void
InitCRC () 
{
  int i = 0;
  int j = 0;
  for (i = 0; i < 256; i++)
    
    {
      unsigned int C = i;
      for (j = 0; j < 8; j++)
        C = (C & 1) ? (C >> 1) ^ 0xEDB88320L : (C >> 1);
      CRCTab[i] = C;
    }
}
unsigned int
CRC (unsigned int StartCRC, const void *Addr, unsigned int Size) 
{
  unsigned int i = 0;
  unsigned char *Data = (unsigned char *) Addr;
  if (CRCTab[1] == 0)
    
    {
      InitCRC ();
    }
  while (Size > 0 && ((long) Data & 7))
    
    {
      StartCRC =
        CRCTab[(unsigned char) (StartCRC ^ Data[0])] ^ (StartCRC >> 8);
      Size--;
      Data++;
    } while (Size >= 8)
    
    {
      StartCRC ^= *(unsigned long int *) Data;
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      StartCRC ^= *(unsigned long int *) (Data + 4);
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      StartCRC = CRCTab[(unsigned char) StartCRC] ^ (StartCRC >> 8);
      Data += 8;
      Size -= 8;
  } for (i = 0; i < Size; i++)
    StartCRC =
      CRCTab[(unsigned char) (StartCRC ^ Data[i])] ^ (StartCRC >> 8);
  return (StartCRC);
}
unsigned short
OldCRC (unsigned short StartCRC, const void *Addr, unsigned int Size) 
{
  unsigned char *Data = (unsigned char *) Addr;
  unsigned int i = 0;
  for (i = 0; i < Size; i++)
    
    {
      StartCRC = (StartCRC + Data[i]) & 0xffff;
      StartCRC = ((StartCRC << 1) | (StartCRC >> 15)) & 0xffff;
    }
  return (StartCRC);
}


//++++++++++++++++++++++++++++//
//----- Methos Funktions -----//
//++++++++++++++++++++++++++++//
  
//
//
//
  void
getState (struct _ns1__getStateResponse *Response, char *uuid) 
{
  struct _ns1__getState query;
  query.serviceID = uuid;
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
// TODO Win32
//      dpws_header_gen_request (&client, MsgId,
//                           wsa_eprlist_elem_get_Addrs (type),
//                           "http://www.ws4d.org/services/life-cycle-manager/getStateRequest",
//                           NULL, NULL, sizeof (struct SOAP_ENV__Header));
    
//      if (soap_call___ns1__getState (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, Response) == SOAP_OK)
  {
    return;
  }
  
//    else
  {
    soap_print_fault (&client, stdout);
    return;
  }
}
unsigned long
file_size (char *filename) 
{
  FILE * pFile = fopen (filename, "rb");
  unsigned long size = ftell (pFile);
  fseek (pFile, 0, SEEK_END);
  fclose (pFile);
  return size;
}


/////////////////////
//--> install
/////////////////////
void 
install (int argc, char **argv) 
{
  struct _ns1__install query;
  struct _ns1__installResponse Response;
  FILE * fin;
  FILE * fout;
  memset (&query, 0, sizeof (struct _ns1__install));
  printf ("\n\nCalling installService ...");
  printf ("\n Sending:");
  query.CRC = 0;
  query.Param.__ptr = buffer;
  
    //zip-File
/*    
    char *argumente[6];

	argumente[0] = "-5";
	argumente[1] = "test.zip";
	argumente[2] = "dsc00071.jpg";
	argumente[3] = "DSCN4595.JPG";
	argumente[4] = NULL;
					    
	do_zip(4, argumente);
*/ 
    
    //query.Param.__size = readfile("D:\\Workspace\\dpws-samples\\life-cycle-manager\\Build\\bin\\Debug\\simpleExample.zip", query.Param.__ptr);
    query.Param.__size =
    readfile ("/home/schorsch/workspace/life-cycle-manager/se.zip",
              query.Param.__ptr);
  if (query.Param.__size < 0)
    
    {
      printf ("Cannot read file for attachment\n");
      return;
    }
  printf ("\nsize of attachment: %d\n", query.Param.__size);
  query.Param.id = "Nix";
  query.Param.type = "application/zip";
  query.Param.options = "Nix";
  query.__anyAttribute = "Nix";
  query.CRC = CRC (0, query.Param.__ptr, query.Param.__size);
  printf ("\nCRC-Prüfsumme: %X", query.CRC);
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
//TODO Win32
//      dpws_header_gen_request (&client, MsgId,
//                           wsa_eprlist_elem_get_Addrs (type),
//                           "http://www.ws4d.org/services/life-cycle-manager/installRequest",
//                           NULL, NULL, sizeof (struct SOAP_ENV__Header));
    
//      if (soap_call___ns1__install (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
  {
    if (Response.status == LCM_OK)
      
      {
        printf ("\n Response: \n\t status\t: %d \n\t serviceID: %s \n\n",
                 Response.status, Response.serviceID);
      }
    
    else
      
      {
        printf ("\n--> %s <--\n", error_list[Response.status].string);
      }
  }
  
//    else
  {
    printf ("\nERROR: could not call installService\n");
    soap_print_fault (&client, stdout);
  }
}


///////////////////////
//--> uninstallService
///////////////////////
void 
uninstall (int argc, char **argv) 
{
  struct _ns1__uninstall query;
  struct _ns1__uninstallResponse Response;
  query.serviceID = argv[0];
  printf ("\n\nCalling uninstall ...");
  printf ("\n Sending:");
  printf ("\n\t serviceID    : %s ", query.serviceID);
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
/* TODO Win32
	dpws_header_gen_request (&client, MsgId,
                           wsa_eprlist_elem_get_Addrs (type),
                           "http://www.ws4d.org/services/life-cycle-manager/uninstallRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

	if (soap_call___ns1__uninstall (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
    {
      
      	if( Response.status == LCM_OK )
    	{
      		printf ("\n Response: \n\t status: %d\n\n", Response.status);
    	}
    	else
    	{
    		printf("\n--> %s <--\n", error_list[ Response.status ].string );	
    	}
    }
    else
    {
      soap_print_fault (&client, stdout);
    }
*/ 
} 

///////////////////
//--> start
///////////////////
void 
start (int argc, char **argv) 
{
  struct _ns1__start query;
  struct _ns1__startResponse Response;
  query.serviceID = argv[0];
  printf ("\n\nCalling start ...");
  printf ("\n Sending:");
  printf ("\n\t serviceID    : %s ", query.serviceID);
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
/* TODO Win32
	dpws_header_gen_request (&client, MsgId,
                           wsa_eprlist_elem_get_Addrs (type),
                           "http://www.ws4d.org/services/life-cycle-manager/startRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

	if (soap_call___ns1__start (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
    {      
      	if( Response.status == LCM_OK )
    	{
      		printf ("\n Response: \n\t status: %d\n\n", Response.status);
    	}
    	else
    	{
    		printf("\n--> %s <--\n", error_list[ Response.status ].string );	
    	}
    }
    else
    {
      soap_print_fault (&client, stdout);
    }
*/ 
} 

//////////////////
//--> stop
//////////////////
void 
stop (int argc, char **argv) 
{
  struct _ns1__stop query;
  struct _ns1__stopResponse Response;
  query.serviceID = argv[0];
  printf ("\n\nCalling stop ...");
  printf ("\n Sending:");
  printf ("\n\t serviceID    : %s ", query.serviceID);
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
/* TODO Win32
	dpws_header_gen_request (&client, MsgId,
                           wsa_eprlist_elem_get_Addrs (type),
                           "http://www.ws4d.org/services/life-cycle-manager/stopRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

	if (soap_call___ns1__stop (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
    {
      	if( Response.status == LCM_OK )
    	{
      		printf ("\n Response: \n\t status: %d\n\n", Response.status);
    	}
    	else
    	{
    		printf("\n--> %s <--\n", error_list[ Response.status ].string );	
    	}
    }
    else
    {
      soap_print_fault (&client, stdout);
    }
*/ 
} 

/////////////////////
//--> resolve
/////////////////////
void 
resolve (int argc, char **argv) 
{
  struct _ns1__resolve query;
  struct _ns1__resolveResponse Response;
  query.serviceID = argv[0];
  printf ("\n\nCalling resolve ...");
  printf ("\n Sending:");
  printf ("\n\t serviceID    : %s ", query.serviceID);
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
/* TODO Win32
	dpws_header_gen_request (&client, MsgId,
                           wsa_eprlist_elem_get_Addrs (type),
                           "http://www.ws4d.org/services/life-cycle-manager/resolveRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

	if (soap_call___ns1__resolve (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
    {
      	if( Response.status == LCM_OK )
    	{
      		printf ("\n Response: \n\t status: %d\n\n", Response.status);
    	}
    	else
    	{
    		printf("\n--> %s <--\n", error_list[ Response.status ].string );	
    	}
    }
    else
    {
      soap_print_fault (&client, stdout);
    }
*/ 
} 

///////////////////
//--> allServices
///////////////////
void 
allServices (int argc, char **argv) 
{
  struct _ns1__ListAll query;
  struct _ns1__ListAllResponse Response;
  int i = 0;
  
    //query.serviceID     = argv[0];                
    printf ("\n\nCalling ListAll ...");
  printf ("\n Sending:");
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
/* TODO Win32
	dpws_header_gen_request (&client, MsgId,
                           wsa_eprlist_elem_get_Addrs (type),
                           "http://www.ws4d.org/services/life-cycle-manager/ListAllRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

	if (soap_call___ns1__ListAll (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
    {
    	struct _ns1__getStateResponse State;
    	
       if( Response.status == LCM_OK )
    	{
    		printf ("\n Response: \n\t status: %d\n\tsize: %d\n", Response.status, Response.__sizeservices);
    		
    		for( i=0; i < Response.__sizeservices; i++)
    		{
    			printf("\n\t service %d: %s", i, Response.services[i] );	
    		}  
    	}
    	else
    	{
    		printf("\n--> %s <--\n", error_list[ Response.status ].string );
    		return;	
    	}
    
          
    }
    else
    {
      soap_print_fault (&client, stdout);
    }
*/ 
} 

///////////////////
//--> state
///////////////////
void 
state (int argc, char **argv) 
{
  struct _ns1__getState query;
  struct _ns1__getStateResponse Response;
  query.serviceID = argv[0];
  printf ("\n\nCalling getState ...");
  printf ("\n Sending:");
  printf ("\n\t serviceID    : %s ", query.serviceID);
  soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
/* TODO Win32
	dpws_header_gen_request (&client, MsgId,
                           wsa_eprlist_elem_get_Addrs (type),
                           "http://www.ws4d.org/services/life-cycle-manager/getStateRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

	if (soap_call___ns1__getState (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
    {	
		if( Response.status == LCM_OK )
    	{
      		printf ("\n Response: \n\t status\t: %d\n\t \n\n", Response.status);
	      
	        printf("\n\t\t serviceState : %d", Response.serviceState);
			printf("\n\t\t valid       : %d", Response.valid);
			printf("\n\t\t id        : %s", (Response.uuid)?(Response.uuid):("NULL") );
			printf("\n\t\t File      : %s", (Response.soFile)?(Response.soFile):("NULL") );
			printf("\n\t\t host        : %s", (Response.host)?(Response.host):("NULL") );
			printf("\n\t\t version     : %s", (Response.version)?(Response.version):("NULL") );
    	}
    	else
    	{
    		printf("\n--> %s <--\n", error_list[ Response.status ].string );	
    	}
	
    }
    else
    {
      soap_print_fault (&client, stdout);
    }
*/ 
} 

////////////////////
//--> update
////////////////////
void 
update (int argc, char **argv) 
{
  struct _ns1__update query;
  struct _ns1__updateResponse Response;
  memset (&query, 0, sizeof (struct _ns1__update));
  query.serviceID = argv[0];
  printf ("\n\nCalling update ...");
  printf ("\n Sending:");
  printf ("\n\t serviceID: %s ", query.serviceID);
  
    // add attachement
    query.CRC = 0;
  query.Param.__ptr = buffer;
  
    //query.Param.__size = readfile("simpleExample_03.zip", query.Param.__ptr);
    //query.Param.__size = readfile("D:\\Workspace\\dpws-samples\\life-cycle-manager\\Build\\bin\\simpleExample_03.zip", query.Param.__ptr);
    query.Param.__size =
    readfile ("/home/schorsch/workspace/life-cycle-manager/se_update.zip",
              query.Param.__ptr);
  if (query.Param.__size < 0)
    
    {
      printf ("Cannot read file for attachment\n");
      return;
    }
  printf ("\nsize of attachment: %d\n", query.Param.__size);
  query.Param.id = NULL;
  query.Param.type = "application/zip";
  query.Param.options = NULL;
  query.__anyAttribute = NULL;
  
    //query.CRC = CRC( 0, query.Param.__ptr, query.Param.__size);
    printf ("\nCRC-Prüfsumme: %X", query.CRC);
  
    // calling Methode
    soap_set_namespaces (&client, lcm_namespaces);
  dpws_header_gen_MessageId (MsgId, DPWS_MSGID_SIZE);
  
/* TODO Win32
	dpws_header_gen_request (&client, MsgId,
                           wsa_eprlist_elem_get_Addrs (type),
                           "http://www.ws4d.org/services/life-cycle-manager/updateRequest",
                           NULL, NULL, sizeof (struct SOAP_ENV__Header));

	if (soap_call___ns1__update (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
    {
      	if( Response.status == LCM_OK )
    	{
      		printf ("\n Response: \n\t status\t: %d\n\t \n\n", Response.status);
    	}
    	else
    	{
    		printf("\n--> %s <--\n", error_list[ Response.status ].string );	
    	}
    }
    else
    {
      soap_print_fault (&client, stdout);
    }
*/ 
}


//+++++++++++++++++++++++++++++//
//----- command functions -----//
//+++++++++++++++++++++++++++++//
  
//+++++++++++++++++++++++++++
//--> print help information
//+++++++++++++++++++++++++++
  void
print_help () 
{
  int numofcmds = sizeof (cmdloop_cmdlist) / sizeof (cmdloop_commands);
  int i;
  printf ("\n");
  printf ("***********************************\n");
  printf ("* gSOAP DPWS Supervisor Help Info *\n");
  printf ("***********************************\n");
  printf ("\n");
  printf ("Commands:\n");
  printf ("\n");
  for (i = 0; i < numofcmds; i++)
    
    {
      printf ("\t%s: (No. of args:%d)\n", cmdloop_cmdlist[i].str,
               cmdloop_cmdlist[i].numargs);
      printf ("\t\t%s\n", cmdloop_cmdlist[i].help);
      printf ("\n");
    }
}


//++++++++++++++++++
//--> exit function 
//++++++++++++++++++
  void
exit_lcm_client () 
{
  soap_end (&discovery);
  soap_done (&discovery);
  dpws_done (&dpws);
  ws4d_alloclist_done (&alist);
  exit (0);
} 

//++++++++++++++++++++++++++++++++++
//--> process commandline parameter
//++++++++++++++++++++++++++++++++++
  int
process_command (char *cmdline) 
{
  char *cmd = NULL, *save_ptr = NULL, **args = NULL;
  int cmdnum = -1;
  int numofcmds = sizeof (cmdloop_cmdlist) / sizeof (cmdloop_commands);
  int cmdfound = 0, num_args = 0, invalid_args = 0;
  int i;
  args = ws4d_malloc (sizeof (char *) * MAX_NUM_ARGS);
  while (memchr (cmdline, '\n', strlen (cmdline)))
    
    {
      char *ret = NULL;
      ret = memchr (cmdline, '\n', strlen (cmdline));
      ret[0] = '\0';
    } cmd = STRTOK (cmdline, " ", &save_ptr);
  
  do
    
    {
      args[num_args] = STRTOK (NULL, " ", &save_ptr);
      if (args[num_args] != NULL)
        
        {
          num_args++;
        }
      
      else
        
        {
          break;
        }
    }
  while (num_args < MAX_NUM_ARGS);
  for (i = 0; i < numofcmds; i++)
    
    {
      if (cmd && STRCASECMP (cmd, cmdloop_cmdlist[i].str) == 0)
        
        {
          cmdnum = cmdloop_cmdlist[i].cmdnum;
          cmdfound++;
          if (num_args < cmdloop_cmdlist[i].numargs)
            
            {
              invalid_args = 1;
            }
          break;
        }
    }
  if (!cmdfound)
    
    {
      printf ("\n\nCommand not found\n");
      print_help ();
      return 0;
    }
  if (invalid_args)
    
    {
      printf ("\n\nInvalid arguments\n");
      print_help ();
      return 0;
    }
  switch (cmdnum)
    
    {
    case PRTHELP:
      print_help ();
      break;
    case INSTALL:
      install (num_args, args);
      break;
    case UNINSTALL:
      uninstall (num_args, args);
      break;
    case START:
      start (num_args, args);
      break;
    case STOP:
      stop (num_args, args);
      break;
    case RESOLVE:
      resolve (num_args, args);
      break;
    case STATE:
      state (num_args, args);
      break;
    case ALL:
      allServices (num_args, args);
      break;
    case UPDATE:
      update (num_args, args);
      break;
    case EXITCMD:
      exit_lcm_client ();
      break;
    default:
      printf ("\n\nInvalid command\n");
      print_help ();
      break;
    }
  return 0;
}


//++++++++++++++++++++++++++//
//------ main funktion -----//
//++++++++++++++++++++++++++//
  int
main (int argc, char **argv) 
{
  char *XAddrs = NULL;
  char cmdline[100];
  FILE * fin;
  FILE * fout;
  
    // parsing command line options 
    while ((argc > 1) && (argv[1][0] == '-'))
    
    {
      char *option = &argv[1][1];
      switch (option[0])
        
        {
        case 'h':
          if (strlen (option) > 2)
            
            {
              ++option;
              host = option;
            }
          
          else
            
            {
              --argc;
              ++argv;
              host = argv[1];
            }
          printf ("\nSet host to \"%s\"", host);
          fflush (NULL);
          break;
        default:
          fprintf (stderr, "\nBad option %s", argv[1]);
        }
      --argc;
      ++argv;
    }
  if (host == NULL)
    
    {
      printf ("\nNo host was specified!");
      fflush (NULL);
      exit (0);
    }
  WS4D_ALLOCLIST_INIT (&alist);
  
    // initialize client soap handle 
    soap_init (&client);
  soap_omode (&client, SOAP_XML_INDENT);
  
    // initialize stack 
    if (dpws_init (&dpws, host) != SOAP_OK)
    
    {
      fprintf (stderr, "\nCould not initialize dpws handle\n");
      fflush (NULL);
      dpws_done (&dpws);
      exit (1);
    }
  dpws_handle_init (&dpws, &client);
  
    // allocate and prepare service to resolve 
//TODO Win32
//  service = wsa_eprlist_alloc_elem (1, &alist);
//  wsa_eprlist_elem_set_Addrs (service, wsa_eprlist_elem_get_Addrs (service));
    ws4d_qnamelist_init (&type_list);
  ws4d_qnamelist_addstring
    ("\"http://www.uni-rostock.de/schemas/hostingService\":hostingService",
     &type_list, &alist);
  
//TODO Win32  service = wsa_eprlist_alloc_elem (1, &alist);
    if (dpws_probe (&dpws, &type_list, NULL, 6000, 100, NULL, 1, service) ==
        SOAP_OK)
    
    {
      
//TODO Win32    printf ("\nFound service %s\n", wsa_eprlist_elem_get_Addrs (service));
    }
  
  else
    
    {
      printf ("\nNo service was found!");
      return 0;
    }
  
//TODO WIN32  XAddrs = (char *) dpws_resolve_addr (&dpws, service);
    if (XAddrs != NULL)
    
    {
      
//TODO Win32
//      fprintf (stderr, "\nService %s found at addr %s\n",
//               wsa_eprlist_elem_get_Addrs (service), XAddrs);
    }
  
  else
    
    {
      
//TODO Win32      fprintf (stderr, "\nService %s cannot be found\n",
//               wsa_eprlist_elem_get_Addrs (service));
        fflush (NULL);
      exit (1);
    }
  
//TODO Win32  type = wsa_eprlist_alloc_elem (1, &alist);
    ws4d_qnamelist_init (&service_list);
  ws4d_qnamelist_addstring
    ("\"http://www.ws4d.org/services/life-cycle-manager\":life-cycle-manager",
     &service_list, &alist);
  if (dpws_find_services (&dpws, service, &service_list, 10000, 1, type) ==
        SOAP_OK)
    
    {
      
//TODO Win32      printf ("\nService offers LCM-Service at %s\n",
//              wsa_eprlist_elem_get_Addrs (type));
    }
  
  else
    
    {
      
//TODO Win32      printf ("\nLCM-Service not found on %s\n",
//              wsa_eprlist_elem_get_Addrs (service));
        exit (1);
    }
  
    // process comandline options
    while (1)
    
    {
      printf ("\n>> ");
      fgets (cmdline, 100, stdin);
      process_command (cmdline);
    }
  exit_lcm_client ();
  return 0;
}


