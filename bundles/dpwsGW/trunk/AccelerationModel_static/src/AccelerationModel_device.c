#include <ws4d-gSOAP/dpwsH.h>
#include <ws4d-gSOAP/dpws_device.h>
#include <stdio.h>
#include <dpws_append_wsdl.h>

#include "Acceleration_operations.h"
#include "DeviceInfo_operations.h"
#include "DataLogging_operations.h"
#include <assert.h>
#define MODEL(X) AccelModel_##X
#include <device.h>

static struct Namespace namespaces[] = {

		{ "SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope",
				"http://www.w3.org/2002/06/soap-envelope" },

				{ "xsi", "http://www.w3.org/2001/XMLSchema-instance",
						"http://www.w3.org/*/XMLSchema-instance", NULL }, { "xsd",
								"http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema",
								NULL },

								{ "acs", "http://www.teco.edu/AccelerationService", NULL, NULL },
								{ "dvcinf", "http://www.teco.edu/DeviceInfoService", NULL, NULL },
								{ "logging", "http://www.teco.edu/DataLoggingService", NULL, NULL},

								{ NULL, NULL, NULL, NULL } };

int MODEL(init_service(struct soap *service)) {
	soap_init(service);
#ifdef DEBUG
	soap_omode (service, SOAP_XML_INDENT);
#endif
	soap_set_namespaces(service, namespaces);
	return WS4D_OK;
}

//typedef int (*serve_requests_ptr)(struct soap *);

extern int AccelerationService_serve_request(struct soap * soap);

extern int DeviceInfoService_serve_request(struct soap * soap);

extern int DataLoggingService_serve_request(struct soap * soap);

static device_serve_requests_ptr serve_requests[] = SOAP_SERVE_SET(
		AccelerationService_serve_request,DeviceInfoService_serve_request,DataLoggingService_serve_request);

ssize_t MODEL(get_serve_requests(device_serve_requests_ptr **s)) {
	ssize_t len=0;
	while (serve_requests[len])len++;
	assert(len<sizeof(serve_requests));
	*s=serve_requests;
	return len;
}

int MODEL(setup_hosting_service(struct dpws_s *device,
		struct soap *service, char *uuid)) {
	int ret = 0;

	ret = dpws_add_hosting_service(device, service, 0, "AccelerationDevice", uuid,
			100, 0);

	if (ret != WS4D_OK) {
		fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
				__LINE__);
		return ret;
	}

	{
		struct ws4d_qname *service_type = ws4d_qname_alloc(1,
				&device->alloc_list);
		service_type->ns = ws4d_strdup("http://www.teco.edu/AccelerationModel",
				&device->alloc_list);
		service_type->name
		= ws4d_strdup("AccelerationDeviceType", &device->alloc_list);
		if (0 != (ret = dpws_add_type(device, service_type))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}
	}

	return WS4D_OK;

}

