
                                README : Kactus2
           Copyright (c) 2012-2018 Tampere University of Technology
                            http://funbase.cs.tut.fi

Summary
----------------------------------------------------

Kactus2 is a toolset for designing embedded products, especially FPGA-based
MP-SoCs. The aim is easier IP reusabilility and integration for both hardware and
software. The tool is based on IEEE 1685-2014 "IP-XACT" standard.

Windows installer and tar-package for Linux are available at https://sourceforge.net/projects/kactus2/

Guidelines for issue reporting and contributing are given in CONTRIBUTING.md. Other support is provided by email: kactus2@cs.tut.fi

What you can do with Kactus2
----------------------------------------------------

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
 
What you cannot do with Kactus2
----------------------------------------------------
 * Behavioral logic: Neither Kactus2 nor IP-XACT handles module implementations
 * Synthesis or simulation: These require tools that are specificly created for the purpose
 
Examples and tutorials
----------------------------------------------------

Example IPs are available in GitHub at https://github.com/kactus2/ipxactexamplelib

Video tutorials are available in Youtube at https://www.youtube.com/user/Kactus2Tutorial

Windows install
----------------------------------------------------

An installer (link above) will guide you through the installation on Windows platforms.

If you want to build Kactus2 on Windows, see separate instructions in our project 
management site: https://kactus2.cs.tut.fi


Linux build and run
----------------------------------------------------

#### 1. Install Qt5

To build and run Kactus2, Qt5 must be installed on your system. If you already have Qt installed,
you can skip to step 2. Otherwise, select installing Qt with either using a package manager
(option a) or manually (option b):

a) Install Qt5 packages using a package manager (requires admin privileges).
The following packages are required (on Ubuntu):

  * qt5-default
  * qttools5-private-dev
  * qttools5-dev-tools

Example:
```
sudo apt-get install qt5-default qttools5-private-dev qttools5-dev-tools
```

b) Manually download and install from https://www.qt.io/download/

When using manually installed Qt5, you need to make sure that Kactus2 install is configured accordingly:
Open file configure in the Kactus2 root directory and set the path to Qt5 binaries in the 
variable QTBIN_PATH. 
  
Example: 
      
    QTBIN_PATH="~/Qt/5.7/gcc_64/bin/"
  
  Please do note the slash at the end of the path. 

#### 2. Build the sources

Get the Kactus2 source files. We recommend using the release tar-package (link above).
Extract the files and navigate to the Kactus2 root directory. 

Example:
```
mkdir ~/kactus2
tar -xvf kactus2-3.4.0.tar.gz -C ~/kactus2
cd ~/kactus2
```

Select installation for either all users (option a) or current user only (option b) below:

a) Installation for all users (requires admin privileges).

Open file .qmake.conf and check that paths are compatible with your Linux distribution.
The defaults work with Ubuntu 64-bit, but for instance, CentOS 7 64-bit requires changing lib_path from /usr/lib to /usr/lib64.
    
Now run following commands:
```
./configure
make
make install
```

b) A local installation for the current user.

  Open file .qmake.conf and set the installation directory in variable LOCAL_INSTALL_DIR.
  
Example:
  
    LOCAL_INSTALL_DIR="~/kactus2"
    
Now run the following commands:
```
./configure
make
make install
```

Note: If you do ./configure with wrong Qt binaries, you will have to delete the generated makefiles before configuring again. The easiest way to do this, is to run command `make distclean`.

#### 3. Run Kactus2

There are three ways to run Kactus2 depending on your system and installation setup.

a) An installation shared between users. Run:
  
    /usr/bin/Kactus2

b) A local installation from the installation directory. Run:
    
    LD_LIBRARY_PATH=. ./Kactus2

c) In some systems, a link to the executable is created if Kactus2 was installed for all users. Run:

    Kactus2

Community Guidelines & Contributions
----------------------------------------------------

See CONTRIBUTING.md for details.


Contributors
----------------------------------------------------

Antti Kamppi, Joni-Matti Määttä, Lauri Matilainen, Timo D. Hämäläinen,
Mikko Teuho, Juho Järvinen, Esko Pekkarinen, Janne Virtanen

Kactus2 is linked with Qt 5.2.0, copyright Digia Plc. (LGPL).

Licencing
----------------------------------------------------

This software is licensed under the GPL2 General Public License.

Kactus2 is also available for dual licensing. Please contact kactus2@cs.tut.fi
to purchase a commercial license.


Icons used in Kactus2
----------------------------------------------------

Icons8 by Icons8 LLC
https://icons8.com/
