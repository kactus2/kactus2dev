---
title: 'Kactus2: A graphical EDA tool built on the IP-XACT standard'
tags:
  - IP-XACT
  - EDA
  - Design
  - FPGA
  - Hardware
  - Intellectual Property
  - reuse
  - mpsoc
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

Kactus2 is a graphical electronic design automation (EDA) tool [@DEV] for packing and assembling building blocks for application-specific integrated circuits (ASIC) and field-programmable gate arrays (FPGA). Thus, the target audience is mostly hardware developers, but also co-operation with software development is enabled through software integration.

The core functionality of Kactus2 is creating and reusing intellectual property. This can be done for individual components as well as designs and hierarchies incorporating multiple component instances. In addition, the tool supports configuration for improving reusability. Furthermore, Kactus2 features integration support such as HDL generation and memory visualization.

Kactus2 is based on IEEE 1685-2014 "IP-XACT" standard [@IP-XACT], which defines an XML format for documents describing the components, designs and configurations. Ideally, this enables vendor independent integration between standard compatible tools. The IP-XACT standard is complex and versatile, but Kactus2 hides most of the complexity and offers the users the easiest to use tool to accomplish IP-XACT related EDA tasks. In addition, Kactus2 includes extensions for software components, software on hardware mapping and API abstraction, as well as physical product hierarchy including printed circuit board level. 

Structurally Kactus2 consists of a core and plugins. The latter implement, for example, import from legacy code (e.g. Verilog, VHDL) and generation of source code for hardware synthesis and software development. Current generators produce Verilog, VHDL, PADS PCB parts as well as C memory map headers. Kactus2 utilizes Qt5 user interface library. Supported platforms are Windows and Linux.

# References