int MODEL(setup_device(struct dpws_s *device, struct soap *service)) {

	{
		char uri[DPWS_URI_MAX_LEN] = "http://host:0/";
		int ret;

		strcat(uri, device->hosting_addr);
		strcat(uri, "0"); //TODO: every service gets an unique id!

		struct ws4d_epr *dpws_service = dpws_service_init(device,
				"AccelerationService");

		if (0 != (ret = dpws_service_bind(device, dpws_service, service, uri,
				DPWS_URI_MAX_LEN, 100))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

		{
			struct ws4d_qname *service_type = ws4d_qname_alloc(1,
					&device->alloc_list);
			service_type->ns = ws4d_strdup("http://www.teco.edu/AccelerationService",
					&device->alloc_list);
			service_type->name = ws4d_strdup("AccelerationService",
					&device->alloc_list);
			if (0 != (ret = dpws_service_add_type(dpws_service, service_type))) {
				fprintf(stderr, "%s:%d:Can't init device and services\n",
						__FILE__, __LINE__);
				return ret;
			}
		}

		if (0 != (ret = dpws_service_set_wsdl(dpws_service, "AccelerationService"))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

		if (0 != (ret = dpws_add_hosted_service(device, dpws_service, uri,
				DPWS_URI_MAX_LEN))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

	}

	{
		char uri[DPWS_URI_MAX_LEN] = "http://host:0/";
		int ret;

		strcat(uri, device->hosting_addr);
		strcat(uri, "1"); //TODO: every service gets an unique id!

		struct ws4d_epr *dpws_service = dpws_service_init(device,
				"DeviceInfoService");

		if (0 != (ret = dpws_service_bind(device, dpws_service, service, uri,
				DPWS_URI_MAX_LEN, 100))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

		{
			struct ws4d_qname *service_type = ws4d_qname_alloc(1,
					&device->alloc_list);
			service_type->ns = ws4d_strdup("http://www.teco.edu/DeviceInfoService",
					&device->alloc_list);
			service_type->name = ws4d_strdup("DeviceInfoService",
					&device->alloc_list);
			if (0 != (ret = dpws_service_add_type(dpws_service, service_type))) {
				fprintf(stderr, "%s:%d:Can't init device and services\n",
						__FILE__, __LINE__);
				return ret;
			}
		}

		if (0 != (ret = dpws_service_set_wsdl(dpws_service, "DeviceInfoService"))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

		if (0 != (ret = dpws_add_hosted_service(device, dpws_service, uri,
				DPWS_URI_MAX_LEN))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

	}

	{
		char uri[DPWS_URI_MAX_LEN] = "http://host:0/";
		int ret;

		strcat(uri, device->hosting_addr);
		strcat(uri, "2"); //TODO: every service gets an unique id!

		struct ws4d_epr *dpws_service = dpws_service_init(device,
				"DataLoggingService");

		if (0 != (ret = dpws_service_bind(device, dpws_service, service, uri,
				DPWS_URI_MAX_LEN, 100))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

		{
			struct ws4d_qname *service_type = ws4d_qname_alloc(1,
					&device->alloc_list);
			service_type->ns = ws4d_strdup("http://www.teco.edu/DataLoggingService",
					&device->alloc_list);
			service_type->name = ws4d_strdup("DataLoggingService",
					&device->alloc_list);
			if (0 != (ret = dpws_service_add_type(dpws_service, service_type))) {
				fprintf(stderr, "%s:%d:Can't init device and services\n",
						__FILE__, __LINE__);
				return ret;
			}
		}

		if (0 != (ret = dpws_service_set_wsdl(dpws_service, "DataLoggingService"))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

		if (0 != (ret = dpws_add_hosted_service(device, dpws_service, uri,
				DPWS_URI_MAX_LEN))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

	}

	return WS4D_OK;
}

int MODEL(set_metadata_device(struct dpws_s *device)) {
	/*set_metadata_ThisDevice*/
	struct ws4d_thisDevice *_device = dpws_change_thisdevice(device);
	static const ws4d_device_FriendlyName_var( friendlyName) = {
			ws4d_locstring_init_string("de", "Beschleunigungsdaten Service"),

			ws4d_locstring_init_string("en", "Acceleration data Service"), };

	ws4d_device_set_FriendlyName(_device, friendlyName, 2);
	ws4d_device_set_FirmwareVersion(_device, "Version 0.1");
	ws4d_device_set_SerialNumber(_device,
			"1234567f-c23f-4d91-84ba-5555555555556");
	return WS4D_OK;
}

int MODEL(set_metadata_model(struct dpws_s *device)) { /*set_metadata_ThisModel*/
	struct ws4d_thisModel *model = dpws_change_thismodel(device);
	static const ws4d_model_Name_var( name) = { ws4d_locstring_init_string(
			"de", "AccelerationDevice Sensorknoten"),

			ws4d_locstring_init_string("en", "AccelerationDevice Sensorknoten"), };
	static const ws4d_model_Manufacturer_var
	( manufacturer) =
	{ ws4d_locstring_init_string("de",
			"TECO (Universit�t Karlsruhe)"),

			ws4d_locstring_init_string("en",
					"TECO (University of Karlsruhe)"), };

	ws4d_model_set_Manufacturer(model, manufacturer, 2);
	ws4d_model_set_ManufacturerUrl(model, "http://www.teco.edu");
	ws4d_model_set_Name(model, name, 2);
	ws4d_model_set_Number(model, "1.0");
	ws4d_model_set_Url(model, "http://www.teco.edu");
	return WS4D_OK;
}

int MODEL(set_wsdl(struct dpws_s *device)) {

	dpws_append_wsdl(
			device,
			"AccelerationService",
			"<wsdl:definitions xmlns:wsoap12=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
			"	xmlns:tns=\"http://www.teco.edu/AccelerationService\" xmlns:wsdl=\"http://schemas.xmlsoap.org/wsdl/\"\n"
			"\n"
			"	xmlns:wsa_=\"http://www.w3.org/2006/05/addressing/wsdl\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"\n"
			"\n"
			"	xmlns:wse=\"http://schemas.xmlsoap.org/ws/2004/08/eventing\" xmlns:soap=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
			"\n"
			"	xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" name=\"AccelerationService\"\n"
			"	targetNamespace=\"http://www.teco.edu/AccelerationService\">\n"
			"\n"
			"	<wsdl:types>\n"
			"		<schema xmlns=\"http://www.w3.org/2001/XMLSchema\"\n"
			"			elementFormDefault=\"qualified\"\n"
			"			targetNamespace=\"http://www.teco.edu/AccelerationService\">	\n"
			"			<annotation>\n"
			"				<appinfo>\n"
			"					<UnitSet xmlns=\"urn:oasis:names:tc:unitsml:schema:xsd:UnitsMLSchema-0.9.18\">\n"
			"					<Unit\n"
			"									xmlns=\"urn:oasis:names:tc:unitsml:schema:xsd:UnitsMLSchema-0.9.18\"\n"
			"									xml:id=\"U_mm.s-2\">\n"
			"									<UnitSystem name=\"SI\"\n"
			"										type=\"SI_derived\" xml:lang=\"en-US\" />\n"
			"									<UnitName xml:lang=\"en-US\">\n"
			"										millimeter per second squared\n"
			"									</UnitName>\n"
			"									<UnitSymbol type=\"HTML\">\n"
			"										mm &#183; s\n"
			"										<sup>-2</sup>\n"
			"									</UnitSymbol>\n"
			"									<RootUnits>\n"
			"										<EnumeratedRootUnit unit=\"meter\"\n"
			"											prefix=\"m\" />\n"
			"										<EnumeratedRootUnit\n"
			"											unit=\"second\" powerNumerator=\"-2\" />\n"
			"									</RootUnits>\n"
			"								</Unit>\n"
			"					</UnitSet>\n"
			"				</appinfo>\n"
			"			</annotation>\n"
			"			<complexType name=\"ADXL210_Sample\">\n"
			"				<annotation>\n"
			"					<documentation>\n"
			"						Analog Devices ADXL210 Low-Cost ±10 g Dual-Axis\n"
			"						Accelerometer with Duty Cycle\n"
			"\n"
			"						(z-output from optional orthogonally mounted\n"
			"						second device)\n"
			"\n"
			"\n"
			"\n"
			"						http://www.analog.com/static/imported-files/data_sheets_obsolete/OBSOLETE%20WATERMARK/ADXL210.pdf\n"
			"					</documentation>\n"
			"				</annotation>\n"
			"				<sequence>\n"
			"					<element name=\"x\" type=\"float\">\n"
			"					</element>\n"
			"					<element name=\"y\" type=\"float\">\n"
			"					</element>\n"
			"					<element name=\"z\" type=\"float\">\n"
			"					</element>\n"
			"				</sequence>\n"
			"			</complexType>\n"
			"\n"
			"			<complexType name=\"AccelerationSample\">\n"
			"				<annotation>\n"
			"					<documentation>\n"
			"						Acceleration Sample\n"
			"					</documentation>\n"
			"				</annotation>\n"
			"\n"
			"				<sequence>\n"
			"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
			"						name=\"delta\" type=\"duration\" />\n"
			"					<element maxOccurs=\"1\" minOccurs=\"1\"\n"
			"						name=\"accl\" type=\"tns:ADXL210_Sample\">\n"
			"					</element>\n"
			"				</sequence>\n"
			"\n"
			"			</complexType>\n"
			"\n"
			"				<complexType name=\"AccelerationSeries\">\n"
			"                    <annotation>\n"
			"                        <documentation>\n"
			"                            Series of measurements\n"
			"                        </documentation>\n"
			"                    </annotation>\n"
			"                    <sequence>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"count\" type=\"nonNegativeInteger\"/>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"0\" name=\"timestamp\" type=\"dateTime\"/>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"delta\" type=\"duration\" />\n"
			"                        <element maxOccurs=\"unbounded\" minOccurs=\"1\" name=\"sample\" type=\"tns:AccelerationSample\"/>\n"
			"                   </sequence>\n"
			"                </complexType>\n"
			"\n"
			"				<complexType name=\"LDCInfo\">\n"
			"					<sequence>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"rate\" >\n"
			"							<simpleType>\n"
			"								<restriction base=\"string\"> \n"
			"									<enumeration value=\"500\"/>"
			"									<enumeration value=\"250\"/>"
			"									<enumeration value=\"100\"/>"
			"									<enumeration value=\"50\"/>"
			"									<enumeration value=\"25\"/>"
			"									<enumeration value=\"10\"/>"
			"									<enumeration value=\"5\"/>"
			"									<enumeration value=\"2\"/>"
			"									<enumeration value=\"1\"/>"
			"									<enumeration value=\"1/2\"/>"
			"									<enumeration value=\"1/5\"/>"
			"									<enumeration value=\"1/10\"/>"
			"									<enumeration value=\"1/30\"/>"
			"									<enumeration value=\"1/60\"/>"
			"									<enumeration value=\"1/120\"/>"
			"									<enumeration value=\"1/300\"/>"
			"									<enumeration value=\"1/600\"/>"
			"									<enumeration value=\"1/1800\"/>"
			"									<enumeration value=\"1/3600\"/>"
			"								</restriction>\n"
			"							</simpleType>\n"
			"						</element>\n"
			"						<element maxOccurs=\"1\" minOccurs=\"1\" name=\"duration\" type=\"nonNegativeInteger\"/>\n"
			"					</sequence>\n"
			"				</complexType>\n"
			"\n"
			"			<element name=\"series\" type=\"tns:AccelerationSeries\" />\n"
			"			<element name=\"ldcinfo\" type=\"tns:LDCInfo\" />\n"
			"\n"
			"		</schema>\n"
			"	</wsdl:types>\n"
			"\n"
			"	<wsdl:message name=\"AccelerationServiceEventMessageOut\">\n"
			"		<wsdl:part name=\"event\" element=\"tns:series\" />\n"
			"	</wsdl:message>\n"
			"\n"
			"	<wsdl:message name=\"StartLDCMessageIn\" >\n"
			"		<wsdl:part name=\"in\" element=\"tns:ldcinfo\"></wsdl:part>\n"
			"	</wsdl:message>\n"
			"	<wsdl:message name=\"StartLDCMessageOut\" />\n"
			"\n"
			"	<wsdl:portType name=\"AccelerationService\" wse:EventSource=\"true\">\n"
			"		<wsdl:operation name=\"AccelerationServiceEvent\">\n"
			"			<wsdl:output message=\"tns:AccelerationServiceEventMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/AccelerationService/AccelerationServiceEventOut\" />\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"StartLDC\">\n"
			"			<wsdl:input message=\"tns:StartLDCMessageIn\"\n"
			"				wsa:Action=\"http://www.teco.edu/AccelerationService/StartLDCIn\" />\n"
			"			<wsdl:output message=\"tns:StartLDCMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/AccelerationService/StartLDCOut\" />\n"
			"		</wsdl:operation>\n"
			"	</wsdl:portType>\n"
			"\n"
			"	<wsdl:binding name=\"AccelerationServiceSOAP\" type=\"tns:AccelerationService\">\n"
			"		<soap:binding style=\"document\"\n"
			"			transport=\"http://schemas.xmlsoap.org/soap/http\" />\n"
			"		<wsdl:operation name=\"AccelerationServiceEvent\">\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"StartLDC\">\n"
			"			<wsdl:input>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:input>\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"  </wsdl:binding>\n"
			"\n"
			"	<wsdl:service name=\"AccelerationService\">\n"
			"		<wsdl:port name=\"AccelerationService\" binding=\"tns:AccelerationServiceSOAP\">\n"
			"			<wsoap12:address location=\"http://localhost:8080\" />\n"
			"		</wsdl:port>\n"
			"	</wsdl:service>\n"
			"</wsdl:definitions>\n");


	dpws_append_wsdl(
			device,
			"DeviceInfoService",
			"<wsdl:definitions xmlns:wsoap12=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
			"	xmlns:dis=\"http://www.teco.edu/DeviceInfoService\" xmlns:wsdl=\"http://schemas.xmlsoap.org/wsdl/\"\n"
			"\n"
			"	xmlns:wsa_=\"http://www.w3.org/2006/05/addressing/wsdl\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"\n"
			"\n"
			"	xmlns:wse=\"http://schemas.xmlsoap.org/ws/2004/08/eventing\" xmlns:soap=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
			"\n"
			"	xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" name=\"DeviceInfoService\"\n"
			"	targetNamespace=\"http://www.teco.edu/DeviceInfoService\">\n"
			"\n"
			"	<wsdl:types>\n"
			"		<schema xmlns=\"http://www.w3.org/2001/XMLSchema\"\n"
			"			elementFormDefault=\"qualified\"\n"
			"			targetNamespace=\"http://www.teco.edu/DeviceInfoService\">	\n"
			"			<complexType name=\"StatusMessage\">"
			"                    <annotation>\n"
			"                        <documentation>\n"
			"                            Node status\n"
			"                        </documentation>\n"
			"                    </annotation>\n"
			"                    <sequence>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"description\" type=\"string\"/>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"ready\" type=\"boolean\"/>\n"
			"                   </sequence>\n"
			"           </complexType>\n"
			"\n"
			"			<element name=\"status\" type=\"dis:StatusMessage\" />\n"
			"\n"
			"		</schema>\n"
			"	</wsdl:types>\n"
			"\n"
			"	<wsdl:message name=\"StopNodeMessageIn\" />\n"
			"	<wsdl:message name=\"StopNodeMessageOut\" />\n"
			"\n"
			"	<wsdl:message name=\"GetNodeStatusMessageIn\" />\n"
			"	<wsdl:message name=\"GetNodeStatusMessageOut\">\n"
			"		<wsdl:part name=\"out\" element=\"dis:status\" />\n"
			"	</wsdl:message>\n"
			"\n"
			"	<wsdl:portType name=\"DeviceInfoService\" >\n"
			"		<wsdl:operation name=\"StopNode\">\n"
			"			<wsdl:input message=\"dis:StopNodeMessageIn\"\n"
			"				wsa:Action=\"http://www.teco.edu/DeviceInfoService/StopNodeIn\" />\n"
			"			<wsdl:output message=\"dis:StopNodeMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/DeviceInfoService/StopNodeOut\" />\n"
			"		</wsdl:operation>\n"
			"\n"
			"       <wsdl:operation name=\"GetNodeStatus\">\n"
			"			<wsdl:input message=\"dis:GetNodeStatusMessageIn\"\n"
			"				wsa:Action=\"http://www.teco.edu/DeviceInfoService/GetNodeStatusIn\" />\n"
			"			<wsdl:output message=\"dis:GetNodeStatusMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/DeviceInfoService/GetNodeStatusOut\" />\n"
			"		</wsdl:operation>\n"
			"	</wsdl:portType>\n"
			"\n"
			"	<wsdl:binding name=\"DeviceInfoServiceSOAP\" type=\"dis:DeviceInfoService\">\n"
			"		<soap:binding style=\"document\"\n"
			"			transport=\"http://schemas.xmlsoap.org/soap/http\" />\n"
			"		<wsdl:operation name=\"GetNodeStatus\">\n"
			"			<wsdl:input>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:input>\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"StopNode\">\n"
			"			<wsdl:input>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:input>\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"  </wsdl:binding>\n"
			"\n"
			"	<wsdl:service name=\"DeviceInfoService\">\n"
			"		<wsdl:port name=\"DeviceInfoService\" binding=\"dis:DeviceInfoServiceSOAP\">\n"
			"			<wsoap12:address location=\"http://localhost:8080\" />\n"
			"		</wsdl:port>\n"
			"	</wsdl:service>\n"
			"</wsdl:definitions>\n");

	dpws_append_wsdl(
			device,
			"DataLoggingService",
			"<wsdl:definitions xmlns:wsoap12=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
			"	xmlns:log=\"http://www.teco.edu/DataLoggingService\" xmlns:wsdl=\"http://schemas.xmlsoap.org/wsdl/\"\n"
			"\n"
			"	xmlns:wsa_=\"http://www.w3.org/2006/05/addressing/wsdl\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"\n"
			"\n"
			"	xmlns:wse=\"http://schemas.xmlsoap.org/ws/2004/08/eventing\" xmlns:soap=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
			"\n"
			"	xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" name=\"DataLoggingService\"\n"
			"	targetNamespace=\"http://www.teco.edu/DataLoggingService\">\n"
			"\n"
			"	<wsdl:types>\n"
			"		<schema xmlns=\"http://www.w3.org/2001/XMLSchema\"\n"
			"			elementFormDefault=\"qualified\"\n"
			"			targetNamespace=\"http://www.teco.edu/DataLoggingService\">	\n"
			"			<annotation>\n"
			"				<appinfo>\n"
			"					<UnitSet xmlns=\"urn:oasis:names:tc:unitsml:schema:xsd:UnitsMLSchema-0.9.18\">\n"
			"					<Unit\n"
			"									xmlns=\"urn:oasis:names:tc:unitsml:schema:xsd:UnitsMLSchema-0.9.18\"\n"
			"									xml:id=\"U_mm.s-2\">\n"
			"									<UnitSystem name=\"SI\"\n"
			"										type=\"SI_derived\" xml:lang=\"en-US\" />\n"
			"									<UnitName xml:lang=\"en-US\">\n"
			"										millimeter per second squared\n"
			"									</UnitName>\n"
			"									<UnitSymbol type=\"HTML\">\n"
			"										mm &#183; s\n"
			"										<sup>-2</sup>\n"
			"									</UnitSymbol>\n"
			"									<RootUnits>\n"
			"										<EnumeratedRootUnit unit=\"meter\"\n"
			"											prefix=\"m\" />\n"
			"										<EnumeratedRootUnit\n"
			"											unit=\"second\" powerNumerator=\"-2\" />\n"
			"									</RootUnits>\n"
			"								</Unit>\n"
			"					</UnitSet>\n"
			"				</appinfo>\n"
			"			</annotation>\n"
			"			<complexType name=\"ADXL210_Sample\">\n"
			"				<annotation>\n"
			"					<documentation>\n"
			"						Analog Devices ADXL210 Low-Cost ±10 g Dual-Axis\n"
			"						Accelerometer with Duty Cycle\n"
			"\n"
			"						(z-output from optional orthogonally mounted\n"
			"						second device)\n"
			"\n"
			"\n"
			"\n"
			"						http://www.analog.com/static/imported-files/data_sheets_obsolete/OBSOLETE%20WATERMARK/ADXL210.pdf\n"
			"					</documentation>\n"
			"				</annotation>\n"
			"				<sequence>\n"
			"					<element name=\"x\" type=\"float\">\n"
			"					</element>\n"
			"					<element name=\"y\" type=\"float\">\n"
			"					</element>\n"
			"					<element name=\"z\" type=\"float\">\n"
			"					</element>\n"
			"				</sequence>\n"
			"			</complexType>\n"
			"\n"
			"			<complexType name=\"AccelerationSample\">\n"
			"				<annotation>\n"
			"					<documentation>\n"
			"						Acceleration Sample\n"
			"					</documentation>\n"
			"				</annotation>\n"
			"\n"
			"				<sequence>\n"
			"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
			"						name=\"delta\" type=\"duration\" />\n"
			"					<element maxOccurs=\"1\" minOccurs=\"1\"\n"
			"						name=\"accl\" type=\"log:ADXL210_Sample\">\n"
			"					</element>\n"
			"				</sequence>\n"
			"\n"
			"			</complexType>\n"
			"\n"
			"				<complexType name=\"AccelerationSeries\">\n"
			"                    <annotation>\n"
			"                        <documentation>\n"
			"                            Series of measurements\n"
			"                        </documentation>\n"
			"                    </annotation>\n"
			"                    <sequence>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"count\" type=\"nonNegativeInteger\"/>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"0\" name=\"timestamp\" type=\"dateTime\"/>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"delta\" type=\"duration\" />\n"
			"                        <element maxOccurs=\"unbounded\" minOccurs=\"1\" name=\"sample\" type=\"log:AccelerationSample\"/>\n"
			"                   </sequence>\n"
			"                </complexType>\n"
			"\n"
			"				<complexType name=\"LoggingConfig\">\n"
			"					<sequence>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"rate\" >\n"
			"							<simpleType>\n"
			"								<restriction base=\"nonNegativeInteger\"> \n"
			"									<enumeration value=\"2048\"/>"
			"									<enumeration value=\"1024\"/>"
			"									<enumeration value=\"512\"/>"
			"									<enumeration value=\"256\"/>"
			"									<enumeration value=\"128\"/>"
			"									<enumeration value=\"64\"/>"
			"									<enumeration value=\"32\"/>"
			"								</restriction>\n"
			"							</simpleType>\n"
			"						</element>\n"
			"						<element maxOccurs=\"1\" minOccurs=\"1\" name=\"duration\" type=\"nonNegativeInteger\"/>\n"
			"					</sequence>\n"
			"				</complexType>\n"
			"\n"
			"				<complexType name=\"SessionInfo\">\n"
			"					<sequence>\n"
			"                        <element maxOccurs=\"1\" minOccurs=\"1\" name=\"count\" type=\"nonNegativeInteger\"/>\n"
			"					</sequence>\n"
			"				</complexType>\n"
			"\n"
			"			<element name=\"series\" type=\"log:AccelerationSeries\" />\n"
			"			<element name=\"loggingconfig\" type=\"log:LoggingConfig\" />\n"
			"			<element name=\"sessioninfo\" type=\"log:SessionInfo\" />\n"
			"\n"
			"		</schema>\n"
			"	</wsdl:types>\n"
			"\n"
			"	<wsdl:message name=\"DataLoggingServiceEventMessageOut\">\n"
			"		<wsdl:part name=\"event\" element=\"log:series\" />\n"
			"	</wsdl:message>\n"
			"\n"
			"	<wsdl:message name=\"StartLoggingMessageIn\" >\n"
			"		<wsdl:part name=\"in\" element=\"log:loggingconfig\"></wsdl:part>\n"
			"	</wsdl:message>\n"
			"	<wsdl:message name=\"StartLoggingMessageOut\" />\n"
			"\n"
			"	<wsdl:message name=\"StartDownloadMessageIn\" />\n"
			"	<wsdl:message name=\"StartDownloadMessageOut\" />\n"
			"\n"
			"	<wsdl:message name=\"EraseMessageIn\" />\n"
			"	<wsdl:message name=\"EraseMessageOut\" />\n"
			"\n"
			"	<wsdl:message name=\"GetSessionCountMessageIn\" />\n"
			"	<wsdl:message name=\"GetSessionCountMessageOut\" >\n"
			"		<wsdl:part name=\"out\" element=\"log:sessioninfo\"></wsdl:part>\n"
			"	</wsdl:message>\n"
			"\n"
			"	<wsdl:portType name=\"DataLoggingService\" wse:EventSource=\"true\">\n"
			"		<wsdl:operation name=\"DataLoggingServiceEvent\">\n"
			"			<wsdl:output message=\"log:DataLoggingServiceEventMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/DataLoggingServiceEventOut\" />\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"StartLogging\">\n"
			"			<wsdl:input message=\"log:StartLoggingMessageIn\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/StartLoggingIn\" />\n"
			"			<wsdl:output message=\"log:StartLoggingMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/StartLoggingOut\" />\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"StartDownload\">\n"
			"			<wsdl:input message=\"log:StartDownloadMessageIn\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/StartDownloadIn\" />\n"
			"			<wsdl:output message=\"log:StartDownloadMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/StartDownloadOut\" />\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"Erase\">\n"
			"			<wsdl:input message=\"log:EraseMessageIn\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/EraseIn\" />\n"
			"			<wsdl:output message=\"log:EraseMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/EraseOut\" />\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"GetSessionCount\">\n"
			"			<wsdl:input message=\"log:GetSessionCountMessageIn\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/GetSessionCountIn\" />\n"
			"			<wsdl:output message=\"log:GetSessionCountMessageOut\"\n"
			"				wsa:Action=\"http://www.teco.edu/DataLoggingService/GetSessionCountOut\" />\n"
			"		</wsdl:operation>\n"
			"	</wsdl:portType>\n"
			"\n"
			"	<wsdl:binding name=\"DataLoggingServiceSOAP\" type=\"log:DataLoggingService\">\n"
			"		<soap:binding style=\"document\"\n"
			"			transport=\"http://schemas.xmlsoap.org/soap/http\" />\n"
			"		<wsdl:operation name=\"DataLoggingServiceEvent\">\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"StartDownload\">\n"
			"			<wsdl:input>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:input>\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"StartLogging\">\n"
			"			<wsdl:input>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:input>\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"		<wsdl:operation name=\"Erase\">\n"
			"			<wsdl:input>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:input>\n"
			"			<wsdl:output>\n"
			"				<soap:body use=\"literal\" />\n"
			"			</wsdl:output>\n"
			"		</wsdl:operation>\n"
			"  </wsdl:binding>\n"
			"\n"
			"	<wsdl:service name=\"DataLoggingService\">\n"
			"		<wsdl:port name=\"DataLoggingService\" binding=\"log:DataLoggingServiceSOAP\">\n"
			"			<wsoap12:address location=\"http://localhost:8080\" />\n"
			"		</wsdl:port>\n"
			"	</wsdl:service>\n"
			"</wsdl:definitions>\n");

	return WS4D_OK;
}

void MODEL(event(int svc, int op, void* dev, char *buf, size_t len)) {
	printf("Calling MODEL(event)\n");
	switch (svc) {

	case SRV_Streaming: {
		extern void AccelerationService_event(int, void *, char *, size_t);
		AccelerationService_event(op, dev, buf, len);
		break;
	}

	case SRV_DeviceInfo: {
		extern void DeviceInfoService_event(int, void *, char *, size_t);
		DeviceInfoService_event(op, dev, buf, len);
		break;

	}

	case SRV_DataLogging: {
		extern void DataLoggingService_event(int, void *, char *, size_t);
		DataLoggingService_event(op, dev, buf, len);
		break;
	}
	}
}

