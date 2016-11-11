---
title: 'Kactus2: A graphical EDA tool built on the IP-XACT standard'
tags:
  - example
  - tags
  - for the paper
authors:
 - name: Antti Kamppi
   affiliation: 1
 - name: Esko Pekkarinen
   affiliation: 1
 - name: Janne Virtanen
   affiliation: 1
 - name: Joni-Matti M‰‰tt‰
   affiliation: 1
 - name: Juho J‰rvinen
   affiliation: 1
 - name: Lauri Matilainen
   affiliation: 1
 - name: Mikko Teuho
   affiliation: 1
 - name: Timo D. H‰m‰l‰inen
   affiliation: 1
affiliations:
 - name: Tampere University of Technology
   index: 1
date: 11 November 2016
bibliography: paper.bib
---

# Summary

Kactus2 is a graphical Electronic Design Automation [@DEV] tool targeted for packing and assembling building blocks for ASICs and FPFGAs. Kactus2 is based on IEEE 1685-2014 "IP-XACT" standard [@IP-XACT], which defines an XML format for documents describing the components, designs and configurations. The IP-XACT standard is complex and versatile, but Kactus2 hides most of the complexity and offers the users the easiest to use tool to accomplish IP-XACT related EDA tasks. In addition, Kactus2 includes extensions for SW components, SW on HW mapping and API abstraction, as well as physical product hierarchy including printed circuit board level. 

Kactus2 is a comprehensive toolset consisting of a core and plugins. The latter implement, for example, import from legacy code (e.g. Verilog, VHDL) and generation of code for HW synthesis and SW development. Current generators support Verilog, VHDL, HTML, PADS PCB parts as well as C Header and Memory map generators. Kactus2 is implemented in Qt5 cross-platform supporting Windows and Linux.

# References

