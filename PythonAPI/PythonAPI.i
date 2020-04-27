/* File : pythonAPI.i */
%module pythonAPI

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "InterfaceFiles\QString.i"
%include "InterfaceFiles\NameGroup.i"

%{
#include "PythonAPI.h"
#include "..\editors\ComponentEditor\common\interfaces\CommonInterface.h"
#include "..\editors\ComponentEditor\common\interfaces\NameGroupInterface.h"
#include "..\editors\ComponentEditor\common\interfaces\ParameterizableInterface.h"
#include "..\editors\ComponentEditor\ports\PortsInterface.h"
#include "..\editors\ComponentEditor\parameters\ParametersInterface.h"
#include "..\editors\ComponentEditor\memoryMaps\interfaces\FieldInterface.h"
#include "..\editors\ComponentEditor\memoryMaps\interfaces\ResetInterface.h"
%}


namespace std {
//	%template(portsVector) vector<SimplePort*>;
	%template(stringVector) vector<string>;
	%template(fieldInterfaceVector) vector<FieldInterface*>;
	%template(resetInterfaceVector) vector<ResetInterface*>;
};

/* Let's just grab the original header file here */
%include "PythonAPI.h"
%include "..\editors\ComponentEditor\common\interfaces\CommonInterface.h"
%include "..\editors\ComponentEditor\common\interfaces\NameGroupInterface.h"
%include "..\editors\ComponentEditor\common\interfaces\ParameterizableInterface.h"
%include "..\editors\ComponentEditor\ports\PortsInterface.h"
%include "..\editors\ComponentEditor\parameters\ParametersInterface.h"
%include "..\editors\ComponentEditor\memoryMaps\interfaces\FieldInterface.h"
%include "..\editors\ComponentEditor\memoryMaps\interfaces\ResetInterface.h"
