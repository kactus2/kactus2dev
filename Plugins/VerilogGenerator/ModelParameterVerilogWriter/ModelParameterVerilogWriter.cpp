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
#include <kactusGenerators/HDLGenerator/HDLUtils.h>

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::ModelParameterVerilogWriter()
//-----------------------------------------------------------------------------
ModelParameterVerilogWriter::ModelParameterVerilogWriter(QSharedPointer<ModelParameter> modelParameter) :
modelParameter_(modelParameter)
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

    if (modelParameter_->getDescription().isEmpty())
    {
        output << endl;
    }
    else
    {
         output << " ";
         HDLUtils::writeDescription(modelParameter_->getDescription(), output, "//");
    }
}    

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::shouldNotWrite()
//-----------------------------------------------------------------------------
bool ModelParameterVerilogWriter::nothingToWrite() const
{
    return modelParameter_.isNull() || modelParameter_->getName().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::formatValue()
//-----------------------------------------------------------------------------
QString ModelParameterVerilogWriter::formattedValue() const
{
    QString value = modelParameter_->getValue();

    if (QString::compare(modelParameter_->getDataType(), "string") == 0)
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

//-----------------------------------------------------------------------------
// Function: ModelParameterVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString ModelParameterVerilogWriter::createDeclaration() const
{
    QString parameterDeclaration("parameter <type> <name> = <default>;");

    parameterDeclaration.replace("<type>", modelParameter_->getDataType());
    parameterDeclaration.replace("<name>", modelParameter_->getName());
    parameterDeclaration.replace("<default>", formattedValue());

    if (modelParameter_->getValue().isEmpty())
    {
        parameterDeclaration.remove(" = ");
    }

    return parameterDeclaration.simplified();
}