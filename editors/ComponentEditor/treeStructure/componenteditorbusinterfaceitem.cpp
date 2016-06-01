//-----------------------------------------------------------------------------
// File: componenteditorbusinterfaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorbusinterfaceitem.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/busInterfaces/businterfaceeditor.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<BusInterfaceValidator> validator,
    ComponentEditorItem* parent, QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
    busif_(busif),
    parentWnd_(parentWnd),
    editAction_(new QAction(tr("Edit"), this)),
    expressionParser_(expressionParser),
    validator_(validator)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
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
    if (!validator_->validate(busif_))
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
		editor_ = new BusInterfaceEditor(libHandler_, component_, busif_, parameterFinder_, expressionFormatter_,
            expressionParser_, validator_, 0, parentWnd_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)),Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
	}
    setHighlight(false);
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

    if (busif_->getAbstractionTypes() && !busif_->getAbstractionTypes()->isEmpty())
    {
        VLNV absdefVLNV = *busif_->getAbstractionTypes()->first()->getAbstractionRef();
        emit openBus(busdefVLNV, absdefVLNV);
    }
}
