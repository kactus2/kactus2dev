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

#include <editors/ComponentEditor/ports/TypedPortEditor.h>
#include <editors/ComponentEditor/ports/TransactionalPortsEditorFactory.h>

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/PortsInterface.h>
#include <KactusAPI/include/LibraryInterface.h>

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
    QSharedPointer<PortsInterface> portsInterface,
    BusInterfaceInterface* busInterface,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    expressions_(expressions),
    portValidator_(new PortValidator(expressions.parser, component->getViews())),
    portsInterface_(portsInterface),
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
        QSharedPointer<PortAbstractionInterface> signalInterface(new PortAbstractionInterface(expressions_.parser, expressions_.formatter));

        const QString defaultPath = QString("%1/transactionalList.csv").arg(libHandler_->getDirectoryPath(component_->getVlnv()));

        TransactionalPortsEditorFactory transactionalFactory(component_, expressions_, portValidator_,
            portsInterface_, signalInterface, busInterface_, defaultPath);

        editor_ = new TypedPortEditor(component_, libHandler_, &transactionalFactory, 
            QStringLiteral("transactional"), portsInterface_, busInterface_);
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
// Function: TransactionalPortsItem::getFont()
//-----------------------------------------------------------------------------
QFont TransactionalPortsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(component_->hasTransactionals());
    return font;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsItem::getTooltip()
//-----------------------------------------------------------------------------
QString TransactionalPortsItem::getTooltip() const
{
	return tr("Contains the transactional ports of the component");
}
