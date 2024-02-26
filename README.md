                                README : Kactus2
				   Copyright (c) 2012-2024 Tampere University
                  https://research.tuni.fi/system-on-chip/tools

## Summary
![Kactus2 logo](common/graphics/appicon.png)

Kactus2 is a toolset for designing SoCs. The aim is easier IP reusability and integration for both hardware and
software. The tool is based on IEEE 1685-2014 "IP-XACT" standard.

Windows installer and tar-package for Linux are available in 
[SourceForge](https://sourceforge.net/projects/kactus2/)

Guidelines for issue reporting and contributing are given in [CONTRIBUTING.md](CONTRIBUTING.md).
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
An installer (link above) will guide you through the installation on Windows platforms.

If you want to build Kactus2 on Windows, see separate instructions in our project 
management site: https://kactus2.cs.tut.fi

## Linux build, install and run

### 1. Install Qt6

To build and run Kactus2, Qt6 must be installed on your system. If you already have Qt installed,
you can skip to step 2. Otherwise, select installing Qt with either using a package manager
(option a) or manually (option b):

a) Install Qt6 packages using a package manager (requires admin privileges).
The following packages are required (on Ubuntu 22.04 LTS):

  * qt6-base-dev
  * qt6-tools-dev
  * qt6-tools-dev-tools
  * libqt6svg6-dev

Install other dependencies:
  * libpython3.8-dev or newer
  * swig, version 3.0.12 or newer
  * libgl-dev

Example with all dependencies:
```
sudo apt-get install qt6-base-dev qt6-tools-dev qt6-tools-dev-tools libqt6svg6-dev libpython3.8-dev swig libgl-dev
```

b) Manually download and install from https://www.qt.io/download/

### 2. Build the sources

First, get the Kactus2 source files. We recommend using the release tar-package (link above)
which has all Windows-specific files removed. Extract the files and navigate to the 
Kactus2 root directory. 

Example:
```
mkdir ~/kactus2
tar -xvf kactus2-3.11.0.tar.gz -C ~/kactus2
cd ~/kactus2
```

#### Build configuration 

Open the file **configure** in the Kactus2 root directory and set the path to Qt6 binaries in the 
variable **QTBIN_PATH**. 
  
Example: 
 ```     
QTBIN_PATH="/lib/qt6/bin/"
 ```

Please do note the slash at the end of the path. 

Open the file **.qmake.conf** and check that the paths are compatible with your Linux distribution.
The defaults should work with Ubuntu 64-bit, Debian and CentOS 7 64-bit, but on some systems
you may have to change **lib_path** from /usr/lib to /usr/lib64.
Next, check that the **PYTHON_CONFIG** variable has been set correctly and matches the installed
Python version.

Example for Python version 3.10:
```
PYTHON_CONFIG=python3.10-config
```

Now run the following command to apply the configuration:
```
./configure
```

Please note, if you run `./configure` with the wrong configuration, you will have to delete
the generated  makefiles before configuring again. The easiest way to do this, is to run 
command `make distclean`.

#### Build and install

Select installation either for all users (option a) or customized target (option b) below:

a) Default installation for all users (requires admin privileges).

Now run following commands:
```
make
sudo make install
```

After the installation, skip to section 3. 

b) Customized installation target. This option can be used for a local installation for the 
current user (e.g. in ~/kactus2) as well as shared installation in a specific directory 
(e.g. /opt/edatools/kactus2).

Open file **.qmake.conf** and set the installation directory in variable **LOCAL_INSTALL_DIR**.
  
Example:
```
LOCAL_INSTALL_DIR="~/kactus2"
```

By default, the shared libraries are installed in the same directory, but can be configured
by setting the **lib_path** on line 60.
  
Now run the following commands:
```
make
make install
```

Run `sudo make install`, if the current user has no write permissions to the target directory 
(e.g. /opt/edatools/kactus2).

#### Shared library setup

Finally, make sure the shared libraries can be found by the program loader. If the installation
is shared between multiple users, consider listing the libraries in /etc/ld.so.conf. 

Example for Ubuntu 22.04 LTS:
Create the file **/etc/ld.so.conf.d/kactus2.conf** and add a single line that contains the target 
installation path (e.g. /opt/edatools/kactus2). Run `ldconfig` to update the paths in the loader.

### 3a. Run Kactus2 GUI

