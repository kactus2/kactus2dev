//-----------------------------------------------------------------------------
// File: EditorConfigurableElement.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.08.2017
//
// Description:
// Container class for configurable elements in the configurable element value tables.
//-----------------------------------------------------------------------------

#include "EditorConfigurableElement.h"

#include <IPXACTmodels/common/Parameter.h>

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::EditorConfigurableElement()
//-----------------------------------------------------------------------------
EditorConfigurableElement::EditorConfigurableElement(QSharedPointer<Parameter> referencedParameter,
    QString const& evaluatedDefaultValue, QString const& configuratedValue):
referencedParameter_(referencedParameter),
evaluatedDefaultValue_(evaluatedDefaultValue),
configuratedValue_(configuratedValue),
type_("Parameter")
{

}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::getReferencedParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> EditorConfigurableElement::getReferencedParameter() const
{
    return referencedParameter_;
}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::setParameterReference()
//-----------------------------------------------------------------------------
void EditorConfigurableElement::setParameterReference(QSharedPointer<Parameter> parameter)
{
    referencedParameter_ = parameter;
}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::getEvaluatedDefaultValue()
//-----------------------------------------------------------------------------
QString const& EditorConfigurableElement::getEvaluatedDefaultValue() const
{
    return evaluatedDefaultValue_;
}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::setEvaluatedDefaultValue()
//-----------------------------------------------------------------------------
void EditorConfigurableElement::setEvaluatedDefaultValue(QString const& newEvaluatedDefaultValue)
{
    evaluatedDefaultValue_ = newEvaluatedDefaultValue;
}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::getConfiguratedValue()
//-----------------------------------------------------------------------------
QString const& EditorConfigurableElement::getConfiguratedValue() const
{
    return configuratedValue_;
}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::setConfiguratedValue()
//-----------------------------------------------------------------------------
void EditorConfigurableElement::setConfiguratedValue(QString const& newConfiguratedValue)
{
    configuratedValue_ = newConfiguratedValue;
}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::getType()
//-----------------------------------------------------------------------------
QString const& EditorConfigurableElement::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: EditorConfigurableElement::setType()
//-----------------------------------------------------------------------------
void EditorConfigurableElement::setType(QString const& newType)
{
    type_ = newType;
}
