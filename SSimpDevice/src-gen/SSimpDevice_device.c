/* Generated file */

#include <ws4d-gSOAP/dpws_device.h>
#include <stdio.h>
#include <dpws_append_wsdl.h>
#define MODEL(X) SSimpDevice_##X
#include <device.h>
#include <assert.h>

static struct Namespace namespaces[] = {

{ "SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope",
		"http://www.w3.org/2002/06/soap-envelope" },

{ "xsi", "http://www.w3.org/2001/XMLSchema-instance",
		"http://www.w3.org/*/XMLSchema-instance", NULL }, { "xsd",
		"http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema",
		NULL },

{ "sens", "http://www.teco.edu/SensorValues", NULL, NULL },

{ NULL, NULL, NULL, NULL } };

int SSimpDevice_init_service(struct soap *service) {
	soap_init(service);
#ifdef DEBUG
	soap_omode (service, SOAP_XML_INDENT);
#endif
	soap_set_namespaces(service, namespaces);
	return WS4D_OK;
}

typedef int (*serve_requests_ptr)(struct soap *);

extern int SensorValues_serve_request(struct soap * soap);

static serve_requests_ptr serve_requests[] = SOAP_SERVE_SET(
		SensorValues_serve_request);

ssize_t MODEL(get_serve_requests(device_serve_requests_ptr **s)) {
	ssize_t len=0;
	while (serve_requests[len])len++;
	assert(len<sizeof(serve_requests));
	*s=serve_requests;
	return len;
}

int SSimpDevice_setup_hosting_service(struct dpws_s *device,
		struct soap *service, char *uuid) {
	int ret = 0;

	ret = dpws_add_hosting_service(device, service, 0, "SSimpDevice", uuid,
			100, 0);

	if (ret != WS4D_OK) {
		fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
				__LINE__);
		return ret;
	}

	{
		struct ws4d_qname *service_type = ws4d_qname_alloc(1,
				&device->alloc_list);
		service_type->ns = ws4d_strdup("http://www.teco.edu/SensorValues",
				&device->alloc_list);
		service_type->name
				= ws4d_strdup("SSimpDeviceType", &device->alloc_list);
		if (0 != (ret = dpws_add_type(device, service_type))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}
	}

	return WS4D_OK;

}

