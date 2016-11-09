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
date: 9 November 2016
bibliography: paper.bib
---

# Summary

Kactus2 is a graphical EDA tool targeted mainly for FPGA design, but is also applicable for documentation [@DATAM] and mapping software on hardware [@MCAPI], making it a more general embedded systems development tool. The tool is based on IP-XACT 2014 standard [@IP-XACT], which defines an XML format for documents describing the overall structure of components, designs, and interfaces between components. The scope of the standard does not include behaviour of the components, but the structural description nescessary for tool integration. Another significant dependency for Kactus2 is the Qt user interface library.

The tool uses the IP-XACT files as the way to store the documents as described in the standard, and most of the fields defined in the standard may be edited by the user. Some of the fields are edited more implicitly, such as interconnections within design diagrams, which are graphically drawn with the diagram editor within the tool. One of the intended benefits of Kactus2 is also the real-time feed back to user about potential design problems, such as mismatching interfaces between two connected components.

Kactus2 is also an expandable tool, as it supports 3rd party plugins, for instance, importer and generator for Verilog. Supported platforms include Windows and Linux.

# References

