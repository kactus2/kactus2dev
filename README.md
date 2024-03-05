                                README : Kactus2
				   Copyright (c) 2012-2024 Tampere University
                  https://research.tuni.fi/system-on-chip/tools

## Summary
![Kactus2 logo](common/graphics/appicon.png)

Kactus2 is a toolset for designing SoCs. The aim is easier IP reusability and integration for both hardware and
software. The tool is based on IEEE 1685-2014 "IP-XACT" standard.

Windows installer and tar-package for Linux are available in 
[SourceForge](https://sourceforge.net/projects/kactus2/)

Guidelines for issue reporting and contributing are given in the wiki page [Contributing to Kactus2](https://github.com/kactus2/kactus2dev/wiki/Contributing-to-Kactus2).  

Other support is provided by email: kactus2@tuni.fi

### What you can do with Kactus2
Package IPs for reuse and exchange
 * Import your existing IPs as IP-XACT components
 * Create new IP-XACT components and generate their HDL module headers
 * Reuse IP-XACT files from any standard compatible vendor
 * Reuse the IPs in your designs and connect them with wires and busses

Create HW designs with hierarchy
 * Create multilevel hierarchies, where a design has multiple sub-designs
 * Configure component instances in designs, including the sub-designs
 * Use generator plugins to create HDL with wiring and parameterization

Integrate HW and SW
 * Use memory designer to preview memory maps and address spaces in your hierarchy
 * Package software to IP-XACT components and map them to hardware
 * Generate makefiles that build executables with rules defined in IP-XACT components
 
### What you cannot do with Kactus2
 * Behavioral logic: Neither Kactus2 nor IP-XACT handles module implementations
 * Synthesis or simulation: These require tools that are specificly created for the purpose
 
### Examples and tutorials
Example IPs are available in GitHub [here](https://github.com/kactus2/ipxactexamplelib).

Video tutorials are available in [Youtube](https://www.youtube.com/user/Kactus2Tutorial).

## Windows install
An installer (SourceForge link above) will guide you through the installation on Windows platforms.

If you want to build Kactus2 on Windows, see separate instructions in our project 
[wiki](https://github.com/kactus2/kactus2dev/wiki/Building-Kactus2-from-source#building-on-windows).

## Linux install

On Linux systems Kactus2 must be built from source. Instructions can be found in the 
[wiki](https://github.com/kactus2/kactus2dev/wiki/Building-Kactus2-from-source#linux-build-install-and-run).

## Settings and configurations

Kactus2 uses a settings file for storing user-specific tool settings. The file default location
is **~/.config/TUT/Kactus2.ini** in Linux and **C:\Users\\\<username>\AppData\Roaming\TUT\Kactus2.ini**
in Windows. The location can be checked on the general settings page in Kactus2.

A system wide default settings file is located in /etc/xdg and C:\ProgramData, respectively.
This will be used as a base for any new user-specific settings and a fallback mechanism, if
the user's file is missing a requested value.

Modifying the settings files manually is not recommended and should be done only by advanced 
users. Most of the time all required changes can be applied in the Kactus2 GUI.

In addition, Kactus2 uses the file **configure.cfg**for updating the users' settings when
new setting options are introduced in the tool. Please do not modify this file as it will
break the compatibility with earlier versions.

## Contributors
Antti Kamppi, Joni-Matti Määttä, Lauri Matilainen, Timo D. Hämäläinen,
Mikko Teuho, Juho Järvinen, Esko Pekkarinen, Janne Virtanen,
Anton Hagqvist

Kactus2 uses [Icons8](https://icons8.com/) provided by Icons8 LLC.

## Licensing
This software is licensed under the [GPL2 General Public License](LICENSE).

Kactus2 is also available for dual licensing. Please contact kactus2@tuni.fi
to purchase a commercial license.

Kactus2 is dynamically linked using Qt 6.2.4 open source libraries (LGPL),
copyright The Qt Company.

Kactus2 uses the Python language and interpreter for automating design tasks.  
Python is a registered trademark of the Python Software Foundation. See also the
[PSF license](PSF_LICENSE.txt).
