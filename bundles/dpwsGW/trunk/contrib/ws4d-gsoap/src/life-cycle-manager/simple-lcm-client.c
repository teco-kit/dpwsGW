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
#else /* 
#include <string.h>
#include <assert.h>
#endif /* 
  
//+++++++++++++++++++++++//
//----- definitions -----//
//+++++++++++++++++++++++//
  
#define MAX_NUM_ARGS 5
  
//+++++++++++++++++++++//
//----- variables -----//
//+++++++++++++++++++++//




ws4d_qnamelist type_list;








//test
// wsa_eprfixedlist target_cache;
// struct wsa_eprlist_elem *device = NULL;
  
//+++++++++++++++++++++++++++++//
//----- enums and strucks -----//
//+++++++++++++++++++++++++++++//

{ 
    0, INSTALL, UNINSTALL, START, STOP, RESOLVE, STATE, ALL, UPDATE, EXITCMD 
};

{
  
  int cmdnum;                   // the command
  int numargs;                  // the number of arguments
  char *args;                   // the args
  char *help;                   //help for the commands
} cmdloop_commands;

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
  
InitCRC () 
{
  
  
  
    
    {
      
      
        
      
    


CRC (unsigned int StartCRC, const void *Addr, unsigned int Size) 
{
  
  
  
    
    {
      
    
  
    
    {
      
        CRCTab[(unsigned char) (StartCRC ^ Data[0])] ^ (StartCRC >> 8);
      
      
    
    
    {
      
      
      
      
      
      
      
      
      
      
      
      
  
    
      CRCTab[(unsigned char) (StartCRC ^ Data[i])] ^ (StartCRC >> 8);
  


OldCRC (unsigned short StartCRC, const void *Addr, unsigned int Size) 
{
  
  
  
    
    {
      
      
    
  



//++++++++++++++++++++++++++++//
//----- Methos Funktions -----//
//++++++++++++++++++++++++++++//
  
//
//
//
  void
getState (struct _ns1__getStateResponse *Response, char *uuid) 
{
  
  
  
  
  
// TODO Win32
//      dpws_header_gen_request (&client, MsgId,
//                           wsa_eprlist_elem_get_Addrs (type),
//                           "http://www.ws4d.org/services/life-cycle-manager/getStateRequest",
//                           NULL, NULL, sizeof (struct SOAP_ENV__Header));
    
//      if (soap_call___ns1__getState (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, Response) == SOAP_OK)
  {
    
  
  
//    else
  {
    
    
  


file_size (char *filename) 
{
  
  
  
  
  



/////////////////////
//--> install
/////////////////////
void 
install (int argc, char **argv) 
{
  
  
  
  
  
  
  
  
  
  
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
  
    
    {
      
      
    
  
  
  
  
  
  
  
  
  
  
//TODO Win32
//      dpws_header_gen_request (&client, MsgId,
//                           wsa_eprlist_elem_get_Addrs (type),
//                           "http://www.ws4d.org/services/life-cycle-manager/installRequest",
//                           NULL, NULL, sizeof (struct SOAP_ENV__Header));
    
//      if (soap_call___ns1__install (&client, wsa_eprlist_elem_get_Addrs (type), NULL, &query, &Response) == SOAP_OK)
  {
    
      
      {
        
                 Response.status, Response.serviceID);
      
    
    else
      
      {
        
      
  
  
//    else
  {
    
    
  



///////////////////////
//--> uninstallService
///////////////////////
void 
uninstall (int argc, char **argv) 
{
  
  
  
  
  
  
  
  
  
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
  
  
  
  
    //query.serviceID     = argv[0];                
    
  
  
  
  
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
  
  
  
  
  
  
  
  
    // add attachement
    query.CRC = 0;
  
  
    //query.Param.__size = readfile("simpleExample_03.zip", query.Param.__ptr);
    //query.Param.__size = readfile("D:\\Workspace\\dpws-samples\\life-cycle-manager\\Build\\bin\\simpleExample_03.zip", query.Param.__ptr);
    query.Param.__size =
    readfile ("/home/schorsch/workspace/life-cycle-manager/se_update.zip",
              query.Param.__ptr);
  
    
    {
      
      
    
  
  
  
  
  
  
    //query.CRC = CRC( 0, query.Param.__ptr, query.Param.__size);
    
  
    // calling Methode
    soap_set_namespaces (&client, lcm_namespaces);
  
  
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
  
  
  
  
  
  
  
  
  
  
    
    {
      
               
      
      
    



//++++++++++++++++++
//--> exit function 
//++++++++++++++++++
  void
exit_lcm_client () 
{
  
  
  
  
  


//++++++++++++++++++++++++++++++++++
//--> process commandline parameter
//++++++++++++++++++++++++++++++++++
  int
process_command (char *cmdline) 
{
  
  
  
  
  
  
  
    
    {
      
      
      
    
  
  do
    
    {
      
      
        
        {
          
        
      
      else
        
        {
          
        
    
  
  
    
    {
      
        
        {
          
          
          
            
            {
              
            
          
        
    
  
    
    {
      
      
      
    
  
    
    {
      
      
      
    
  
    
    {
    
      
      
    
      
      
    
      
      
    
      
      
    
      
      
    
      
      
    
      
      
    
      
      
    
      
      
    
      
      
    
      
      
      
    
  



//++++++++++++++++++++++++++//
//------ main funktion -----//
//++++++++++++++++++++++++++//
  int
main (int argc, char **argv) 
{
  
  
  
  
  
    // parsing command line options 
    while ((argc > 1) && (argv[1][0] == '-'))
    
    {
      
      
        
        {
        
          
            
            {
              
+option;
              
            
          
          else
            
            {
              
              
+argv;
              
            
          
          
          
        
          
        
      
      
+argv;
    
  
    
    {
      
      
      
    
  
  
    // initialize client soap handle 
    soap_init (&client);
  
  
    // initialize stack 
    if (dpws_init (&dpws, host) != SOAP_OK)
    
    {
      
      
      
      
    
  
  
    // allocate and prepare service to resolve 
//TODO Win32
//  service = wsa_eprlist_alloc_elem (1, &alist);
//  wsa_eprlist_elem_set_Addrs (service, wsa_eprlist_elem_get_Addrs (service));
    
  
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
      
    
  
//TODO Win32  type = wsa_eprlist_alloc_elem (1, &alist);
    
  
    ("\"http://www.ws4d.org/services/life-cycle-manager\":life-cycle-manager",
     &service_list, &alist);
  
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
    
  
    // process comandline options
    while (1)
    
    {
      
      
      
    
  
  


