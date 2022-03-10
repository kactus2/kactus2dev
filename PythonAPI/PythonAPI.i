/* File : pythonAPI.i */

%feature("autodoc", "3");
%module pythonAPI

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "InterfaceFiles/QString.i"
%include "InterfaceFiles/NameGroup.i"

%include <windows.i>
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)

%include "../common/Global.h"

%{
#include "../common/Global.h"
#include "PythonAPI.h"
#include "../editors/ComponentEditor/common/interfaces/CommonInterface.h"
#include "../editors/ComponentEditor/common/interfaces/NameGroupInterface.h"
#include "../editors/ComponentEditor/common/interfaces/ParameterizableInterface.h"
#include "../editors/ComponentEditor/ports/interfaces/MasterPortInterface.h"
#include "../editors/ComponentEditor/ports/interfaces/PortsInterface.h"
#include "../editors/ComponentEditor/parameters/AbstractParameterInterface.h"
#include "../editors/ComponentEditor/parameters/ParametersInterface.h"
#include "../editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h"
#include "../editors/ComponentEditor/memoryMaps/interfaces/MemoryBlockInterface.h"
#include "../editors/ComponentEditor/memoryMaps/interfaces/SubspaceMapInterface.h"
#include "../editors/ComponentEditor/memoryMaps/interfaces/AddressBlockInterface.h"
#include "../editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h"
#include "../editors/ComponentEditor/memoryMaps/interfaces/FieldInterface.h"
#include "../editors/ComponentEditor/memoryMaps/interfaces/ResetInterface.h"
#include "../editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h"
#include "../editors/ComponentEditor/fileSet/interfaces/FileInterface.h"
#include "../editors/ComponentEditor/fileSet/interfaces/FileBuilderInterface.h"
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
%include "../editors/ComponentEditor/common/interfaces/CommonInterface.h"
%include "../editors/ComponentEditor/common/interfaces/NameGroupInterface.h"
%include "../editors/ComponentEditor/common/interfaces/ParameterizableInterface.h"
%include "../editors/ComponentEditor/ports/interfaces/MasterPortInterface.h"
%include "../editors/ComponentEditor/ports/interfaces/PortsInterface.h"
%include "../editors/ComponentEditor/parameters/AbstractParameterInterface.h"
%include "../editors/ComponentEditor/parameters/ParametersInterface.h"
%include "../editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h"
%include "../editors/ComponentEditor/memoryMaps/interfaces/MemoryBlockInterface.h"
%include "../editors/ComponentEditor/memoryMaps/interfaces/SubspaceMapInterface.h"
%include "../editors/ComponentEditor/memoryMaps/interfaces/AddressBlockInterface.h"
%include "../editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h"
%include "../editors/ComponentEditor/memoryMaps/interfaces/FieldInterface.h"
%include "../editors/ComponentEditor/memoryMaps/interfaces/ResetInterface.h"
%include "../editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h"
%include "../editors/ComponentEditor/fileSet/interfaces/FileInterface.h"
%include "../editors/ComponentEditor/fileSet/interfaces/FileBuilderInterface.h"
