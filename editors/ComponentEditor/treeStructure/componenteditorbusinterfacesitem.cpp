//-----------------------------------------------------------------------------
// File: componenteditorbusinterfacesitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Bus interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorbusinterfacesitem.h"
#include "componenteditorbusinterfaceitem.h"

#include <editors/ComponentEditor/busInterfaces/businterfaceseditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::ComponentEditorBusInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfacesItem::ComponentEditorBusInterfacesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent, QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
busifs_(component->getBusInterfaces()),
parentWnd_(parentWnd),
expressionParser_(expressionParser),
validator_()
{
    createBusInterfaceValidator();

    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);

	foreach (QSharedPointer<BusInterface> busif, *busifs_)
    {
		QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
            new ComponentEditorBusInterfaceItem(busif, model, libHandler, component, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, validator_, this, parentWnd));

        connect(busifItem.data(), SIGNAL(openReferenceTree(QString)),
            this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

		childItems_.append(busifItem);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::~ComponentEditorBusInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfacesItem::~ComponentEditorBusInterfacesItem() 
{
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::createBusInterfaceValidator()
//-----------------------------------------------------------------------------
void ComponentEditorBusInterfacesItem::createBusInterfaceValidator()
{
    QSharedPointer<PortMapValidator> portMapValidator(
        new PortMapValidator(expressionParser_, component_->getPorts(), libHandler_));

    QSharedPointer<ParameterValidator2014> parameterValidator(
        new ParameterValidator2014(expressionParser_, component_->getChoices()));

    validator_ = QSharedPointer<BusInterfaceValidator>(new BusInterfaceValidator(expressionParser_,
        component_->getChoices(), component_->getViews(), component_->getPorts(), component_->getAddressSpaces(),
        component_->getMemoryMaps(), component_->getBusInterfaces(), component_->getFileSets(),
        component_->getRemapStates(), portMapValidator, parameterValidator, libHandler_));
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorBusInterfacesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(component_->hasInterfaces());
    return font;
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorBusInterfacesItem::text() const
{
	return tr("Bus interfaces");
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorBusInterfacesItem::editor()
{
	if (!editor_)
    {
		editor_ = new BusInterfacesEditor(libHandler_, component_, validator_, parameterFinder_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childMoved(int, int)), this, SLOT(onMoveChild(int, int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorBusInterfacesItem::getTooltip() const
{
	return tr("Contains the bus interfaces specified for a component");
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorBusInterfacesItem::createChild(int index)
{
	QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
		new ComponentEditorBusInterfaceItem(busifs_->at(index), model_, libHandler_, component_, referenceCounter_,
        parameterFinder_, expressionFormatter_, expressionParser_, validator_, this, parentWnd_));
	busifItem->setLocked(locked_);

    connect(busifItem.data(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

	childItems_.insert(index, busifItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::createChild()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentEditorItem> ComponentEditorBusInterfacesItem::getBusInterfaceItem(
    QString const& interfaceName) const
{
	foreach (const QSharedPointer<ComponentEditorItem> child, childItems_)
    {		
		// if the bus interface name matches the searched interface name
		if (child->text() == interfaceName)
        {
			return child;
		}
	}

	// if child was not found
	return QSharedPointer<ComponentEditorItem>();
}
