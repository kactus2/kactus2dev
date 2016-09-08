//-----------------------------------------------------------------------------
// File: VerilogInterconnectionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 07.09.2014
//
// Description:
// Class for writing a Verilog interconnection declaration.
//-----------------------------------------------------------------------------

#include "VerilogInterconnectionWriter.h"

//-----------------------------------------------------------------------------
// Function: VerilogInterconnectionWriter::InterconnectionVerilogWriter()
//-----------------------------------------------------------------------------
VerilogInterconnectionWriter::VerilogInterconnectionWriter(QSharedPointer<GenerationInterconnection> interconnection) : interconnection_(interconnection)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogInterconnectionWriter::~InterconnectionVerilogWriter()
//-----------------------------------------------------------------------------
VerilogInterconnectionWriter::~VerilogInterconnectionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogInterconnectionWriter::write()
//-----------------------------------------------------------------------------
void VerilogInterconnectionWriter::write(QTextStream& output) const
{
    QString instanceString = "<typeName> <parameters><instanceName>();";

    instanceString.replace("<typeName>", interconnection_->typeName);
    instanceString.replace("<parameters>", parameterAssignments());
    instanceString.replace("<instanceName>", interconnection_->name);

    output<< indentation() << instanceString << endl;
}

//-----------------------------------------------------------------------------
// Function: VerilogInterconnectionWriter::indentation()
//-----------------------------------------------------------------------------
QString VerilogInterconnectionWriter::indentation() const
{
    return "    ";
}

//-----------------------------------------------------------------------------
// Function: VerilogInterconnectionWriter::parameterAssignments()
//-----------------------------------------------------------------------------
QString VerilogInterconnectionWriter::parameterAssignments() const
{
    return "";
    /*QString instanceParameters("#(\n<namesAndValues>)\n");

    QStringList assignments;

    if (instance_->parameters.count() < 1)
    {
        return "";
    }

    foreach(QSharedPointer<Parameter> parameter, instance_->parameters)
    {
        // If the parameters are not user resolved nor generated, then there cannot be any override by the design.
        if (parameter->getValueResolve() != "user" && parameter->getValueResolve() != "generated")
        {
            continue;
        }

        QString assignment(indentation().repeated(2) + ".<parameter>(<value>)");
        assignment.replace("<parameter>", 
            expressionParser_->parseExpression(parameter->name()).leftJustified(20));
        assignment.replace("<value>", 
            expressionFormatter_->formatReferringExpression(parameter->getValue()));
        assignments.append(assignment);
    }

    instanceParameters.replace("<namesAndValues>", assignments.join(",\n"));
    return instanceParameters;*/
}