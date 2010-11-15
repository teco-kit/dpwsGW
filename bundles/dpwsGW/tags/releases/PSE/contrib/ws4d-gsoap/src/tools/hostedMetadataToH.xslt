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

<xsl:template match="/wsm:Metadata">
#ifndef <xsl:value-of select="$nsprefix"/>_METADATA_H
#define <xsl:value-of select="$nsprefix"/>_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dpws_hosted.h"
<xsl:for-each select="wsm:MetadataSection">
<xsl:if test="@Dialect='http://schemas.xmlsoap.org/ws/2006/02/devprof/Relationship'">
<xsl:apply-templates/>
</xsl:if>
</xsl:for-each>
#ifdef __cplusplus
}
#endif

#endif /* <xsl:value-of select="$nsprefix"/>_METADATA_H */

</xsl:template>

<xsl:template match="wdp:Relationship">
<xsl:apply-templates/>
</xsl:template>

<xsl:template match="wdp:Host">
/* ignore host section */
</xsl:template>

<xsl:template match="wdp:Hosted">
int <xsl:value-of select="$nsprefix"/>_setup_<xsl:value-of select="Name"/>(struct dpws_s *device, struct soap *handle, const char *wsdl, int backlog);
</xsl:template>

</xsl:stylesheet>
