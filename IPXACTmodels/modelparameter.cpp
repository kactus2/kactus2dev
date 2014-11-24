//-----------------------------------------------------------------------------
// File: modelparameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 2.8.2010
//
// Description:
// Equals the spirit:modelParameter element in IP-Xact specification.
//-----------------------------------------------------------------------------

#include "modelparameter.h"

#include <QString>
#include <QDomNode>
#include <QMap>
#include <QObject>
#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: ModelParameter::ModelParameter()
//-----------------------------------------------------------------------------
ModelParameter::ModelParameter(QDomNode &modelParameterNode): Parameter(modelParameterNode)
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameter::ModelParameter()
//-----------------------------------------------------------------------------
ModelParameter::ModelParameter(): Parameter()
{

}


//-----------------------------------------------------------------------------
// Function: ModelParameter::ModelParameter()
//-----------------------------------------------------------------------------
ModelParameter::ModelParameter(const ModelParameter &other ): Parameter(other)
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameter::operator=()
//-----------------------------------------------------------------------------
ModelParameter & ModelParameter::operator=(const ModelParameter &other)
{
	if (this != &other)
    {
        Parameter::operator=(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::~ModelParameter()
//-----------------------------------------------------------------------------
ModelParameter::~ModelParameter()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameter::getDataType()
//-----------------------------------------------------------------------------
QString ModelParameter::getDataType() const
{
	return getAttribute("spirit:dataType");
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::setDataType()
//-----------------------------------------------------------------------------
void ModelParameter::setDataType( const QString& dataType )
{
	setAttribute("spirit:dataType", dataType);
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::getUsageType()
//-----------------------------------------------------------------------------
QString ModelParameter::getUsageType() const
{
	return getAttribute("spirit:usageType");
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::setUsageType()
//-----------------------------------------------------------------------------
void ModelParameter::setUsageType( const QString& usageType )
{
	setAttribute("spirit:usageType", usageType);
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::elementName()
//-----------------------------------------------------------------------------
QString ModelParameter::elementName() const
{
    return "model parameter";
}

//-----------------------------------------------------------------------------
// Function: ModelParameter::elementIdentifier()
//-----------------------------------------------------------------------------
QString ModelParameter::elementIdentifier() const
{
    return "spirit:modelParameter";
}
