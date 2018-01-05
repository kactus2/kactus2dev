//-----------------------------------------------------------------------------
// File: ConfigurableElementChangeCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.08.2017
//
// Description:
// Undo change commands for configurable element value editor.
//-----------------------------------------------------------------------------

#include "ConfigurableElementChangeCommand.h"

#include <IPXACTmodels/common/ConfigurableElementValue.h>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementChangeCommand::ConfigurableElementChangeCommand()
//-----------------------------------------------------------------------------
ConfigurableElementChangeCommand::ConfigurableElementChangeCommand(QString const& elementID,
    QString const& oldValue, QString const& newValue, QString const& defaultValue,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements, QUndoCommand* parent):
QObject(0),
QUndoCommand(parent),
oldValue_(oldValue),
newValue_(newValue),
defaultValue_(defaultValue),
element_(0),
configurableElements_(configurableElements)
{
    element_ = setupConfigurableElement(elementID);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementChangeCommand::~ConfigurableElementChangeCommand()
//-----------------------------------------------------------------------------
ConfigurableElementChangeCommand::~ConfigurableElementChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementChangeCommand::setupConfigurableElement()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableElementValue> ConfigurableElementChangeCommand::setupConfigurableElement(
    QString const& elementID)
{
    foreach (QSharedPointer<ConfigurableElementValue> singleElement, *configurableElements_)
    {
        if (singleElement->getReferenceId() == elementID)
        {
            return singleElement;
        }
    }

    QSharedPointer<ConfigurableElementValue> newElement(new ConfigurableElementValue(newValue_, elementID));
    return newElement;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementChangeCommand::undo()
//-----------------------------------------------------------------------------
void ConfigurableElementChangeCommand::undo()
{
    changeConfigurableElement(oldValue_);

    emit increaseReferencesInNewValue(oldValue_);
    emit decreaseReferencesInOldValue(newValue_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementChangeCommand::redo()
//-----------------------------------------------------------------------------
void ConfigurableElementChangeCommand::redo()
{
    changeConfigurableElement(newValue_);

    emit increaseReferencesInNewValue(newValue_);
    emit decreaseReferencesInOldValue(oldValue_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementChangeCommand::changeConfigurableElement()
//-----------------------------------------------------------------------------
void ConfigurableElementChangeCommand::changeConfigurableElement(QString const& newElementValue)
{
    element_->setConfigurableValue(newElementValue);

    if (!defaultValue_.isEmpty())
    {
        if (newElementValue.isEmpty() || newElementValue == defaultValue_)
        {
            if (configurableElements_->contains(element_))
            {
                configurableElements_->removeOne(element_);
            }
        }
        else if (!configurableElements_->contains(element_))
        {
            configurableElements_->append(element_);
        }
    }

    emit dataChangedInID(element_->getReferenceId(), newElementValue);
}
