/* File : pythonAPI.i */
%module pythonAPI

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "InterfaceFiles\QString.i"
%include "InterfaceFiles\NameGroup.i"

%{
#include "PythonAPI.h"
#include "..\editors\ComponentEditor\common\ParameterizableInterface.h"
#include "..\editors\ComponentEditor\ports\PortsInterface.h"
#include "..\editors\ComponentEditor\parameters\ParametersInterface.h"
#include "..\editors\ComponentEditor\memoryMaps\FieldInterface.h"
%}


namespace std {
//	%template(portsVector) vector<SimplePort*>;
	%template(stringVector) vector<string>;
	%template(fieldInterfaceVector) vector<FieldInterface*>;
};

/* Let's just grab the original header file here */
%include "PythonAPI.h"
%include "..\editors\ComponentEditor\common\ParameterizableInterface.h"
%include "..\editors\ComponentEditor\ports\PortsInterface.h"
%include "..\editors\ComponentEditor\parameters\ParametersInterface.h"
%include "..\editors\ComponentEditor\memoryMaps\FieldInterface.h"
