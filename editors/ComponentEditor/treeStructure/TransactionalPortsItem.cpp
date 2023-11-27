//-----------------------------------------------------------------------------
// File: TransactionalPortsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 22.11.2023
//
// Description:
// The Transactional ports-item in the component editor's navigation tree. 
//-----------------------------------------------------------------------------


#include "TransactionalPortsItem.h"

#include <editors/ComponentEditor/ports/TransactionalPortEditor.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsItem::TransactionalPortsItem()
//-----------------------------------------------------------------------------
TransactionalPortsItem::TransactionalPortsItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<ReferenceCounter> refCounter,
    ExpressionSet expressions, 
    BusInterfaceInterface* busInterface,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
portValidator_(new PortValidator(expressions.parser, component->getViews())),
busInterface_(busInterface)
{
    setReferenceCounter(refCounter);
    setParameterFinder(expressions.finder);
    setExpressionFormatter(expressions.formatter);
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsItem::text()
//-----------------------------------------------------------------------------
QString TransactionalPortsItem::text() const
{
	return tr("Transactionals");
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsItem::isValid()
//-----------------------------------------------------------------------------
bool TransactionalPortsItem::isValid() const
{
    QStringList portNames;
	for (QSharedPointer<Port> port : *component_->getPorts()) 
    {
        if (port->getTransactional() && (portNames.contains(port->name()) || !portValidator_->validate(port)))
        {
			return false;
		}

        portNames.append(port->name());
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* TransactionalPortsItem::editor()
{
	if (!editor_)
    {
		editor_ = new TransactionalPortEditor(
            component_, libHandler_, parameterFinder_, expressionFormatter_, portValidator_, busInterface_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(createInterface()), 
            this, SIGNAL(createInterface()), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
        connectItemEditorToVendorExtensionsEditor();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsItem::getTooltip()
//-----------------------------------------------------------------------------
QString TransactionalPortsItem::getTooltip() const
{
	return tr("Contains the transactional ports of the component");
}
