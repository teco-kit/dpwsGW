/* Copyright (C) 2007  University of Rostock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

//gsoapopt cw

/******************************************************************************\
 *                                                                            *
 * http://schemas.example.org/SimpleService                                   *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Import                                                                     *
 *                                                                            *
\******************************************************************************/

#import "devprof.gsoap"

/******************************************************************************\
 *                                                                            *
 * Schema Namespaces                                                          *
 *                                                                            *
\******************************************************************************/

/* NOTE:

It is strongly recommended to customize the names of the namespace prefixes
generated by wsdl2h. To do so, modify the prefix bindings below and add the
modified lines to typemap.dat to rerun wsdl2h:

sis1 = "http://schemas.example.org/SimpleService"

*/

//gsoap sis1  schema namespace: http://schemas.example.org/SimpleService
//gsoap sis1  schema elementForm:       qualified
//gsoap sis1  schema attributeForm:     unqualified

/******************************************************************************\
 *                                                                            *
 * Schema Types                                                               *
 *                                                                            *
\******************************************************************************/



/// "http://schemas.example.org/SimpleService":OneWayType is a complexType.
struct sis1__OneWayType
{
/// Element Param of type xs:int.
  int Param 1;                  ///< Required element.
/// TODO: <any namespace="##other" minOccurs="0" maxOccurs="unbounded">
///       Schema extensibility is user-definable.
///       Consult the protocol documentation to change or insert declarations.
///       Use wsdl2h option -x to remove this element.
///       Use wsdl2h option -d to use xsd__anyType DOM.
/// Size of the dynamic array of XML is 0..unbounded
  int __size;
  _XML *__any;                  ///< Catch any element content in XML string.
/// TODO: <anyAttribute namespace="##other">
///       Schema extensibility is user-definable.
///       Consult the protocol documentation to change or insert declarations.
///       Use wsdl2h option -x to remove this attribute.
   @_XML __anyAttribute;        ///< Catch any attribute content in XML string.
};

/// "http://schemas.example.org/SimpleService":TwoWayType is a complexType.
struct sis1__TwoWayType
{
/// Element X of type xs:int.
  int X 1;                      ///< Required element.
/// Element Y of type xs:int.
  int Y 1;                      ///< Required element.
/// TODO: <any namespace="##other" minOccurs="0" maxOccurs="unbounded">
///       Schema extensibility is user-definable.
///       Consult the protocol documentation to change or insert declarations.
///       Use wsdl2h option -x to remove this element.
///       Use wsdl2h option -d to use xsd__anyType DOM.
/// Size of the dynamic array of XML is 0..unbounded
  int __size;
  _XML *__any;                  ///< Catch any element content in XML string.
/// TODO: <anyAttribute namespace="##other">
///       Schema extensibility is user-definable.
///       Consult the protocol documentation to change or insert declarations.
///       Use wsdl2h option -x to remove this attribute.
   @_XML __anyAttribute;        ///< Catch any attribute content in XML string.
};

/// "http://schemas.example.org/SimpleService":TwoWayResponseType is a complexType.
struct sis1__TwoWayResponseType
{
/// Element Sum of type xs:int.
  int Sum 1;                    ///< Required element.
/// TODO: <any namespace="##other" minOccurs="0" maxOccurs="unbounded">
///       Schema extensibility is user-definable.
///       Consult the protocol documentation to change or insert declarations.
///       Use wsdl2h option -x to remove this element.
///       Use wsdl2h option -d to use xsd__anyType DOM.
/// Size of the dynamic array of XML is 0..unbounded
  int __size;
  _XML *__any;                  ///< Catch any element content in XML string.
/// TODO: <anyAttribute namespace="##other">
///       Schema extensibility is user-definable.
///       Consult the protocol documentation to change or insert declarations.
///       Use wsdl2h option -x to remove this attribute.
   @_XML __anyAttribute;        ///< Catch any attribute content in XML string.
};

/// Element "http://schemas.example.org/SimpleService":OneWay of type "http://schemas.example.org/SimpleService":OneWayType.
/// Note: use wsdl2h option -g to generate this global element declaration.

