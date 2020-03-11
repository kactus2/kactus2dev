/* File : pythonAPI.i */
%module pythonAPI

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "InterfaceFiles\QString.i"
%include "InterfaceFiles\NameGroup.i"

%{
#include "PythonAPI.h"
#include "simpleModels\SimpleNameGroup.h"
#include "simpleModels\SimplePort.h"
%}


namespace std {
	%template(portsVector) vector<SimplePort*>;
};

/* Let's just grab the original header file here */
%include "PythonAPI.h"
%include "simpleModels\SimpleNameGroup.h"
%include "simpleModels\SimplePort.h"
