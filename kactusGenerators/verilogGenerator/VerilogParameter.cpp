//-----------------------------------------------------------------------------
// File: VerilogParameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// VerilogParameter represents a Verilog parameter in a component or entity declaration.
//-----------------------------------------------------------------------------

#include "VerilogParameter.h"

#include <IPXACTmodels/modelparameter.h>

//-----------------------------------------------------------------------------
// Function: VerilogParameter::VerilogParameter()
//-----------------------------------------------------------------------------
VerilogParameter::VerilogParameter(ModelParameter* modelParameter, QObject* parent):
HDLModelParameter(modelParameter, parent)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogParameter::~VerilogParameter()
//-----------------------------------------------------------------------------
VerilogParameter::~VerilogParameter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogParameter::write()
//-----------------------------------------------------------------------------
void VerilogParameter::write(QTextStream& stream) const
{
    if (name().isEmpty() || defaultValue().isEmpty())
    {           
        return;
    }

    stream << "parameter ";
    
    if (!type().isEmpty())
    {
        stream << type() << " ";
    }
    
    stream << name() <<  " = " << formattedValue() << ";";
}    

//-----------------------------------------------------------------------------
// Function: VerilogParameter::formatValue()
//-----------------------------------------------------------------------------
QString VerilogParameter::formattedValue() const
{
    QString value = defaultValue();

    if (QString::compare(type(), "string") == 0)
    {
        if (!value.startsWith("\""))
        {
            value.prepend("\"");
        }

        if (!value.endsWith("\""))
        {
            value.append("\"");
        }
    }

    return value;
}


