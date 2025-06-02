# Kactus2 reference guide {#home}

Welcome to the Kactus2 reference page. Here you can find the API reference for the PythonAPI as well as the reference for the supporting KactusAPI module.

## PythonAPI

The PythonAPI is a Python module that enables scripting support for Kactus2 using Python. The library uses SWIG to wrap the PythonAPI C++ class into a Python module. This reference guide contains the C++ descriptions for the PythonAPI class and its methods.

The PythonAPI contains classes and methods to manipulate Kactus2 IP-XACT objects. Everything is designed to be accessed through the PythonAPI class, which contains methods to access the underlying KactusAPI interfaces, which in turn do the heavy lifting in interfacing with the IP-XACT models. The SWIG wrapper isn't perfect and thus some KactusAPI interfaces wrapped for Python might contain methods that are not meant to be called from a Python script. For example, any functions either returning Qt-types or taking them as paremeters should not be called using Python.

The PythonAPI currently contains the following functionality:
- Creating and modifying components (including component memory maps, address blocks, etc., as well as bus interfaces)
- Creating and modifying designs (including adding/removing component instances and connections)
- Launching generators and importers

### PythonAPI usage

PythonAPI is not a standalone module and must be used in tandem with Kactus2 either inside the Kactus2 GUI in the "Script editor" window or on the command line Python interpreter by starting Kactus2 with the -c (--no-gui) option. Running ready-made scripts on the CLI can be done with `exec(open(<file path>).read())`.

### Usable KactusAPI interfaces

The PythonAPI automatically initializes and provides methods to access the following KactusAPI interfaces, which can be used to modify component sub-elements:

- PortsInterface for modifying component ports
- ParametersInterface for modifying component parameters
- MemoryMapInterface for modifying component memory maps
- FileSetInterface for modifying component file sets
- BusInterfaceInterface for modifying component bus interfaces

Above interfaces may contain sub-interfaces that give further access to sub-elements, for example the address blocks for a memory map can be accessed via the AddressBlockInterface which in turn can be accessed with MemoryMapInterface::getAddressBlockInterface. Some sub-interfaces may need additional configuration before use, examples of this can be found in the [scripting examples](#examples).