/// Element "http://schemas.example.org/SimpleService":TwoWayRequest of type "http://schemas.example.org/SimpleService":TwoWayType.
/// Note: use wsdl2h option -g to generate this global element declaration.

/// Element "http://schemas.example.org/SimpleService":TwoWayResponse of type "http://schemas.example.org/SimpleService":TwoWayResponseType.
/// Note: use wsdl2h option -g to generate this global element declaration.

/******************************************************************************\
 *                                                                            *
 * Services                                                                   *
 *                                                                            *
\******************************************************************************/


//gsoap sis1 service name:      SimpleServiceSoap12Binding
//gsoap sis1 service type:      SimpleService
//gsoap sis1 service port:      http://localhost/WebService/Simple.asmx
//gsoap sis1 service namespace: http://schemas.example.org/SimpleService
//gsoap sis1 service transport: http://schemas.xmlsoap.org/soap/http

/** @mainpage Service Definitions

@section Service_bindings Bindings
  - @ref SimpleServiceSoap12Binding

*/

/**

@page SimpleServiceSoap12Binding Binding "SimpleServiceSoap12Binding"

@section SimpleServiceSoap12Binding_operations Operations of Binding  "SimpleServiceSoap12Binding"
  - @ref __sis1__OneWay
  - @ref __sis1__TwoWay

@section SimpleServiceSoap12Binding_ports Endpoints of Binding  "SimpleServiceSoap12Binding"
  - http://localhost/WebService/Simple.asmx

Note: use wsdl2h option -N to change the service binding prefix name

*/

/******************************************************************************\
 *                                                                            *
 * SimpleServiceSoap12Binding                                                 *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * __sis1__OneWay                                                             *
 *                                                                            *
\******************************************************************************/


/// Operation "__sis1__OneWay" of service binding "SimpleServiceSoap12Binding"

/**

Operation details:

  - One-way message
  - SOAP document/literal style
  - SOAP action="http://schemas.example.org/SimpleService/OneWay"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___sis1__OneWay(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    struct sis1__OneWayType*            sis1__OneWay,
    // response parameters:
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __sis1__OneWay(
    struct soap *soap,
    // request parameters:
    struct sis1__OneWayType*            sis1__OneWay,
    // response parameters:
  );
@endcode

*/

//gsoap sis1 service method-style:      OneWay document
//gsoap sis1 service method-encoding:   OneWay literal
//gsoap sis1 service method-action:     OneWay http://schemas.example.org/SimpleService/OneWay
int __sis1__OneWay (struct sis1__OneWayType *sis1__OneWay,      ///< Request parameter
                    void        ///< One-way message: no response parameter
  );

/******************************************************************************\
 *                                                                            *
 * __sis1__TwoWay                                                             *
 *                                                                            *
\******************************************************************************/


/// Operation "__sis1__TwoWay" of service binding "SimpleServiceSoap12Binding"

/**

Operation details:

  - SOAP document/literal style
  - SOAP action="http://schemas.example.org/SimpleService/TwoWayRequest"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___sis1__TwoWay(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    struct sis1__TwoWayType*            sis1__TwoWayRequest,
    // response parameters:
    struct sis1__TwoWayResponseType*    sis1__TwoWayResponse
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __sis1__TwoWay(
    struct soap *soap,
    // request parameters:
    struct sis1__TwoWayType*            sis1__TwoWayRequest,
    // response parameters:
    struct sis1__TwoWayResponseType*    sis1__TwoWayResponse
  );
@endcode

*/

//gsoap sis1 service method-style:      TwoWay document
//gsoap sis1 service method-encoding:   TwoWay literal
//gsoap sis1 service method-action:     TwoWay http://schemas.example.org/SimpleService/TwoWayRequest
int __sis1__TwoWay (struct sis1__TwoWayType *sis1__TwoWayRequest,       ///< Request parameter
                    struct sis1__TwoWayResponseType *sis1__TwoWayResponse       ///< Response parameter
  );

/* End of SimpleService.h */
