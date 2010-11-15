<?xml version="1.0" encoding="UTF-8"?>

<!--
 _one line to give the program's name and a brief idea of what it does._
 Copyright (C) 2007  University of Rostock

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 02110-1301 USA.
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:wsm="http://schemas.xmlsoap.org/ws/2004/09/mex"
    xmlns:wdp="http://schemas.xmlsoap.org/ws/2006/02/devprof"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:wsd="http://schemas.xmlsoap.org/ws/2005/04/discovery"
    xmlns:wsa="http://schemas.xmlsoap.org/ws/2004/08/addressing"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema">
<xsl:output method="text" encoding="UTF-8"/>

<xsl:param name="nsprefix" select="'dpws'"/>

<xsl:template match="text()" />

<xsl:template match="node()" mode="AddType" name="AddType">
<xsl:param name="type" select="."/>
<xsl:param name="begin" select="."/>
<xsl:param name="middle" select="."/>
<xsl:param name="end" select="."/>
<xsl:choose>
<xsl:when test="contains($type, ':')">
<xsl:value-of select="$begin" />
<xsl:for-each select="namespace::*">
<xsl:choose>
<xsl:when test="contains(substring-before($type, ':'), name(.))">"<xsl:value-of select="."/>"</xsl:when>
</xsl:choose>
</xsl:for-each>
<xsl:value-of select="$middle" />"<xsl:value-of select="substring-after($type, ':')" />"<xsl:value-of select="$end" /></xsl:when>
</xsl:choose>
</xsl:template>

<xsl:template match="node()" mode="AddTypes" name="AddTypes">
<xsl:param name="types" select="."/>
<xsl:param name="begin" select="."/>
<xsl:param name="middle" select="."/>
<xsl:param name="end" select="."/>
<xsl:choose>
<xsl:when test="contains($types, ' ')">
<xsl:call-template name="AddType">
<xsl:with-param name="type" select="substring-before($types, ' ')" />
<xsl:with-param name="begin" select="$begin" />
<xsl:with-param name="middle" select="$middle" />
<xsl:with-param name="end" select="$end" />
</xsl:call-template>
<xsl:call-template name="AddTypes">
<xsl:with-param name="types" select="substring-after($types, ' ')" />
<xsl:with-param name="begin" select="$begin" />
<xsl:with-param name="middle" select="$middle" />
<xsl:with-param name="end" select="$end" />
</xsl:call-template>
</xsl:when>
<xsl:otherwise>
<xsl:call-template name="AddType">
<xsl:with-param name="type" select="$types" />
<xsl:with-param name="begin" select="$begin" />
<xsl:with-param name="middle" select="$middle" />
<xsl:with-param name="end" select="$end" />
</xsl:call-template>
</xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="/wsm:Metadata">
#include "dpwsH.h"
#include "dpws_hosted.h"
#include "<xsl:value-of select="$nsprefix"/>_metadata.h"
<xsl:for-each select="wsm:MetadataSection">
<xsl:if test="@Dialect='http://schemas.xmlsoap.org/ws/2006/02/devprof/Relationship'">
<xsl:apply-templates/>
</xsl:if>
</xsl:for-each>
<xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="wdp:Relationship">
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="wdp:Host">
/* ignore host section */
</xsl:template>

<xsl:template match="wdp:Hosted">
int <xsl:value-of select="$nsprefix"/>_setup_<xsl:value-of select="Name"/>(struct dpws_s *device, struct soap *handle, const char *wsdl, int backlog)
{
  char uri[DPWS_URI_MAX_LEN] = <xsl:choose><xsl:when test="count(wsa:EndpointReference/wsa:Address)!= 0">&quot;<xsl:value-of select="wsa:EndpointReference/wsa:Address"/>&quot;;</xsl:when><xsl:otherwise>"http://host:0/";</xsl:otherwise></xsl:choose>
  struct ws4d_qname *service_type;
  struct ws4d_epr *service = NULL;

  service = dpws_service_init(device, &quot;<xsl:value-of select="wdp:ServiceId"/>&quot;);

  if (dpws_service_bind (device, service, handle, uri, DPWS_URI_MAX_LEN, backlog))
  {
    return WS4D_ERR;
  }

  service_type = NULL;
  <xsl:choose>
  <xsl:when test="wdp:Types!=''">
  <xsl:call-template name="AddTypes">
        <xsl:with-param name="types"
                        select="wdp:Types" />
        <xsl:with-param name="begin">
  service_type = ws4d_qname_alloc (1, &amp;device->alloc_list);
  service_type->ns = ws4d_strdup (</xsl:with-param><xsl:with-param name="middle">, &amp;device->alloc_list);
  service_type->name = ws4d_strdup (</xsl:with-param><xsl:with-param name="end">, &amp;device->alloc_list);
  if (dpws_service_add_type (service, service_type))
  {
    return WS4D_ERR;
  }
  </xsl:with-param>
  </xsl:call-template>
  </xsl:when>
  </xsl:choose>

  if (wsdl &amp;&amp; *wsdl)
  {
    if (dpws_service_set_wsdl(service, wsdl))
    {
      return WS4D_ERR;
    }
  }

  return dpws_add_hosted_service (device, service, uri, DPWS_URI_MAX_LEN);
}
</xsl:template>
</xsl:stylesheet>
