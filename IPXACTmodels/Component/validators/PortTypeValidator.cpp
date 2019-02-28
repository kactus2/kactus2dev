//-----------------------------------------------------------------------------
// Port: PortTypeValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.01.2018
//
// Description:
// Validator for port types and their associated elements.
//-----------------------------------------------------------------------------

#include "PortTypeValidator.h"

#include <IPXACTmodels/Component/View.h>

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::PortTypeValidator()
//-----------------------------------------------------------------------------
PortTypeValidator::PortTypeValidator(QSharedPointer<QList<QSharedPointer<View> > > views):
availableViews_(views)
{

}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::componentChange()
//-----------------------------------------------------------------------------
void PortTypeValidator::componentChange(QSharedPointer<QList<QSharedPointer<View> > > newViews)
{
    availableViews_ = newViews;
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::validate()
//-----------------------------------------------------------------------------
bool PortTypeValidator::validate(QSharedPointer<WireTypeDef> typeDefinition,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const
{
    return hasValidtype(typeDefinition) && hasValidViewReferences(typeDefinition, definitionList);
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::hasValidtype()
//-----------------------------------------------------------------------------
bool PortTypeValidator::hasValidtype(QSharedPointer<WireTypeDef> /*typeDefinition*/) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::hasValidViewReferences()
//-----------------------------------------------------------------------------
bool PortTypeValidator::hasValidViewReferences(QSharedPointer<WireTypeDef> typeDefinition,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const
{
    if (typeDefinition->getViewRefs()->isEmpty() && definitionList->size() != 1)
    {
        return false;
    }
    else
    {
        for (QString const& view : *typeDefinition->getViewRefs())
        {            
            if (!hasValidView(view, definitionList))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::hasValidView()
//-----------------------------------------------------------------------------
bool PortTypeValidator::hasValidView(QString const& view,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitionList) const
{
    return !view.isEmpty() && referencedViewExists(view) && viewIsReferencedOnce(view, typeDefinitionList);
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::referencedViewExists()
//-----------------------------------------------------------------------------
bool PortTypeValidator::referencedViewExists(QString const& viewRef) const
{
    for (QSharedPointer<View> view : *availableViews_)
    {
        if (view->name() == viewRef)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::viewIsReferencedOnce()
//-----------------------------------------------------------------------------
bool PortTypeValidator::viewIsReferencedOnce(QString const& referencedView,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitionList) const
{
    if (!typeDefinitionList)
    {
        return false;
    }

    int count = 0;
    for (QSharedPointer<WireTypeDef> typeDefinition : *typeDefinitionList)
    {
        for (QString const& typeView : *typeDefinition->getViewRefs())
        {           
            if (QString::compare(typeView, referencedView) == 0)
            {
                ++count;
                if (count > 1)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PortTypeValidator::findErrorsIn(QVector<QString>& errors, QString const& context,
    QSharedPointer<WireTypeDef> typeDefinition,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const
{
    if (!hasValidtype(typeDefinition))
    {
        errors.append(QObject::tr("Empty port type name set for wire type definition within %1").arg(context));
    }

    findErrorsInViewReferences(errors, context, typeDefinition, definitionList);
}

//-----------------------------------------------------------------------------
// Function: PortTypeValidator::findErrorsInViewReferences()
//-----------------------------------------------------------------------------
void PortTypeValidator::findErrorsInViewReferences(QVector<QString>& errors, QString const& context,
    QSharedPointer<WireTypeDef> typeDefinition,
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList) const
{
    QString typeName = typeDefinition->getTypeName();

    if (typeDefinition->getViewRefs()->isEmpty() && definitionList->size() > 1)
    {
        errors.append(QObject::tr(
            "Port type %1 within %2 should be the only defined type for the containing port").
            arg(typeName).arg(context));
    }
    else
    {
        for (QString const& view : *typeDefinition->getViewRefs())
        {
            if (view.isEmpty())
            {
                errors.append(QObject::tr("Empty view reference assigned to port type %1 within %2").
                    arg(typeName).arg(context));
            }
            else
            {
                if (!referencedViewExists(view))
                {
                    errors.append(QObject::tr("View %1 referenced by port type %2 does not exist within %3").
                        arg(view).arg(typeName).arg(context));
                }

                if (!viewIsReferencedOnce(view, definitionList))
                {
                    errors.append(QObject::tr(
                        "View %1 referenced by port type %2 has been used in multiple types within %3").
                        arg(view).arg(typeName).arg(context));
                }
            }
        }
    }
}
