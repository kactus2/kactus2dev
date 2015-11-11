//-----------------------------------------------------------------------------
// File: ModuleParameterVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Class for writing model parameter declarations in Verilog.
//-----------------------------------------------------------------------------

#include "ModelParameterVerilogWriter.h"

#include <IPXACTmodels/common/ModuleParameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::ModuleParameterVerilogWriter()
//-----------------------------------------------------------------------------
ModuleParameterVerilogWriter::ModuleParameterVerilogWriter(QSharedPointer<ModuleParameter> ModuleParameter,
    QSharedPointer<ExpressionFormatter> formatter) :
moduleParameter_(ModuleParameter),
formatter_(formatter)
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
    return moduleParameter_.isNull() || moduleParameter_->name().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString ModuleParameterVerilogWriter::createDeclaration() const
{
    QString parameterDeclaration("parameter <type> <arrayBounds> <name> = <default>");

    parameterDeclaration.replace("<type>", moduleParameter_->getDataType().leftJustified(7));
    parameterDeclaration.replace("<arrayBounds>", arrayBounds().leftJustified(20));
    //parameterDeclaration.replace("<vectorBounds>", vectorBounds());
    parameterDeclaration.replace("<name>", moduleParameter_->name().leftJustified(16));
    parameterDeclaration.replace("<default>", formattedValue());

    if (moduleParameter_->getValue().isEmpty())
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
    QString arrayLeft = moduleParameter_->getAttribute("kactus2:arrayLeft");
    QString arrayRight = moduleParameter_->getAttribute("kactus2:arrayRight");

    QString arrayDefinition = "[" + arrayLeft + ":" + arrayRight + "]";
    arrayDefinition.remove(" ");

    if (arrayDefinition == "[:]")
    {
        arrayDefinition.clear();
    }

    QString vectorLeft = moduleParameter_->getVectorLeft();
    QString vectorRight = moduleParameter_->getVectorRight();

    QString vectorDefinition = "[" + vectorLeft + ":" + vectorRight + "]";
    vectorDefinition.remove(" ");

    if (vectorDefinition == "[:]")
    {
        vectorDefinition.clear();
    }

    return formatter_->formatReferringExpression(arrayDefinition + vectorDefinition);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterVerilogWriter::formattedValue()
//-----------------------------------------------------------------------------
QString ModuleParameterVerilogWriter::formattedValue() const
{
    QString value = moduleParameter_->getValue();

    if (QString::compare(moduleParameter_->getDataType(), "string") == 0)
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

    QString formatted = formatter_->formatReferringExpression(value);

    return formatted;
}
