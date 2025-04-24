/* File : pythonAPI.i */

//%feature("autodoc", "2");
%module pythonAPI

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "InterfaceFiles/QString.i"
%include "InterfaceFiles/NameGroup.i"

%include <windows.i>
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)

%include "../KactusAPI/KactusAPIGlobal.h"

%{
#include "../KactusAPI/KactusAPIGlobal.h"
#include "PythonAPI.h"
#include "../KactusAPI/include/CommonInterface.h"
#include "../KactusAPI/include/NameGroupInterface.h"
#include "../KactusAPI/include/ParameterizableInterface.h"
#include "../KactusAPI/include/MasterPortInterface.h"
#include "../KactusAPI/include/PortsInterface.h"
#include "../KactusAPI/include/AbstractParameterInterface.h"
#include "../KactusAPI/include/ParametersInterface.h"
#include "../KactusAPI/include/MemoryMapInterface.h"
#include "../KactusAPI/include/MemoryBlockInterface.h"
#include "../KactusAPI/include/SubspaceMapInterface.h"
#include "../KactusAPI/include/AddressBlockInterface.h"
#include "../KactusAPI/include/RegisterInterface.h"
#include "../KactusAPI/include/FieldInterface.h"
#include "../KactusAPI/include/ResetInterface.h"
#include "../KactusAPI/include/FileSetInterface.h"
#include "../KactusAPI/include/FileInterface.h"
#include "../KactusAPI/include/FileBuilderInterface.h"
#include "../KactusAPI/include/AbstractionTypeInterface.h"
#include "../KactusAPI/include/PortAbstractionInterface.h"
#include "../KactusAPI/include/PortMapInterface.h"
#include "../KactusAPI/include/BusInterfaceInterface.h"
%}


namespace std {
//	%template(portsVector) vector<SimplePort*>;
	%template(stringVector) vector<string>;	
//	%template(fieldInterfaceVector) vector<FieldInterface*>;
//	%template(resetInterfaceVector) vector<ResetInterface*>;
//	%template(registerInterfaceVector) vector<RegisterInterface*>;
};

/* Let's just grab the original header file here */
%include "PythonAPI.h"
%include "../KactusAPI/include/CommonInterface.h"
%include "../KactusAPI/include/NameGroupInterface.h"
%include "../KactusAPI/include/ParameterizableInterface.h"
%include "../KactusAPI/include/MasterPortInterface.h"
%include "../KactusAPI/include/PortsInterface.h"
%include "../KactusAPI/include/AbstractParameterInterface.h"
%include "../KactusAPI/include/ParametersInterface.h"
%include "../KactusAPI/include/MemoryMapInterface.h"
%include "../KactusAPI/include/MemoryBlockInterface.h"
%include "../KactusAPI/include/SubspaceMapInterface.h"
%include "../KactusAPI/include/AddressBlockInterface.h"
%include "../KactusAPI/include/RegisterInterface.h"
%include "../KactusAPI/include/FieldInterface.h"
%include "../KactusAPI/include/ResetInterface.h"
%include "../KactusAPI/include/FileSetInterface.h"
%include "../KactusAPI/include/FileInterface.h"
%include "../KactusAPI/include/FileBuilderInterface.h"
%include "../KactusAPI/include/AbstractionTypeInterface.h"
%include "../KactusAPI/include/PortAbstractionInterface.h"
%include "../KactusAPI/include/PortMapInterface.h"
%include "../KactusAPI/include/BusInterfaceInterface.h"

// Workaround fix for calling AddressBlockInterface/SubspaceMapInterface::addBlock(std::string const&) from python
%extend AddressBlockInterface {
    void addBlock(std::string const& newBlockName = std::string("")) {
        return static_cast<MemoryBlockInterface*>(self)->addBlock(newBlockName);
    }
}

%extend SubspaceMapInterface {
    void addBlock(std::string const& newBlockName = std::string("")) {
        return static_cast<MemoryBlockInterface*>(self)->addBlock(newBlockName);
    }
}