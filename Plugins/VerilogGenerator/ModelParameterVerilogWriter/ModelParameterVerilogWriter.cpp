//-----------------------------------------------------------------------------
// File: ModuleParameterVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Class for writing model parameter declarations in Verilog.
//-----------------------------------------------------------------------------

#include "ModelParameterVerilogWriter.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::ModuleParameterVerilogWriter()
//-----------------------------------------------------------------------------
ModuleParameterVerilogWriter::ModuleParameterVerilogWriter(QSharedPointer<Parameter> parameter) :
parameter_(parameter)
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::~ModuleParameterVerilogWriter()
//-----------------------------------------------------------------------------
ModuleParameterVerilogWriter::~ModuleParameterVerilogWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::write()
//-----------------------------------------------------------------------------
void ModuleParameterVerilogWriter::write(QTextStream& output) const
{
    if (nothingToWrite())
    {
        return;
    }

    output << createDeclaration(); 
}    

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::shouldNotWrite()
//-----------------------------------------------------------------------------
bool ModuleParameterVerilogWriter::nothingToWrite() const
{
    return parameter_.isNull() || parameter_->name().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString ModuleParameterVerilogWriter::createDeclaration() const
{
    QString parameterDeclaration("parameter <type> <arrayBounds> <name> = <default>");

    parameterDeclaration.replace("<type>", parameter_->getType().leftJustified(7));
    parameterDeclaration.replace("<arrayBounds>", arrayBounds().leftJustified(20));
    //parameterDeclaration.replace("<vectorBounds>", vectorBounds());
    parameterDeclaration.replace("<name>", parameter_->name().leftJustified(16));
    parameterDeclaration.replace("<default>", formattedValue());

    if (parameter_->getValue().isEmpty())
    {
        parameterDeclaration.remove(" = ");
    }

    return parameterDeclaration;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::arrayBounds()
//-----------------------------------------------------------------------------
QString ModuleParameterVerilogWriter::arrayBounds() const
{
    QString arrayLeft = parameter_->getAttribute("kactus2:arrayLeft");
    QString arrayRight = parameter_->getAttribute("kactus2:arrayRight");

    QString arrayDefinition = "[" + arrayLeft + ":" + arrayRight + "]";
    arrayDefinition.remove(" ");

    if (arrayDefinition == "[:]")
    {
        arrayDefinition.clear();
    }

    QString vectorLeft = parameter_->getVectorLeft();
    QString vectorRight = parameter_->getVectorRight();

    QString vectorDefinition = "[" + vectorLeft + ":" + vectorRight + "]";
    vectorDefinition.remove(" ");

    if (vectorDefinition == "[:]")
    {
        vectorDefinition.clear();
    }

    return arrayDefinition + vectorDefinition;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::formattedValue()
//-----------------------------------------------------------------------------
QString ModuleParameterVerilogWriter::formattedValue() const
{
    QString value = parameter_->getValue();

    if (QString::compare(parameter_->getType(), "string") == 0)
    {
        QRegularExpression expression("\".*\"");
        QRegularExpressionMatch expressionMatch = expression.match(value);

        if (expressionMatch.hasMatch())
        {
            value = expressionMatch.captured(0);
        }

        if (!value.startsWith("\""))
        {
            value.prepend("\"");
        }

        if (!value.endsWith("\""))
        {
            value.append("\"");
        }
    }

    else
    {
        value.remove(" ");
    }

    return value;
}