There are three ways to run Kactus2 depending on your system and installation setup.

a) A default installation shared between users. Run:

```  
/usr/bin/kactus2
```

b) A local installation from the installation directory. Run:

```
LD_LIBRARY_PATH=. ./kactus2
```

If the library paths are set in /etc/ld.so.conf, the binary can be run directly.
Example:

```
/opt/edatools/kactus2
```

c) In some systems, a link to the executable is created, if Kactus2 was installed for all users. Run:

```
kactus2
```

### 3b. Run Kactus2 on command-line

Kactus2 can be run in the command-line without the GUI. Run:

```
kactus2 -c
```

This will start an interactive Python® interpreter for executing series of commands. 

Kactus2 supports the following command-line options:
* `-c, --no-gui`: Run in command-line mode without the GUI.
* `-h, --help`: Show the application usage help and exit.
* `-v, --version`: Show the application version and exit.

On command-line the IP-XACT data is readable and modifiable through the provided interafaces.
Interface documentation will be added later. Example script files can be found in 
PythonAPI/ExampleScripts directory. 
The API module is imported at startup and instantiated as `kactus2`. 

Example usage showing importing a verilog file to IP-XACT component and generating an equivalent VHDL file: 
```
$kactus2 -c
...
Python 3.8.3 (tags/v3.8.3:6f8c832, May 13 2020, 22:20:16) [MSC v.1925 64 bit (AMD64)]
>>> kactus2.importFile('D:/Data/ipxactexamplelib/wb_sum_buffer.v', 'tuni.fi:peripheral.logic:wb_sum_buffer:1.0')
1
>>> kactus2.generate('VHDL', 'tuni.fi:peripheral.logic:wb_sum_buffer:1.0', 'flat_verilog', 'D:/Data/ipxactexamplelib/generated')
Running VHDL Generator 1.3.
Running generation for tuni.fi:peripheral.logic:wb_sum_buffer:1.0 and view 'flat_verilog'.
Target directory: D:/Data/ipxactexamplelib/generated
Parsing the IP-Xact models...
Writing the vhdl file...
Done writing the vhdl file.
VHDL generation complete.
>>> exit()
```

The Python interface can be used without running Kactus2 by importing the pythonAPI module and instantiating the API:
```
Python 3.8.3 (tags/v3.8.3:6f8c832, May 13 2020, 22:20:16) [MSC v.1925 64 bit (AMD64)]
>>> import pythonAPI
>>> kactus2 = pythonAPI.PythonAPI()
...
```

Please note the following when using the module independent of Kactus2:
* The API is dependent on a shared library named KactusAPI. It must be found at import time.
* Kactus2 settings, such as library paths, are not loaded with the module. You must set them manually. An example is shown below:
```
>>> kactus2.setLibraryPaths(['/home/me/ipxact', '/home/me/ip/uart/'])
```

### Troubleshooting for Python API:

```
sh: 1: python3-config: not found
```
Running `./configure` gives this error when the Python 3 configuration utilities
cannot be found. Set the variable **PYTHON_CONFIG** in **.qmake.conf**.

```
Could not import Kactus2 PythonAPI.
ModuleNotFoundError: No module named 'pythonAPI'
```
The Python module (pythonAPI, no underscore prefix) for the API could not be found. Make sure 
the directory PythonAPI is in your PYTHONPATH environment variable. For non-persistent 
setup you can set PYTHONPATH at Kactus2 startup by running e.g. 
`PYTHONPATH=~/kactus2/PythonAPI kactus2 -c`


```
ModuleNotFoundError: No module named '_pythonAPI'
```
File _pythonAPI.so (with underscore) could not be found. Check that PythonAPI project was 
compiled succesfully and libPythonAPI.so was created in the PythonAPI directory. 
SWIG requires the library name to match _pythonAPI.so, so create the link manually with 
`ln -s -f PythonAPI/libPythonAPI.so.1.0.0 PythonAPI/_pythonAPI.so`


```
ImportError: libKactus2.so: cannot open shared object file: No such file or directory
```
File libKactus2.so could not be found. Check that Kactus2 project was compiled succesfully
and libKactus2.so was created in /usr/lib, /usr/lib64 or your local install directory. Running
`make install` copies the .so file in the target install directory. Also make sure the
library can be found run-time. See the end of section 2 for details on library path
loading.


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

## Community Guidelines & Contributions
Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details.

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
