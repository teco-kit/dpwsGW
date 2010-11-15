#include <ws4d-gSOAP/stddpws.h>
#include <stdio.h>

static int dpws_append_wsdl(struct dpws_s *device, char *name, char *content)
{
int old = 0, index = 0;
	struct wsdl_s *wsdls = NULL;
	if(device->wsdls)
	{
		for(old = 0; device->wsdls[old].name != NULL; old++);
	}// get_length from null-terminated list

	wsdls = ws4d_malloc_alist((1 + old + 1) * sizeof(struct wsdl_s), &device->alloc_list);
	if(device->wsdls)
	{
		for(index = 0; index < old; index ++)
		{
			wsdls[index].name = device->wsdls[index].name;
			wsdls[index].content = device->wsdls[index].content;
		}
	}//memcpy

	{
	wsdls[index].name = name;
	wsdls[index].content = content;
	index++;

	wsdls[index].name = NULL; //terminate
	wsdls[index].content = NULL; //terminate
	}//append on copy

	{
	if (device->wsdls)
	{
		ws4d_free_alist(device->wsdls);
	}
	device->wsdls = wsdls;
	}//exchange

	return SOAP_OK;
}
