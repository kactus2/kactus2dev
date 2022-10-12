//-----------------------------------------------------------------------------
// File: ConfigurableElementRemoveCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 28.08.2017
//
// Description:
// Undo remove command for configurable element values.
//-----------------------------------------------------------------------------

#include "ConfigurableElementRemoveCommand.h"

#include <IPXACTmodels/common/ConfigurableElementValue.h>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementRemoveCommand::ConfigurableElementRemoveCommand()
//-----------------------------------------------------------------------------
ConfigurableElementRemoveCommand::ConfigurableElementRemoveCommand(QString const& elementID, int itemRow,
    QString const& parentName,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements, QUndoCommand* parent):
QObject(0),
QUndoCommand(parent),
parentName_(parentName),
itemRow_(itemRow),
configurableElements_(configurableElements),
rowInConfigurableElements_(0),
removedElement_(getRemovedConfigurableElement(elementID))
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementRemoveCommand::getRemovedConfigurableElement()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableElementValue> ConfigurableElementRemoveCommand::getRemovedConfigurableElement(
    QString const& elementID)
{
    foreach (QSharedPointer<ConfigurableElementValue> element, *configurableElements_)
    {
        if (element->getReferenceId() == elementID)
        {
            return element;
        }
    }

    return QSharedPointer<ConfigurableElementValue>();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementRemoveCommand::undo()
//-----------------------------------------------------------------------------
void ConfigurableElementRemoveCommand::undo()
{
    if (removedElement_)
    {
        if (!configurableElements_->contains(removedElement_))
        {
            configurableElements_->insert(rowInConfigurableElements_, removedElement_);

            emit addConfigurableElement(
                removedElement_->getReferenceId(), removedElement_->getConfigurableValue(), itemRow_);

            emit increaseReferencesInNewValue(removedElement_->getConfigurableValue());
        }
    }

    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementRemoveCommand::redo()
//-----------------------------------------------------------------------------
void ConfigurableElementRemoveCommand::redo()
{
    QUndoCommand::redo();

    if (removedElement_)
    {
        rowInConfigurableElements_ = configurableElements_->indexOf(removedElement_);

        if (configurableElements_->contains(removedElement_))
        {
            configurableElements_->removeOne(removedElement_);

            emit removeConfigurableElement(removedElement_->getReferenceId(), itemRow_);

            emit decreaseReferencesInOldValue(removedElement_->getConfigurableValue());
        }
    }
}
