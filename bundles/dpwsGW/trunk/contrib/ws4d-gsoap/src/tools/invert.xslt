<?xml version="1.0" encoding="ISO-8859-1"?>

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
	xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/">
	<xsl:output method="xml" indent="yes" omit-xml-declaration="yes" />

	<xsl:template match="/">
		<xsl:apply-templates select="*" />
	</xsl:template>

	<xsl:template match="wsdl:input">
		<wsdl:output>
			<xsl:apply-templates select="@* | node()" />
		</wsdl:output>
	</xsl:template>

	<xsl:template match="wsdl:output">
		<wsdl:input>
			<xsl:apply-templates select="@* | node()" />
		</wsdl:input>
	</xsl:template>

	<xsl:template match="@* | node()" priority="-1">
		<xsl:copy>
			<xsl:apply-templates select="@* | node()" />
		</xsl:copy>
	</xsl:template>

	<!-- 
		<xsl:template match="@*|node()" mode="copy">
		<xsl:apply-templates select="@*" mode="copy"/>
		<xsl:apply-templates mode="copy"/>
		</xsl:template>
	-->

</xsl:stylesheet>