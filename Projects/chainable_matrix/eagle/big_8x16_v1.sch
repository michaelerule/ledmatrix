<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="mrule">
<packages>
<package name="LD-1088BS">
<wire x1="16" y1="16" x2="-16" y2="16" width="0.127" layer="21"/>
<wire x1="-16" y1="16" x2="-16" y2="-16" width="0.127" layer="21"/>
<wire x1="-16" y1="-16" x2="16" y2="-16" width="0.127" layer="21"/>
<wire x1="16" y1="-16" x2="16" y2="16" width="0.127" layer="21"/>
<pad name="C3" x="-1.27" y="12" drill="0.8" diameter="1.778"/>
<pad name="A2" x="1.27" y="12" drill="0.8" diameter="1.778"/>
<pad name="A3" x="-3.81" y="12" drill="0.8" diameter="1.778"/>
<pad name="C2" x="3.81" y="12" drill="0.8" diameter="1.778"/>
<pad name="C4" x="-6.35" y="12" drill="0.8" diameter="1.778"/>
<pad name="C1" x="6.35" y="12" drill="0.8" diameter="1.778"/>
<pad name="C5" x="-8.89" y="12" drill="0.8" diameter="1.778"/>
<pad name="A1" x="8.89" y="12" drill="0.8" diameter="1.778"/>
<pad name="C7" x="-1.27" y="-12" drill="0.8" diameter="1.778"/>
<pad name="A6" x="1.27" y="-12" drill="0.8" diameter="1.778"/>
<pad name="C8" x="-3.81" y="-12" drill="0.8" diameter="1.778"/>
<pad name="C6" x="3.81" y="-12" drill="0.8" diameter="1.778"/>
<pad name="A7" x="-6.35" y="-12" drill="0.8" diameter="1.778"/>
<pad name="A5" x="6.35" y="-12" drill="0.8" diameter="1.778"/>
<pad name="A8" x="-8.89" y="-12" drill="0.8" diameter="1.778"/>
<pad name="A4" x="8.89" y="-12" drill="0.8" diameter="1.778"/>
<text x="0" y="-9" size="1.778" layer="21" font="vector" align="center">LD-1088BS</text>
<text x="8.89" y="13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="1.27" y="13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="-3.81" y="13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="8.89" y="-13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="6.35" y="-13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="1.27" y="-13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="-8.89" y="-13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="-6.35" y="-13.97" size="1.27" layer="21" font="vector" align="center">A</text>
<text x="-3.81" y="-13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<text x="-1.27" y="-13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<text x="3.81" y="-13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<text x="-8.89" y="13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<text x="-6.35" y="13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<text x="-1.27" y="13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<text x="3.81" y="13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<text x="6.35" y="13.97" size="1.27" layer="21" font="vector" align="center">C</text>
<circle x="-14" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-10" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-6" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-2" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="2" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="6" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="10" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="-2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-14" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-10" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-6" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-2" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="2" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="6" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="10" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="2" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-14" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-10" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-6" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-2" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="2" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="6" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="10" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-14" y="10" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="10" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-14" y="-10" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="-10" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-14" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-10" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-6" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-2" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="2" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="6" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="10" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="-6" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-14" y="14" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="14" radius="1.4142125" width="0.127" layer="21"/>
<circle x="14" y="-14" radius="1.4142125" width="0.127" layer="21"/>
<circle x="-14" y="-14" radius="1.4142125" width="0.127" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="LD788BS-SS22">
<wire x1="-10.16" y1="-10.16" x2="12.7" y2="-10.16" width="0.254" layer="94"/>
<wire x1="12.7" y1="-10.16" x2="12.7" y2="12.7" width="0.254" layer="94"/>
<wire x1="12.7" y1="12.7" x2="-10.16" y2="12.7" width="0.254" layer="94"/>
<wire x1="-10.16" y1="12.7" x2="-10.16" y2="-10.16" width="0.254" layer="94"/>
<pin name="A1" x="-15.24" y="10.16" length="middle"/>
<pin name="C1" x="-15.24" y="7.62" length="middle"/>
<pin name="C2" x="-15.24" y="5.08" length="middle"/>
<pin name="A2" x="-15.24" y="2.54" length="middle"/>
<pin name="C3" x="-15.24" y="0" length="middle"/>
<pin name="A3" x="-15.24" y="-2.54" length="middle"/>
<pin name="C4" x="-15.24" y="-5.08" length="middle"/>
<pin name="C5" x="-15.24" y="-7.62" length="middle"/>
<pin name="A4" x="17.78" y="10.16" length="middle" rot="R180"/>
<pin name="A5" x="17.78" y="7.62" length="middle" rot="R180"/>
<pin name="C6" x="17.78" y="5.08" length="middle" rot="R180"/>
<pin name="A6" x="17.78" y="2.54" length="middle" rot="R180"/>
<pin name="C7" x="17.78" y="0" length="middle" rot="R180"/>
<pin name="C8" x="17.78" y="-2.54" length="middle" rot="R180"/>
<pin name="A7" x="17.78" y="-5.08" length="middle" rot="R180"/>
<pin name="A8" x="17.78" y="-7.62" length="middle" rot="R180"/>
<text x="5.08" y="0" size="1.27" layer="94" font="vector" rot="R90" align="center">LD788BS-SS22</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="LD-1088BS">
<gates>
<gate name="G$1" symbol="LD788BS-SS22" x="0" y="0"/>
</gates>
<devices>
<device name="" package="LD-1088BS">
<connects>
<connect gate="G$1" pin="A1" pad="A1"/>
<connect gate="G$1" pin="A2" pad="A2"/>
<connect gate="G$1" pin="A3" pad="A3"/>
<connect gate="G$1" pin="A4" pad="A4"/>
<connect gate="G$1" pin="A5" pad="A5"/>
<connect gate="G$1" pin="A6" pad="A6"/>
<connect gate="G$1" pin="A7" pad="A7"/>
<connect gate="G$1" pin="A8" pad="A8"/>
<connect gate="G$1" pin="C1" pad="C1"/>
<connect gate="G$1" pin="C2" pad="C2"/>
<connect gate="G$1" pin="C3" pad="C3"/>
<connect gate="G$1" pin="C4" pad="C4"/>
<connect gate="G$1" pin="C5" pad="C5"/>
<connect gate="G$1" pin="C6" pad="C6"/>
<connect gate="G$1" pin="C7" pad="C7"/>
<connect gate="G$1" pin="C8" pad="C8"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="atmel">
<description>&lt;b&gt;AVR Devices&lt;/b&gt;&lt;p&gt;
Configurable logic, microcontrollers, nonvolatile memories&lt;p&gt;
Based on the following sources:&lt;p&gt;
&lt;ul&gt;
&lt;li&gt;www.atmel.com
&lt;li&gt;CD-ROM : Configurable Logic Microcontroller Nonvolatile Memory
&lt;li&gt;CadSoft download site, www.cadsoft.de or www.cadsoftusa.com , file at90smcu_v400.zip
&lt;li&gt;avr.lbr
&lt;/ul&gt;
&lt;author&gt;Revised by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="MLF32">
<description>&lt;b&gt;32M1-A&lt;/b&gt; Micro Lead Frame package (MLF)</description>
<wire x1="-2.35" y1="2.05" x2="-2.05" y2="2.35" width="0.254" layer="21"/>
<wire x1="-2.05" y1="2.35" x2="2.05" y2="2.35" width="0.254" layer="51"/>
<wire x1="2.05" y1="2.35" x2="2.35" y2="2.05" width="0.254" layer="21"/>
<wire x1="2.35" y1="2.05" x2="2.35" y2="-2.05" width="0.254" layer="51"/>
<wire x1="2.35" y1="-2.05" x2="2.05" y2="-2.35" width="0.254" layer="21"/>
<wire x1="2.05" y1="-2.35" x2="-2.05" y2="-2.35" width="0.254" layer="51"/>
<wire x1="-2.05" y1="-2.35" x2="-2.35" y2="-2.05" width="0.254" layer="21"/>
<wire x1="-2.35" y1="-2.05" x2="-2.35" y2="2.05" width="0.254" layer="51"/>
<circle x="-1.55" y="1.55" radius="0.15" width="0.254" layer="21"/>
<smd name="1" x="-2.35" y="1.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="2" x="-2.35" y="1.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="3" x="-2.35" y="0.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="4" x="-2.35" y="0.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="5" x="-2.35" y="-0.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="6" x="-2.35" y="-0.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="7" x="-2.35" y="-1.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="8" x="-2.35" y="-1.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="9" x="-1.75" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="10" x="-1.25" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="11" x="-0.75" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="12" x="-0.25" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="13" x="0.25" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="14" x="0.75" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="15" x="1.25" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="16" x="1.75" y="-2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="17" x="2.35" y="-1.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="18" x="2.35" y="-1.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="19" x="2.35" y="-0.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="20" x="2.35" y="-0.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="21" x="2.35" y="0.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="22" x="2.35" y="0.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="23" x="2.35" y="1.25" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="24" x="2.35" y="1.75" dx="0.7" dy="0.3" layer="1" roundness="50"/>
<smd name="25" x="1.75" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="26" x="1.25" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="27" x="0.75" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="28" x="0.25" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="29" x="-0.25" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="30" x="-0.75" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="31" x="-1.25" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<smd name="32" x="-1.75" y="2.35" dx="0.3" dy="0.7" layer="1" roundness="50"/>
<text x="-2.54" y="3.175" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.54" y="-4.445" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-2.5" y1="1.6" x2="-2.05" y2="1.9" layer="51"/>
<rectangle x1="-2.5" y1="1.1" x2="-2.05" y2="1.4" layer="51"/>
<rectangle x1="-2.5" y1="0.6" x2="-2.05" y2="0.9" layer="51"/>
<rectangle x1="-2.5" y1="0.1" x2="-2.05" y2="0.4" layer="51"/>
<rectangle x1="-2.5" y1="-0.4" x2="-2.05" y2="-0.1" layer="51"/>
<rectangle x1="-2.5" y1="-0.9" x2="-2.05" y2="-0.6" layer="51"/>
<rectangle x1="-2.5" y1="-1.4" x2="-2.05" y2="-1.1" layer="51"/>
<rectangle x1="-2.5" y1="-1.9" x2="-2.05" y2="-1.6" layer="51"/>
<rectangle x1="-1.9" y1="-2.5" x2="-1.6" y2="-2.05" layer="51"/>
<rectangle x1="-1.4" y1="-2.5" x2="-1.1" y2="-2.05" layer="51"/>
<rectangle x1="-0.9" y1="-2.5" x2="-0.6" y2="-2.05" layer="51"/>
<rectangle x1="-0.4" y1="-2.5" x2="-0.1" y2="-2.05" layer="51"/>
<rectangle x1="0.1" y1="-2.5" x2="0.4" y2="-2.05" layer="51"/>
<rectangle x1="0.6" y1="-2.5" x2="0.9" y2="-2.05" layer="51"/>
<rectangle x1="1.1" y1="-2.5" x2="1.4" y2="-2.05" layer="51"/>
<rectangle x1="1.6" y1="-2.5" x2="1.9" y2="-2.05" layer="51"/>
<rectangle x1="2.05" y1="-1.9" x2="2.5" y2="-1.6" layer="51"/>
<rectangle x1="2.05" y1="-1.4" x2="2.5" y2="-1.1" layer="51"/>
<rectangle x1="2.05" y1="-0.9" x2="2.5" y2="-0.6" layer="51"/>
<rectangle x1="2.05" y1="-0.4" x2="2.5" y2="-0.1" layer="51"/>
<rectangle x1="2.05" y1="0.1" x2="2.5" y2="0.4" layer="51"/>
<rectangle x1="2.05" y1="0.6" x2="2.5" y2="0.9" layer="51"/>
<rectangle x1="2.05" y1="1.1" x2="2.5" y2="1.4" layer="51"/>
<rectangle x1="2.05" y1="1.6" x2="2.5" y2="1.9" layer="51"/>
<rectangle x1="1.6" y1="2.05" x2="1.9" y2="2.5" layer="51"/>
<rectangle x1="1.1" y1="2.05" x2="1.4" y2="2.5" layer="51"/>
<rectangle x1="0.6" y1="2.05" x2="0.9" y2="2.5" layer="51"/>
<rectangle x1="0.1" y1="2.05" x2="0.4" y2="2.5" layer="51"/>
<rectangle x1="-0.4" y1="2.05" x2="-0.1" y2="2.5" layer="51"/>
<rectangle x1="-0.9" y1="2.05" x2="-0.6" y2="2.5" layer="51"/>
<rectangle x1="-1.4" y1="2.05" x2="-1.1" y2="2.5" layer="51"/>
<rectangle x1="-1.9" y1="2.05" x2="-1.6" y2="2.5" layer="51"/>
</package>
<package name="TQFP32-08">
<description>&lt;B&gt;Thin Plasic Quad Flat Package&lt;/B&gt; Grid 0.8 mm</description>
<wire x1="3.505" y1="3.505" x2="3.505" y2="-3.505" width="0.1524" layer="21"/>
<wire x1="3.505" y1="-3.505" x2="-3.505" y2="-3.505" width="0.1524" layer="21"/>
<wire x1="-3.505" y1="-3.505" x2="-3.505" y2="3.15" width="0.1524" layer="21"/>
<wire x1="-3.15" y1="3.505" x2="3.505" y2="3.505" width="0.1524" layer="21"/>
<wire x1="-3.15" y1="3.505" x2="-3.505" y2="3.15" width="0.1524" layer="21"/>
<circle x="-2.7432" y="2.7432" radius="0.3592" width="0.1524" layer="21"/>
<smd name="1" x="-4.2926" y="2.8" dx="1.27" dy="0.5588" layer="1"/>
<smd name="2" x="-4.2926" y="2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="3" x="-4.2926" y="1.2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="4" x="-4.2926" y="0.4" dx="1.27" dy="0.5588" layer="1"/>
<smd name="5" x="-4.2926" y="-0.4" dx="1.27" dy="0.5588" layer="1"/>
<smd name="6" x="-4.2926" y="-1.2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="7" x="-4.2926" y="-2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="8" x="-4.2926" y="-2.8" dx="1.27" dy="0.5588" layer="1"/>
<smd name="9" x="-2.8" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="10" x="-2" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="11" x="-1.2" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="12" x="-0.4" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="13" x="0.4" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="14" x="1.2" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="15" x="2" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="16" x="2.8" y="-4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="17" x="4.2926" y="-2.8" dx="1.27" dy="0.5588" layer="1"/>
<smd name="18" x="4.2926" y="-2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="19" x="4.2926" y="-1.2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="20" x="4.2926" y="-0.4" dx="1.27" dy="0.5588" layer="1"/>
<smd name="21" x="4.2926" y="0.4" dx="1.27" dy="0.5588" layer="1"/>
<smd name="22" x="4.2926" y="1.2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="23" x="4.2926" y="2" dx="1.27" dy="0.5588" layer="1"/>
<smd name="24" x="4.2926" y="2.8" dx="1.27" dy="0.5588" layer="1"/>
<smd name="25" x="2.8" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="26" x="2" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="27" x="1.2" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="28" x="0.4" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="29" x="-0.4" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="30" x="-1.2" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="31" x="-2" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<smd name="32" x="-2.8" y="4.2926" dx="0.5588" dy="1.27" layer="1"/>
<text x="-2.7686" y="5.08" size="0.8128" layer="25">&gt;NAME</text>
<text x="-3.0226" y="-1.27" size="0.8128" layer="27">&gt;VALUE</text>
<rectangle x1="-4.5466" y1="2.5714" x2="-3.556" y2="3.0286" layer="51"/>
<rectangle x1="-4.5466" y1="1.7714" x2="-3.556" y2="2.2286" layer="51"/>
<rectangle x1="-4.5466" y1="0.9714" x2="-3.556" y2="1.4286" layer="51"/>
<rectangle x1="-4.5466" y1="0.1714" x2="-3.556" y2="0.6286" layer="51"/>
<rectangle x1="-4.5466" y1="-0.6286" x2="-3.556" y2="-0.1714" layer="51"/>
<rectangle x1="-4.5466" y1="-1.4286" x2="-3.556" y2="-0.9714" layer="51"/>
<rectangle x1="-4.5466" y1="-2.2286" x2="-3.556" y2="-1.7714" layer="51"/>
<rectangle x1="-4.5466" y1="-3.0286" x2="-3.556" y2="-2.5714" layer="51"/>
<rectangle x1="-3.0286" y1="-4.5466" x2="-2.5714" y2="-3.556" layer="51"/>
<rectangle x1="-2.2286" y1="-4.5466" x2="-1.7714" y2="-3.556" layer="51"/>
<rectangle x1="-1.4286" y1="-4.5466" x2="-0.9714" y2="-3.556" layer="51"/>
<rectangle x1="-0.6286" y1="-4.5466" x2="-0.1714" y2="-3.556" layer="51"/>
<rectangle x1="0.1714" y1="-4.5466" x2="0.6286" y2="-3.556" layer="51"/>
<rectangle x1="0.9714" y1="-4.5466" x2="1.4286" y2="-3.556" layer="51"/>
<rectangle x1="1.7714" y1="-4.5466" x2="2.2286" y2="-3.556" layer="51"/>
<rectangle x1="2.5714" y1="-4.5466" x2="3.0286" y2="-3.556" layer="51"/>
<rectangle x1="3.556" y1="-3.0286" x2="4.5466" y2="-2.5714" layer="51"/>
<rectangle x1="3.556" y1="-2.2286" x2="4.5466" y2="-1.7714" layer="51"/>
<rectangle x1="3.556" y1="-1.4286" x2="4.5466" y2="-0.9714" layer="51"/>
<rectangle x1="3.556" y1="-0.6286" x2="4.5466" y2="-0.1714" layer="51"/>
<rectangle x1="3.556" y1="0.1714" x2="4.5466" y2="0.6286" layer="51"/>
<rectangle x1="3.556" y1="0.9714" x2="4.5466" y2="1.4286" layer="51"/>
<rectangle x1="3.556" y1="1.7714" x2="4.5466" y2="2.2286" layer="51"/>
<rectangle x1="3.556" y1="2.5714" x2="4.5466" y2="3.0286" layer="51"/>
<rectangle x1="2.5714" y1="3.556" x2="3.0286" y2="4.5466" layer="51"/>
<rectangle x1="1.7714" y1="3.556" x2="2.2286" y2="4.5466" layer="51"/>
<rectangle x1="0.9714" y1="3.556" x2="1.4286" y2="4.5466" layer="51"/>
<rectangle x1="0.1714" y1="3.556" x2="0.6286" y2="4.5466" layer="51"/>
<rectangle x1="-0.6286" y1="3.556" x2="-0.1714" y2="4.5466" layer="51"/>
<rectangle x1="-1.4286" y1="3.556" x2="-0.9714" y2="4.5466" layer="51"/>
<rectangle x1="-2.2286" y1="3.556" x2="-1.7714" y2="4.5466" layer="51"/>
<rectangle x1="-3.0286" y1="3.556" x2="-2.5714" y2="4.5466" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="23-I/O">
<wire x1="-17.78" y1="27.94" x2="20.32" y2="27.94" width="0.254" layer="94"/>
<wire x1="20.32" y1="27.94" x2="20.32" y2="-35.56" width="0.254" layer="94"/>
<wire x1="20.32" y1="-35.56" x2="-17.78" y2="-35.56" width="0.254" layer="94"/>
<wire x1="-17.78" y1="-35.56" x2="-17.78" y2="27.94" width="0.254" layer="94"/>
<text x="-17.78" y="-38.1" size="1.778" layer="95">&gt;NAME</text>
<text x="-17.78" y="29.21" size="1.778" layer="96">&gt;VALUE</text>
<pin name="PB5(SCK)" x="25.4" y="-33.02" length="middle" rot="R180"/>
<pin name="PB7(XTAL2/TOSC2)" x="-22.86" y="2.54" length="middle"/>
<pin name="PB6(XTAL1/TOSC1)" x="-22.86" y="7.62" length="middle"/>
<pin name="GND@1" x="-22.86" y="-5.08" length="middle" direction="pwr"/>
<pin name="GND@2" x="-22.86" y="-7.62" length="middle" direction="pwr"/>
<pin name="VCC@1" x="-22.86" y="-10.16" length="middle" direction="pwr"/>
<pin name="VCC@2" x="-22.86" y="-12.7" length="middle" direction="pwr"/>
<pin name="GND" x="-22.86" y="20.32" length="middle" direction="pwr"/>
<pin name="AREF" x="-22.86" y="17.78" length="middle" direction="pas"/>
<pin name="AVCC" x="-22.86" y="15.24" length="middle" direction="pwr"/>
<pin name="PB4(MISO)" x="25.4" y="-30.48" length="middle" rot="R180"/>
<pin name="PB3(MOSI/OC2)" x="25.4" y="-27.94" length="middle" rot="R180"/>
<pin name="PB2(SS/OC1B)" x="25.4" y="-25.4" length="middle" rot="R180"/>
<pin name="PB1(OC1A)" x="25.4" y="-22.86" length="middle" rot="R180"/>
<pin name="PB0(ICP)" x="25.4" y="-20.32" length="middle" rot="R180"/>
<pin name="PD7(AIN1)" x="25.4" y="-15.24" length="middle" rot="R180"/>
<pin name="PD6(AIN0)" x="25.4" y="-12.7" length="middle" rot="R180"/>
<pin name="PD5(T1)" x="25.4" y="-10.16" length="middle" rot="R180"/>
<pin name="PD4(XCK/T0)" x="25.4" y="-7.62" length="middle" rot="R180"/>
<pin name="PD3(INT1)" x="25.4" y="-5.08" length="middle" rot="R180"/>
<pin name="PD2(INT0)" x="25.4" y="-2.54" length="middle" rot="R180"/>
<pin name="PD1(TXD)" x="25.4" y="0" length="middle" rot="R180"/>
<pin name="PD0(RXD)" x="25.4" y="2.54" length="middle" rot="R180"/>
<pin name="ADC7" x="25.4" y="7.62" length="middle" rot="R180"/>
<pin name="ADC6" x="25.4" y="10.16" length="middle" rot="R180"/>
<pin name="PC5(ADC5/SCL)" x="25.4" y="12.7" length="middle" rot="R180"/>
<pin name="PC4(ADC4/SDA)" x="25.4" y="15.24" length="middle" rot="R180"/>
<pin name="PC3(ADC3)" x="25.4" y="17.78" length="middle" rot="R180"/>
<pin name="PC2(ADC2)" x="25.4" y="20.32" length="middle" rot="R180"/>
<pin name="PC1(ADC1)" x="25.4" y="22.86" length="middle" rot="R180"/>
<pin name="PC0(ADC0)" x="25.4" y="25.4" length="middle" rot="R180"/>
<pin name="PC6(/RESET)" x="-22.86" y="25.4" length="middle" function="dot"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="MEGA8" prefix="IC">
<description>&lt;b&gt;MICROCONTROLLER&lt;/b&gt;&lt;p&gt;
8 Kbytes FLASH, 1 kbytes SRAM, 512 bytes EEPROM, USART, 6-channel 10 bit ADC, 2-channel 8 bit ADC&lt;br&gt;
Pin compatible with Atmega48, ATMega88, ATMega168&lt;br&gt;
Source: avr.lbr</description>
<gates>
<gate name="G$1" symbol="23-I/O" x="0" y="2.54"/>
</gates>
<devices>
<device name="-MI" package="MLF32">
<connects>
<connect gate="G$1" pin="ADC6" pad="19"/>
<connect gate="G$1" pin="ADC7" pad="22"/>
<connect gate="G$1" pin="AREF" pad="20"/>
<connect gate="G$1" pin="AVCC" pad="18"/>
<connect gate="G$1" pin="GND" pad="21"/>
<connect gate="G$1" pin="GND@1" pad="3"/>
<connect gate="G$1" pin="GND@2" pad="5"/>
<connect gate="G$1" pin="PB0(ICP)" pad="12"/>
<connect gate="G$1" pin="PB1(OC1A)" pad="13"/>
<connect gate="G$1" pin="PB2(SS/OC1B)" pad="14"/>
<connect gate="G$1" pin="PB3(MOSI/OC2)" pad="15"/>
<connect gate="G$1" pin="PB4(MISO)" pad="16"/>
<connect gate="G$1" pin="PB5(SCK)" pad="17"/>
<connect gate="G$1" pin="PB6(XTAL1/TOSC1)" pad="7"/>
<connect gate="G$1" pin="PB7(XTAL2/TOSC2)" pad="8"/>
<connect gate="G$1" pin="PC0(ADC0)" pad="23"/>
<connect gate="G$1" pin="PC1(ADC1)" pad="24"/>
<connect gate="G$1" pin="PC2(ADC2)" pad="25"/>
<connect gate="G$1" pin="PC3(ADC3)" pad="26"/>
<connect gate="G$1" pin="PC4(ADC4/SDA)" pad="27"/>
<connect gate="G$1" pin="PC5(ADC5/SCL)" pad="28"/>
<connect gate="G$1" pin="PC6(/RESET)" pad="29"/>
<connect gate="G$1" pin="PD0(RXD)" pad="30"/>
<connect gate="G$1" pin="PD1(TXD)" pad="31"/>
<connect gate="G$1" pin="PD2(INT0)" pad="32"/>
<connect gate="G$1" pin="PD3(INT1)" pad="1"/>
<connect gate="G$1" pin="PD4(XCK/T0)" pad="2"/>
<connect gate="G$1" pin="PD5(T1)" pad="9"/>
<connect gate="G$1" pin="PD6(AIN0)" pad="10"/>
<connect gate="G$1" pin="PD7(AIN1)" pad="11"/>
<connect gate="G$1" pin="VCC@1" pad="4"/>
<connect gate="G$1" pin="VCC@2" pad="6"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="ATMEGA88PA-MU" constant="no"/>
<attribute name="OC_FARNELL" value="1715491" constant="no"/>
<attribute name="OC_NEWARK" value="15R0272" constant="no"/>
</technology>
</technologies>
</device>
<device name="-AI" package="TQFP32-08">
<connects>
<connect gate="G$1" pin="ADC6" pad="19"/>
<connect gate="G$1" pin="ADC7" pad="22"/>
<connect gate="G$1" pin="AREF" pad="20"/>
<connect gate="G$1" pin="AVCC" pad="18"/>
<connect gate="G$1" pin="GND" pad="21"/>
<connect gate="G$1" pin="GND@1" pad="3"/>
<connect gate="G$1" pin="GND@2" pad="5"/>
<connect gate="G$1" pin="PB0(ICP)" pad="12"/>
<connect gate="G$1" pin="PB1(OC1A)" pad="13"/>
<connect gate="G$1" pin="PB2(SS/OC1B)" pad="14"/>
<connect gate="G$1" pin="PB3(MOSI/OC2)" pad="15"/>
<connect gate="G$1" pin="PB4(MISO)" pad="16"/>
<connect gate="G$1" pin="PB5(SCK)" pad="17"/>
<connect gate="G$1" pin="PB6(XTAL1/TOSC1)" pad="7"/>
<connect gate="G$1" pin="PB7(XTAL2/TOSC2)" pad="8"/>
<connect gate="G$1" pin="PC0(ADC0)" pad="23"/>
<connect gate="G$1" pin="PC1(ADC1)" pad="24"/>
<connect gate="G$1" pin="PC2(ADC2)" pad="25"/>
<connect gate="G$1" pin="PC3(ADC3)" pad="26"/>
<connect gate="G$1" pin="PC4(ADC4/SDA)" pad="27"/>
<connect gate="G$1" pin="PC5(ADC5/SCL)" pad="28"/>
<connect gate="G$1" pin="PC6(/RESET)" pad="29"/>
<connect gate="G$1" pin="PD0(RXD)" pad="30"/>
<connect gate="G$1" pin="PD1(TXD)" pad="31"/>
<connect gate="G$1" pin="PD2(INT0)" pad="32"/>
<connect gate="G$1" pin="PD3(INT1)" pad="1"/>
<connect gate="G$1" pin="PD4(XCK/T0)" pad="2"/>
<connect gate="G$1" pin="PD5(T1)" pad="9"/>
<connect gate="G$1" pin="PD6(AIN0)" pad="10"/>
<connect gate="G$1" pin="PD7(AIN1)" pad="11"/>
<connect gate="G$1" pin="VCC@1" pad="4"/>
<connect gate="G$1" pin="VCC@2" pad="6"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="ATMEGA8-16AU" constant="no"/>
<attribute name="OC_FARNELL" value="9171371" constant="no"/>
<attribute name="OC_NEWARK" value="73M8863" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U$1" library="mrule" deviceset="LD-1088BS" device=""/>
<part name="U$2" library="mrule" deviceset="LD-1088BS" device=""/>
<part name="IC1" library="atmel" deviceset="MEGA8" device="-AI"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$1" gate="G$1" x="48.26" y="40.64"/>
<instance part="U$2" gate="G$1" x="48.26" y="63.5"/>
<instance part="IC1" gate="G$1" x="109.22" y="40.64" rot="R180"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$17" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PC0(ADC0)"/>
<pinref part="U$1" gate="G$1" pin="A8"/>
<wire x1="83.82" y1="15.24" x2="66.04" y2="33.02" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="C5"/>
<wire x1="66.04" y1="33.02" x2="33.02" y2="55.88" width="0.1524" layer="91"/>
<junction x="66.04" y="33.02"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PC1(ADC1)"/>
<pinref part="U$1" gate="G$1" pin="A7"/>
<wire x1="83.82" y1="17.78" x2="66.04" y2="35.56" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="C4"/>
<wire x1="33.02" y1="58.42" x2="66.04" y2="35.56" width="0.1524" layer="91"/>
<junction x="66.04" y="35.56"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PC2(ADC2)"/>
<pinref part="U$1" gate="G$1" pin="C8"/>
<wire x1="83.82" y1="20.32" x2="66.04" y2="38.1" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="A3"/>
<wire x1="66.04" y1="38.1" x2="33.02" y2="60.96" width="0.1524" layer="91"/>
<junction x="66.04" y="38.1"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PC3(ADC3)"/>
<pinref part="U$1" gate="G$1" pin="C7"/>
<wire x1="83.82" y1="22.86" x2="66.04" y2="40.64" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="A1"/>
<wire x1="33.02" y1="73.66" x2="66.04" y2="40.64" width="0.1524" layer="91"/>
<junction x="66.04" y="40.64"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PC4(ADC4/SDA)"/>
<pinref part="U$1" gate="G$1" pin="A6"/>
<wire x1="83.82" y1="25.4" x2="66.04" y2="43.18" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="C3"/>
<wire x1="33.02" y1="63.5" x2="66.04" y2="43.18" width="0.1524" layer="91"/>
<junction x="66.04" y="43.18"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="PC5(ADC5/SCL)"/>
<pinref part="U$1" gate="G$1" pin="C6"/>
<wire x1="83.82" y1="27.94" x2="66.04" y2="45.72" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="A2"/>
<wire x1="66.04" y1="45.72" x2="33.02" y2="66.04" width="0.1524" layer="91"/>
<junction x="66.04" y="45.72"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="A5"/>
<pinref part="U$2" gate="G$1" pin="C2"/>
<wire x1="33.02" y1="68.58" x2="66.04" y2="48.26" width="0.1524" layer="91"/>
<wire x1="66.04" y1="48.26" x2="83.82" y2="48.26" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PD4(XCK/T0)"/>
<junction x="66.04" y="48.26"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="A4"/>
<pinref part="U$2" gate="G$1" pin="C1"/>
<wire x1="66.04" y1="50.8" x2="33.02" y2="71.12" width="0.1524" layer="91"/>
<wire x1="66.04" y1="50.8" x2="83.82" y2="50.8" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PD5(T1)"/>
<junction x="66.04" y="50.8"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="C6"/>
<pinref part="U$1" gate="G$1" pin="A2"/>
<wire x1="66.04" y1="68.58" x2="33.02" y2="43.18" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PB3(MOSI/OC2)"/>
<wire x1="83.82" y1="68.58" x2="66.04" y2="68.58" width="0.1524" layer="91"/>
<junction x="66.04" y="68.58"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="A6"/>
<pinref part="U$1" gate="G$1" pin="C3"/>
<wire x1="33.02" y1="40.64" x2="66.04" y2="66.04" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PB2(SS/OC1B)"/>
<wire x1="83.82" y1="66.04" x2="66.04" y2="66.04" width="0.1524" layer="91"/>
<junction x="66.04" y="66.04"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="C7"/>
<pinref part="U$1" gate="G$1" pin="A1"/>
<wire x1="33.02" y1="50.8" x2="66.04" y2="63.5" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PB1(OC1A)"/>
<wire x1="83.82" y1="63.5" x2="66.04" y2="63.5" width="0.1524" layer="91"/>
<junction x="66.04" y="63.5"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="C8"/>
<pinref part="U$1" gate="G$1" pin="A3"/>
<wire x1="33.02" y1="38.1" x2="66.04" y2="60.96" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PB0(ICP)"/>
<wire x1="83.82" y1="60.96" x2="66.04" y2="60.96" width="0.1524" layer="91"/>
<junction x="66.04" y="60.96"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="A7"/>
<pinref part="U$1" gate="G$1" pin="C4"/>
<wire x1="33.02" y1="35.56" x2="66.04" y2="58.42" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PD7(AIN1)"/>
<wire x1="83.82" y1="55.88" x2="68.58" y2="55.88" width="0.1524" layer="91"/>
<wire x1="68.58" y1="55.88" x2="66.04" y2="58.42" width="0.1524" layer="91"/>
<junction x="66.04" y="58.42"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="A8"/>
<pinref part="U$1" gate="G$1" pin="C5"/>
<wire x1="33.02" y1="33.02" x2="66.04" y2="55.88" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PD6(AIN0)"/>
<wire x1="83.82" y1="53.34" x2="68.58" y2="53.34" width="0.1524" layer="91"/>
<wire x1="68.58" y1="53.34" x2="66.04" y2="55.88" width="0.1524" layer="91"/>
<junction x="66.04" y="55.88"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="A5"/>
<pinref part="U$1" gate="G$1" pin="C2"/>
<wire x1="33.02" y1="45.72" x2="66.04" y2="71.12" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PB4(MISO)"/>
<wire x1="83.82" y1="71.12" x2="66.04" y2="71.12" width="0.1524" layer="91"/>
<junction x="66.04" y="71.12"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="A4"/>
<pinref part="U$1" gate="G$1" pin="C1"/>
<wire x1="66.04" y1="73.66" x2="33.02" y2="48.26" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="PB5(SCK)"/>
<wire x1="83.82" y1="73.66" x2="66.04" y2="73.66" width="0.1524" layer="91"/>
<junction x="66.04" y="73.66"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="GND@2"/>
<pinref part="IC1" gate="G$1" pin="GND@1"/>
<wire x1="132.08" y1="48.26" x2="132.08" y2="45.72" width="0.1524" layer="91"/>
<wire x1="132.08" y1="45.72" x2="134.62" y2="43.18" width="0.1524" layer="91"/>
<wire x1="134.62" y1="43.18" x2="134.62" y2="22.86" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="GND"/>
<wire x1="134.62" y1="22.86" x2="132.08" y2="20.32" width="0.1524" layer="91"/>
<junction x="132.08" y="45.72"/>
</segment>
</net>
<net name="N$19" class="0">
<segment>
<pinref part="IC1" gate="G$1" pin="AVCC"/>
<pinref part="IC1" gate="G$1" pin="AREF"/>
<wire x1="132.08" y1="25.4" x2="132.08" y2="22.86" width="0.1524" layer="91"/>
<wire x1="132.08" y1="25.4" x2="137.16" y2="30.48" width="0.1524" layer="91"/>
<pinref part="IC1" gate="G$1" pin="VCC@2"/>
<pinref part="IC1" gate="G$1" pin="VCC@1"/>
<wire x1="132.08" y1="53.34" x2="132.08" y2="50.8" width="0.1524" layer="91"/>
<wire x1="137.16" y1="30.48" x2="137.16" y2="45.72" width="0.1524" layer="91"/>
<wire x1="137.16" y1="45.72" x2="132.08" y2="50.8" width="0.1524" layer="91"/>
<junction x="132.08" y="50.8"/>
<junction x="132.08" y="25.4"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
