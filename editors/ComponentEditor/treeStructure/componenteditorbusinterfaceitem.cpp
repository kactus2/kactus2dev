//-----------------------------------------------------------------------------
// File: componenteditorbusinterfaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorbusinterfaceitem.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/busInterfaces/businterfaceeditor.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, ExpressionSet expressions,
    QSharedPointer<BusInterfaceValidator> validator, BusInterfaceInterface* busInterface,
    PortMapInterface* portMapInterface, ComponentEditorItem* parent, QWidget* parentWnd):
ParameterizableItem(model, libHandler, component, parent),
busif_(busif),
parentWnd_(parentWnd),
editAction_(new QAction(tr("Edit"), this)),
expressions_(expressions),
validator_(validator),
busInterface_(busInterface),
portMapInterface_(portMapInterface)
{
    setParameterFinder(expressions.finder);
    setExpressionFormatter(expressions.formatter);
    setReferenceCounter(referenceCounter);

    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::~ComponentEditorBusInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfaceItem::~ComponentEditorBusInterfaceItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorBusInterfaceItem::text() const
{
	return busif_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorBusInterfaceItem::isValid() const
{
    if (!validator_->validate(busif_, component_->getRevision()))
    {
        return false;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorBusInterfaceItem::editor()
{
	if (!editor_)
    {
        editor_ = new BusInterfaceEditor(libHandler_, component_, busif_, expressions_, 
            busInterface_, portMapInterface_, 0, parentWnd_);
        editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)),Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorBusInterfaceItem::getTooltip() const
{
	return tr("Defines properties of a specific interface in a component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorBusInterfaceItem::canBeOpened() const
{
	if (busif_->getBusType().isValid())
    {
		return libHandler_->contains(busif_->getBusType());
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorBusInterfaceItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);
    
    return actionList;   
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorBusInterfaceItem::openItem() 
{
	VLNV busdefVLNV = busif_->getBusType();

    emit openBus(busdefVLNV);
}
