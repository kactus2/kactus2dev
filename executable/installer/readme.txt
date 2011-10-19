
===============================================================================
                                README : Kactus2
              Copyright (c) 2011 Tampere University of Technology
                            http://funbase.cs.tut.fi
===============================================================================

Summary
----------------------------------------------------

Kactus2 is a toolset to design embedded products, especially FPGA-based
MP-SoCs. We aim easier IP reusabilility and practical HW/SW abstraction for
easier application SW development. It is based on IEEE1685/IP-XACT XML metadata
and design methodology, but extends the IP-XACT usage to upper product
hierarchies and HW/SW abstraction with Multicore Association MCAPI. Kactus2 is
aimed to offer easy entrance to metadata based design methodology.


What you can do with Kactus2
----------------------------------------------------

Draft & Specify from scratch
 * Quickly draft block diagram blueprints for product boards (PCB), chips,
   system-on-chip, IPs and get them stored in IP-XACT format
 * Draft MCAPI endpoint design for all processors and fixed IPs in a product
 * For new IP-blocks generate code templates (VHDL entities, headers) from
   IP-XACT components defined in Kactus2

Packetize IP for reuse and exchange
 * Create "electronic datasheets" of your existing IPs for library as templates
   and blocks ready for integration
 * Import, export and integrity check IP libraries from any standard compatible
   IP vendor
 * Create MP-SoC products

Create HW designs with hierarchy
 * Create system designs that map SW to HW
 * Create SW architecture in MCAPI communication abstraction
 * Configure all designs
 * Generate everything ready for HDL synthesis and SW build for all processors


Contributors
----------------------------------------------------

Antti Kamppi, Joni-Matti M‰‰tt‰, Lauri Matilainen, Timo D. H‰m‰l‰inen,
Juho J‰rvinen