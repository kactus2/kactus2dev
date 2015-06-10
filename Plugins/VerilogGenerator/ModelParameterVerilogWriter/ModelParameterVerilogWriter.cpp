//-----------------------------------------------------------------------------
// File: ModelParameterVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Class for writing model parameter declarations in Verilog.
//-----------------------------------------------------------------------------

#include "ModelParameterVerilogWriter.h"

#include <IPXACTmodels/modelparameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::ModelParameterVerilogWriter()
//-----------------------------------------------------------------------------
ModelParameterVerilogWriter::ModelParameterVerilogWriter(QSharedPointer<ModelParameter> modelParameter,
    QSharedPointer<ExpressionFormatter> formatter) :
modelParameter_(modelParameter),
formatter_(formatter)
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::~ModelParameterVerilogWriter()
//-----------------------------------------------------------------------------
ModelParameterVerilogWriter::~ModelParameterVerilogWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::write()
//-----------------------------------------------------------------------------
void ModelParameterVerilogWriter::write(QTextStream& output) const
{
    if (nothingToWrite())
    {
        return;
    }

    output << createDeclaration(); 
}    

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::shouldNotWrite()
//-----------------------------------------------------------------------------
bool ModelParameterVerilogWriter::nothingToWrite() const
{
    return modelParameter_.isNull() || modelParameter_->getName().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString ModelParameterVerilogWriter::createDeclaration() const
{
    QString parameterDeclaration("parameter <type> <arrayBounds> <name> = <default>");

    parameterDeclaration.replace("<type>", modelParameter_->getDataType().leftJustified(7));
    parameterDeclaration.replace("<arrayBounds>", arrayBounds().leftJustified(20));
    //parameterDeclaration.replace("<vectorBounds>", vectorBounds());
    parameterDeclaration.replace("<name>", modelParameter_->getName().leftJustified(16));
    parameterDeclaration.replace("<default>", formattedValue());

    if (modelParameter_->getValue().isEmpty())
    {
        parameterDeclaration.remove(" = ");
    }

    return parameterDeclaration;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::arrayBounds()
//-----------------------------------------------------------------------------
QString ModelParameterVerilogWriter::arrayBounds() const
{
    QString arrayLeft = modelParameter_->getAttribute("kactus2:arrayLeft");
    QString arrayRight = modelParameter_->getAttribute("kactus2:arrayRight");

    QString arrayDefinition = "[" + arrayLeft + ":" + arrayRight + "]";
    arrayDefinition.remove(" ");

    if (arrayDefinition == "[:]")
    {
        arrayDefinition.clear();
    }

    QString vectorLeft = modelParameter_->getBitWidthLeft();
    QString vectorRight = modelParameter_->getBitWidthRight();

    QString vectorDefinition = "[" + vectorLeft + ":" + vectorRight + "]";
    vectorDefinition.remove(" ");

    if (vectorDefinition == "[:]")
    {
        vectorDefinition.clear();
    }

    return formatter_->formatReferringExpression(arrayDefinition + vectorDefinition);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::formattedValue()
//-----------------------------------------------------------------------------
QString ModelParameterVerilogWriter::formattedValue() const
{
    QString value = modelParameter_->getValue();

    if (QString::compare(modelParameter_->getDataType(), "string") == 0)
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
