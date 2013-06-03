<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" >
<xsl:output method="text"/>

<!-- ************************* -->
<!-- ** MAIN CALL TO FORMAT ** -->
<!-- ************************* -->
<xsl:template match="/rpms_status">
    <xsl:call-template name="head"/>
    <xsl:call-template name="comp_detail"/>
    <xsl:call-template name="comp_summary"/>
    <xsl:call-template name="node_summary"/>
    <xsl:call-template name="foot"/>
</xsl:template>

<!-- ************************** -->
<!-- ** FORMATTING TEMPLATES ** -->
<!-- ************************** -->

<xsl:template name="comp_detail">
    <xsl:call-template name="comp_detail_title"/>

    <!-- This is a catch all if the systemi is not running -->
    <xsl:if test="sum(/rpms_status/daemon_status/@running) != 0">
        <xsl:for-each select="daemon_status">
            <xsl:sort order="ascending" select="@hostname"/>
            <xsl:if test="position() > 1">
                <xsl:call-template name="part_break"/>
            </xsl:if>
            <xsl:call-template name="tab"/>
            <xsl:value-of select="@hostname"/>
            <xsl:call-template name="nl"/>
    
            <!-- If we have nothing assigned to the daemon -->
            <xsl:if test="sum(component_status) = 0 and @running = 1">
                <xsl:call-template name="tab"/>
                <xsl:call-template name="tab"/>
                <xsl:text>No components have been assigned to this daemon</xsl:text>
                <xsl:call-template name="nl"/>
            </xsl:if>
    
            <xsl:if test="sum(component_status) = 0 and @registered = 0">
                <xsl:call-template name="errtab"/>
                <xsl:call-template name="tab"/>
                <xsl:text>Daemon not registered *</xsl:text>
                <xsl:call-template name="nl"/>
            </xsl:if>
    
            <xsl:for-each select="component_status">
                <xsl:sort order="ascending" select="@compname"/>
                <xsl:call-template name="tab"/>
                <xsl:call-template name="tab"/>
                <xsl:value-of select="@compname"/>
                <xsl:text>:</xsl:text>
                <xsl:value-of select="@sysinstance"/>
                <xsl:if test="@running = 1">
                    <xsl:text> - pid (</xsl:text>
                    <xsl:value-of select="@pid"/>
                    <xsl:text>), rss (</xsl:text>
                    <xsl:value-of select="@rss"/>
                    <xsl:text>k), rss growth (</xsl:text>
                    <xsl:value-of select="@rssavg"/>
                    <xsl:text>k/sec), cputime (</xsl:text>
                    <xsl:value-of select="@cpu"/>
                    <xsl:text>), cpuAvg (</xsl:text>
                    <xsl:value-of select="@cpuavg"/>
                    <xsl:text>/sec), starttime (</xsl:text>
                    <xsl:value-of select="@starttime"/>
                    <xsl:text>)</xsl:text>
                </xsl:if>
                <xsl:if test="@running = 0">
                    <xsl:text> - not running *</xsl:text>
                </xsl:if>
                <xsl:call-template name="nl"/>
            </xsl:for-each>
    
            
        </xsl:for-each>
    </xsl:if>

    <!-- Catch all for the system not running -->
    <xsl:if test="sum(/rpms_status/daemon_status/@running) = 0">
        <xsl:call-template name="tab"/>
            <xsl:text>The system is currently not running</xsl:text>
        <xsl:call-template name="nl"/>
    </xsl:if>

    <!-- end with a new line -->
    <xsl:call-template name="nl"/>

</xsl:template>


<xsl:template name="comp_summary">
    <xsl:call-template name="comp_title"/>

    <!-- If there are none running the xslt transformer will fall over -->
    <xsl:if test="sum(/rpms_status/daemon_status/@running) != 0">
        <xsl:if test="sum(/rpms_status/daemon_status/component_status) != 0">
            <xsl:for-each select="daemon_status/component_status[not(@compname = parent::*/preceding-sibling::daemon_status/component_status/@compname) 
                                                                and not(@compname = preceding-sibling::component_status/@compname)]">
                <xsl:sort order="ascending" select="@compname"/>
                <xsl:variable name="exe">
                    <xsl:value-of select="@compname"/>
                </xsl:variable>
                <xsl:call-template name="tab"/>
                <xsl:value-of select="$exe"/>
                <xsl:text> [</xsl:text>
                    <xsl:value-of select="sum(/rpms_status/daemon_status/component_status[@compname = $exe]/@running)" />
                <xsl:text>/</xsl:text>
                    <xsl:value-of select="sum(/rpms_status/daemon_status/component_status[@compname = $exe]/@runnable)" />
                <xsl:text>]</xsl:text>
                <xsl:if test="sum(/rpms_status/daemon_status/component_status[@compname = $exe]/@running) != sum(/rpms_status/daemon_status/component_status[@compname = $exe]/@runnable)">
                    <xsl:text> *</xsl:text>
                </xsl:if>
                <xsl:call-template name="nl"/>
            </xsl:for-each>
        </xsl:if>

        <xsl:if test="sum(/rpms_status/daemon_status/component_status) = 0">
            <xsl:call-template name="tab"/>
            <xsl:text>No components have been assigned to the daemons at the moment</xsl:text>
            <xsl:call-template name="nl"/>
        </xsl:if>
    </xsl:if>

    <xsl:if test="sum(/rpms_status/daemon_status/@running) = 0">
        <xsl:call-template name="tab"/>
        <xsl:text>The system is currently not running</xsl:text>
        <xsl:call-template name="nl"/>
    </xsl:if>
    <xsl:call-template name="nl"/>