int SSimpDevice_setup_device(struct dpws_s *device, struct soap *service) {

	{
		char uri[DPWS_URI_MAX_LEN] = "http://host:0/";
		int ret;

		strcat(uri, device->hosting_addr);
		strcat(uri, "-1"); //TODO: every service gets an unique id!

		struct ws4d_epr *dpws_service = dpws_service_init(device,
				"SensorValues");

		if (0 != (ret = dpws_service_bind(device, dpws_service, service, uri,
				DPWS_URI_MAX_LEN, 100))) {
			fprintf(stderr, "%s:%d:Can't init device and services\n", __FILE__,
					__LINE__);
			return ret;
		}

		{
			struct ws4d_qname *service_type = ws4d_qname_alloc(1,
					&device->alloc_list);
			service_type->ns = ws4d_strdup("http://www.teco.edu/SensorValues",
					&device->alloc_list);
			service_type->name = ws4d_strdup("SensorValues",
					&device->alloc_list);
			if (0 != (ret = dpws_service_add_type(dpws_service, service_type))) {
				fprintf(stderr, "%s:%d:Can't init device and services\n",
						__FILE__, __LINE__);
				return ret;
			}
		}

		if (0 != (ret = dpws_service_set_wsdl(dpws_service, "SensorValues"))) {
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

int SSimpDevice_set_metadata_device(struct dpws_s *device) {
	/*set_metadata_ThisDevice*/
	struct ws4d_thisDevice *_device = dpws_change_thisdevice(device);
	static const ws4d_device_FriendlyName_var( friendlyName) = {
			ws4d_locstring_init_string("de", "Sensorüberwachungsdienst"),

			ws4d_locstring_init_string("en", "Sensor Monitoring Service"), };

	ws4d_device_set_FriendlyName(_device, friendlyName, 2);
	ws4d_device_set_FirmwareVersion(_device, "Version 0.1");
	ws4d_device_set_SerialNumber(_device,
			"1234567f-c23f-4d91-84ba-555555555555");
	return WS4D_OK;
}

int SSimpDevice_set_metadata_model(struct dpws_s *device) { /*set_metadata_ThisModel*/
	struct ws4d_thisModel *model = dpws_change_thismodel(device);
	static const ws4d_model_Name_var( name) = { ws4d_locstring_init_string(
			"de", "SSimp Sensorknoten"),

	ws4d_locstring_init_string("en", "SSimp Sensorknoten"), };
	static const ws4d_model_Manufacturer_var
			( manufacturer) =
					{ ws4d_locstring_init_string("de",
							"TECO (Universität Karlsruhe)"),

					ws4d_locstring_init_string("en",
							"TECO (University of Karlsruhe)"), };

	ws4d_model_set_Manufacturer(model, manufacturer, 2);
	ws4d_model_set_ManufacturerUrl(model, "http://www.teco.edu");
	ws4d_model_set_Name(model, name, 2);
	ws4d_model_set_Number(model, "1.0");
	ws4d_model_set_Url(model, "http://www.teco.edu");
	return WS4D_OK;
}

int SSimpDevice_set_wsdl(struct dpws_s *device) {

	dpws_append_wsdl(
			device,
			"SensorValues",
			"<wsdl:definitions xmlns:wsoap12=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
				"	xmlns:tns=\"http://www.teco.edu/SensorValues\" xmlns:wsdl=\"http://schemas.xmlsoap.org/wsdl/\"\n"
				"\n"
				"	xmlns:wsa_=\"http://www.w3.org/2006/05/addressing/wsdl\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\"\n"
				"\n"
				"	xmlns:wse=\"http://schemas.xmlsoap.org/ws/2004/08/eventing\" xmlns:soap=\"http://schemas.xmlsoap.org/wsdl/soap12/\"\n"
				"\n"
				"	xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" name=\"SensorValues\"\n"
				"	targetNamespace=\"http://www.teco.edu/SensorValues\">\n"
				"\n"
				"	<wsdl:types>\n"
				"		<schema xmlns=\"http://www.w3.org/2001/XMLSchema\"\n"
				"			elementFormDefault=\"qualified\"\n"
				"			targetNamespace=\"http://www.teco.edu/SensorValues\">	\n"
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
				"						Analog Devices ADXL210 Low-Cost Â±10 g Dual-Axis\n"
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
				"					<element name=\"x\">\n"
				"						<annotation>\n"
				"							<appinfo>\n"
				"								<Quantity\n"
				"									xmlns=\"urn:oasis:names:tc:unitsml:schema:xsd:UnitsMLSchema-0.9.18\">\n"
				"									<QuantityName>acceleration</QuantityName>\n"
				"									<UnitReference>#U_mm.s-2</UnitReference>\n"
				"								</Quantity>\n"
				"							</appinfo>\n"
				"						</annotation>\n"
				"						<simpleType>\n"
				"							<restriction base=\"short\">\n"
				"								<maxInclusive value=\"+2000\"></maxInclusive>\n"
				"								<minInclusive value=\"-2000\"></minInclusive>\n"
				"							</restriction>\n"
				"						</simpleType>\n"
				"					</element>\n"
				"					<element name=\"y\">\n"
				"						<simpleType>\n"
				"							<restriction base=\"short\">\n"
				"								<minInclusive value=\"-2000\"></minInclusive>\n"
				"								<maxInclusive value=\"+2000\"></maxInclusive>\n"
				"							</restriction>\n"
				"						</simpleType>\n"
				"					</element>\n"
				"					<element name=\"z\">\n"
				"						<simpleType>\n"
				"							<restriction base=\"short\">\n"
				"								<maxInclusive value=\"+2000\"></maxInclusive>\n"
				"								<minInclusive value=\"-2000\"></minInclusive>\n"
				"							</restriction>\n"
				"						</simpleType>\n"
				"					</element>\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"index\"\n"
				"						type=\"byte\" />\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"			<complexType name=\"TSL2550_Sample\">\n"
				"				<annotation>\n"
				"					<documentation>\n"
				"						Taos TSL2550 Ambient Light Sensor\n"
				"\n"
				"						The TSL2550 is a digital-output light sensor\n"
				"						with a two-wire, SMBus serial interface. It\n"
				"						combines two photodiodes and a companding\n"
				"						analog-to-digital converter (ADC) on a single\n"
				"						CMOS integrated circuit to provide light\n"
				"						measurements over an effective 12-bit dynamic\n"
				"						range with a response similar to that of the\n"
				"						human eye. The TSL2550 is designed for use with\n"
				"						broad wavelength light sources. One of the\n"
				"						photodiodes (channel 0) is sensitive to visible\n"
				"						and infrared light, while the second photodiode\n"
				"						(channel 1) is sensitive primarily to infrared\n"
				"						light.\n"
				"\n"
				"\n"
				"\n"
				"						http://www.taosinc.com/getfile.aspx?type=press&amp;file=tsl2550-e75.pdf\n"
				"						Light Level (lux)\n"
				"						=(daylight-infrared)*0.39*e(0.181R^2 )\n"
				"\n"
				"						Typical outputs:\n"
				"\n"
				"						Fluorescent (297 lux) : daylight=831 infrared=68\n"
				"						Daylight, shade (201 lux): daylight=895\n"
				"						infrared=343 Incadescent (42 lux): daylight 42\n"
				"						infrared=959\n"
				"					</documentation>\n"
				"				</annotation>\n"
				"				<sequence>\n"
				"					<element name=\"daylight\" type=\"unsignedShort\">\n"
				"						<annotation>\n"
				"							<documentation>\n"
				"								ADC ouput for channel 0\n"
				"							</documentation>\n"
				"						</annotation>\n"
				"					</element>\n"
				"					<element name=\"infrared\" type=\"unsignedShort\">\n"
				"						<annotation>\n"
				"							<documentation>\n"
				"								ADC ouput for channel 1\n"
				"							</documentation>\n"
				"						</annotation>\n"
				"					</element>\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"index\"\n"
				"						type=\"byte\" />\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"			<complexType name=\"FSR152_Sample\">\n"
				"				<sequence>\n"
				"					<element name=\"value\" type=\"unsignedByte\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"index\"\n"
				"						type=\"byte\" />\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"			<complexType name=\"SP101_Sample\">\n"
				"				<sequence>\n"
				"					<element name=\"volume\" type=\"unsignedByte\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"index\"\n"
				"						type=\"byte\" />\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"			<complexType name=\"TC74_Sample\">\n"
				"				<annotation>\n"
				"					<documentation>\n"
				"						Microchip TC74 Tiny Serial Digital Thermal\n"
				"						Sensor\n"
				"\n"
				"\n"
				"\n"
				"						http://ww1.microchip.com/downloads/en/devicedoc/21462c.pdf\n"
				"					</documentation>\n"
				"				</annotation>\n"
				"				<sequence>\n"
				"					<element name=\"value\">\n"
				"						<simpleType>\n"
				"							<restriction base=\"byte\">\n"
				"								<minInclusive value=\"-20\"></minInclusive>\n"
				"								<maxInclusive value=\"80\"></maxInclusive>\n"
				"							</restriction>\n"
				"						</simpleType>\n"
				"					</element>\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"index\"\n"
				"						type=\"byte\" />\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"			<complexType name=\"SSimpSample\">\n"
				"				<annotation>\n"
				"					<documentation>\n"
				"						Single Sample with sensors enabled according to\n"
				"						configuration\n"
				"					</documentation>\n"
				"				</annotation>\n"
				"\n"
				"				<sequence>\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
				"						name=\"TimeStamp\" type=\"dateTime\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
				"						name=\"Accelleration\" type=\"tns:ADXL210_Sample\">\n"
				"					</element>\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"Audio\"\n"
				"						type=\"tns:SP101_Sample\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"Light\"\n"
				"						type=\"tns:TSL2550_Sample\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"Force\"\n"
				"						type=\"tns:FSR152_Sample\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
				"						name=\"Temperature\" type=\"tns:TC74_Sample\" />\n"
				"				</sequence>\n"
				"\n"
				"			</complexType>\n"
				"\n"
				"			<element name=\"Sample\" type=\"tns:SSimpSample\" />\n"
				"\n"
				"\n"
				"			<complexType name=\"SSimpRateConfig\">\n"
				"				<annotation>\n"
				"					<documentation></documentation>\n"
				"				</annotation>\n"
				"				<sequence>\n"
				"					<element maxOccurs=\"1\" minOccurs=\"1\" name=\"rate\"\n"
				"						type=\"byte\">\n"
				"						<annotation>\n"
				"							<documentation>\n"
				"								sample period= 2^rate * 13 ms\n"
				"							</documentation>\n"
				"						</annotation>\n"
				"					</element>\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"\n"
				"			<complexType name=\"SensorConfigurationType\">\n"
				"				<annotation>\n"
				"					<documentation>\n"
				"						If a config element is present the sensor will\n"
				"						be enabled, otherwise it will be disable.\n"
				"					</documentation>\n"
				"				</annotation>\n"
				"				<sequence>\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
				"						name=\"Acceleration\" type=\"tns:SSimpRateConfig\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"Audio\"\n"
				"						type=\"tns:SSimpRateConfig\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"Light\"\n"
				"						type=\"tns:SSimpRateConfig\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
				"						name=\"AmbientLight\" type=\"tns:SSimpRateConfig\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\" name=\"Force\"\n"
				"						type=\"tns:SSimpRateConfig\" />\n"
				"					<element maxOccurs=\"1\" minOccurs=\"0\"\n"
				"						name=\"Temperature\" type=\"tns:SSimpRateConfig\" />\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"			<complexType name=\"SSimpStatus\">\n"
				"				<sequence>\n"
				"					<element name=\"SensorConfig\"\n"
				"						type=\"tns:SensorConfigurationType\">\n"
				"					</element>\n"
				"					<element name=\"AllSensorConfig\"\n"
				"						type=\"tns:SensorConfigurationType\">\n"
				"					</element>\n"
				"					<element name=\"BatteryVoltage\"\n"
				"						type=\"unsignedShort\">\n"
				"						<annotation>\n"
				"							<documentation></documentation>\n"
				"						</annotation>\n"
				"					</element>\n"
				"					<element name=\"CurrentTime\" type=\"dateTime\"></element>\n"
				"					<element name=\"UpTime\" type=\"dateTime\"></element>\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"			<element name=\"Status\" type=\"tns:SSimpStatus\"></element>\n"
				"\n"
				"			<complexType name=\"SSimpControl\">\n"
				"				<sequence>\n"
				"					<element name=\"SensorConfig\"\n"
				"						type=\"tns:SensorConfigurationType\" minOccurs=\"0\" maxOccurs=\"1\">\n"
				"					</element>\n"
				"					<element name=\"NewTime\" type=\"dateTime\"\n"
				"						minOccurs=\"0\" maxOccurs=\"1\">\n"
				"					</element>\n"
				"				</sequence>\n"
				"			</complexType>\n"
				"\n"
				"\n"
				"			<element name=\"StatusControl\" type=\"tns:SSimpControl\"></element>\n"
				"\n"
				"		</schema>\n"
				"	</wsdl:types>\n"
				"\n"
				"	<wsdl:message name=\"GetSensorValuesMessageIn\" />\n"
				"	<wsdl:message name=\"GetSensorValuesMessageOut\">\n"
				"		<wsdl:part name=\"out\" element=\"tns:Sample\" />\n"
				"	</wsdl:message>\n"
				"	<wsdl:message name=\"SensorValuesEventMessageOut\">\n"
				"		<wsdl:part name=\"event\" element=\"tns:Sample\" />\n"
				"	</wsdl:message>\n"
				"\n"
				"	<wsdl:message name=\"ConfigRequest\">\n"
				"		<wsdl:part name=\"in\" element=\"tns:StatusControl\"></wsdl:part>\n"
				"	</wsdl:message>\n"
				"	<wsdl:message name=\"ConfigResponse\">\n"
				"		<wsdl:part name=\"out\" element=\"tns:Status\"></wsdl:part>\n"
				"	</wsdl:message>\n"
				"\n"
				"	<wsdl:portType name=\"SensorValues\" wse:EventSource=\"true\">\n"
				"		<wsdl:operation name=\"GetSensorValues\">\n"
				"			<wsdl:input message=\"tns:GetSensorValuesMessageIn\"\n"
				"				wsa:Action=\"http://www.teco.edu/SensorValues/GetSensorValuesIn\" />\n"
				"			<wsdl:output message=\"tns:GetSensorValuesMessageOut\"\n"
				"				wsa:Action=\"http://www.teco.edu/SensorValues/GetSensorValuesOut\" />\n"
				"		</wsdl:operation>\n"
				"\n"
				"		<wsdl:operation name=\"SensorValuesEvent\">\n"
				"			<wsdl:output message=\"tns:SensorValuesEventMessageOut\"\n"
				"				wsa:Action=\"http://www.teco.edu/SensorValues/SensorValuesEventOut\" />\n"
				"		</wsdl:operation>\n"
				"\n"
				"\n"
				"		<wsdl:operation name=\"Config\">\n"
				"			<wsdl:input message=\"tns:ConfigRequest\"\n"
				"				wsa:Action=\"http://www.teco.edu/SensorValues/ConfigRequest\" />\n"
				"			<wsdl:output message=\"tns:ConfigResponse\"\n"
				"				wsa:Action=\"http://www.teco.edu/SensorValues/ConfigResponse\" />\n"
				"		</wsdl:operation>\n"
				"	</wsdl:portType>\n"
				"\n"
				"	<wsdl:binding name=\"SensorValuesSOAP\" type=\"tns:SensorValues\">\n"
				"		<soap:binding style=\"document\"\n"
				"			transport=\"http://schemas.xmlsoap.org/soap/http\" />\n"
				"		<wsdl:operation name=\"GetSensorValues\">\n"
				"			<wsdl:input>\n"
				"				<soap:body use=\"literal\" />\n"
				"			</wsdl:input>\n"
				"			<wsdl:output>\n"
				"				<soap:body use=\"literal\" />\n"
				"			</wsdl:output>\n"
				"		</wsdl:operation>\n"
				"		<wsdl:operation name=\"SensorValuesEvent\">\n"
				"			<wsdl:output>\n"
				"				<soap:body use=\"literal\" />\n"
				"			</wsdl:output>\n"
				"		</wsdl:operation>\n"
				"\n"
				"		<wsdl:operation name=\"Config\">\n"
				"			<wsdl:input>\n"
				"				<soap:body use=\"literal\" />\n"
				"			</wsdl:input>\n"
				"			<wsdl:output>\n"
				"				<soap:body use=\"literal\" />\n"
				"			</wsdl:output>\n"
				"		</wsdl:operation>\n"
				"	</wsdl:binding>\n"
				"\n"
				"	<wsdl:service name=\"SensorValues\">\n"
				"		<wsdl:port name=\"SensorValues\" binding=\"tns:SensorValuesSOAP\">\n"
				"			<wsoap12:address location=\"http://localhost:8080\" />\n"
				"		</wsdl:port>\n"
				"	</wsdl:service>\n"
				"</wsdl:definitions>\n");

	return WS4D_OK;
}

void SSimpDevice_event(int svc, int op, void* dev, char *buf, size_t len) {
	switch (svc) {

	case 0: {
		extern void SensorValues_event(int, void *, char *, size_t);
		SensorValues_event(op, dev, buf, len);
		break;
	}
	}
}