</xsl:template>

<!-- -->
<xsl:template name="node_summary">
    <xsl:call-template name="node_title"/>
    
    <xsl:for-each select="daemon_status">
        <xsl:sort order="ascending" select="@hostname"/>

        <xsl:if test="position() > 1">
            <xsl:call-template name="part_break"/>
        </xsl:if>
        <xsl:call-template name="tab"/>
        <xsl:value-of select="@hostname"/>
        <xsl:call-template name="nl"/>

        <xsl:if test="@registered='1'">
            <xsl:call-template name="tab"/>
            <xsl:call-template name="tab"/>
            <xsl:text>DaemonStatus ........ REGISTERED</xsl:text>
            <xsl:call-template name="nl"/>
            <xsl:if test="@running='1'">
                <xsl:call-template name="tab"/>
                <xsl:call-template name="tab"/>
                <xsl:text>AppStatus ........... RUNNING</xsl:text>
                <xsl:call-template name="nl"/>
            </xsl:if> 
            <xsl:if test="@running='0'">
                <xsl:call-template name="errtab"/>
                <xsl:call-template name="tab"/>
                <xsl:text>AppStatus ........... NOT RUNNING *</xsl:text>
                <xsl:call-template name="nl"/>
            </xsl:if>
                <xsl:if test="@directorhost='1'">
                <xsl:call-template name="tab"/>
                <xsl:call-template name="tab"/>
                <xsl:text>Director Status ..... MASTER</xsl:text>
                <xsl:call-template name="nl"/>
            </xsl:if>
            <xsl:if test="@directorhost='0'">
                <xsl:call-template name="tab"/>
                <xsl:call-template name="tab"/>
                <xsl:text>Director Status ..... SLAVE</xsl:text>
                <xsl:call-template name="nl"/>
            </xsl:if>
        </xsl:if>
        <xsl:if test="@registered='0'">
            <xsl:call-template name="errtab"/>
            <xsl:call-template name="tab"/>
            <xsl:text>DaemonStatus ........ NOT REGISTERED *</xsl:text>
            <xsl:call-template name="nl"/>
        </xsl:if>
    </xsl:for-each>
    <xsl:call-template name="nl"/>
</xsl:template>

<!-- ***** TITLES ***** -->
<!-- ***** NODE TITLE ***** -->
<xsl:template name="node_title">
    <xsl:call-template name="title_break"/>
    <xsl:text>NODE SUMMARY</xsl:text>
    <xsl:call-template name="nl"/>
    <xsl:call-template name="title_break"/>
</xsl:template>

<!-- ***** COMP TITLE ***** -->
<xsl:template name="comp_title">
    <xsl:call-template name="title_break"/>
    <xsl:text>COMPONENT SUMMARY</xsl:text>
    <xsl:call-template name="nl"/>
    <xsl:call-template name="title_break"/>
</xsl:template>

<!-- ***** COMP TITLE ***** -->
<xsl:template name="comp_detail_title">
    <xsl:call-template name="title_break"/>
    <xsl:text>COMPONENT DETAIL</xsl:text>
    <xsl:call-template name="nl"/>
    <xsl:call-template name="title_break"/>
</xsl:template>



<!-- ***** PAGE HEADER ***** -->
<xsl:template name="head">
    <xsl:text>====================================================</xsl:text>
    <xsl:call-template name="nl"/>
    <xsl:call-template name="nl"/>
    <xsl:text>*** RPMS Status for </xsl:text>
    <xsl:value-of select="/rpms_status/system_status/@systemname"/>
    <xsl:text> [</xsl:text>
    <xsl:value-of select="/rpms_status/system_status/@instanceid"/>
    <xsl:text>] - </xsl:text>
    <xsl:value-of select="/rpms_status/system_status/@date"/>
    <xsl:text> ***</xsl:text>
    <xsl:call-template name="nl"/>
</xsl:template>

<!-- ***** PAGE FOOTER ***** -->
<xsl:template name="foot">
    <xsl:text>====================================================</xsl:text>
    <xsl:call-template name="nl"/>
</xsl:template>


<!-- Extras -->
<!-- ***** NEW LINE ***** -->
<xsl:template name="nl">
<xsl:text>
</xsl:text>
</xsl:template>

<!-- ***** TAB SPACES ***** -->
<xsl:template name="tab">
    <xsl:text>    </xsl:text>
</xsl:template>

<!-- ***** TAB SPACES ***** -->
<xsl:template name="errtab">
    <xsl:text>    </xsl:text>
</xsl:template>

<!-- ***** title break ***** -->
<xsl:template name="part_break">
    <xsl:call-template name="tab"/>
    <xsl:text>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~</xsl:text>
    <xsl:call-template name="nl"/>
</xsl:template>

<!-- ***** title break ***** -->
<xsl:template name="title_break">
    <xsl:text>*****************************************************</xsl:text>
    <xsl:call-template name="nl"/>
</xsl:template>


<!-- ################# -->
   
</xsl:stylesheet>
