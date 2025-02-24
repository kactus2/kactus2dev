//-----------------------------------------------------------------------------
// File: componenteditorbusinterfacesitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Bus interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorbusinterfacesitem.h"
#include "componenteditorbusinterfaceitem.h"

#include <editors/ComponentEditor/busInterfaces/businterfaceseditor.h>

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/PortsInterface.h>
#include <KactusAPI/include/PortMapInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::ComponentEditorBusInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfacesItem::ComponentEditorBusInterfacesItem(BusInterfaceInterface* busInterface,
    PortMapInterface* portMapInterface, ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    ExpressionSet expressions, ComponentEditorItem* parent, QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
busifs_(component->getBusInterfaces()),
parentWnd_(parentWnd),
expressions_(expressions),
busInterface_(busInterface),
portMapInterface_(portMapInterface)
{
    createBusInterfaceValidator();

    setParameterFinder(expressions.finder);
    setExpressionFormatter(expressions.formatter);
    setReferenceCounter(referenceCounter);

	for (QSharedPointer<BusInterface> busif : *busifs_)
    {
		QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(new ComponentEditorBusInterfaceItem(
            busif, model, libHandler, component, referenceCounter_, expressions,
            busInterfaceValidator_, busInterface_, portMapInterface_, this, parentWnd));

        connect(busifItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
            this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

		childItems_.append(busifItem);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::createBusInterfaceValidator()
//-----------------------------------------------------------------------------
void ComponentEditorBusInterfacesItem::createBusInterfaceValidator()
{
    QSharedPointer<PortMapValidator> portMapValidator(
        new PortMapValidator(expressions_.parser, component_->getPorts(), libHandler_));

    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(expressions_.parser, component_->getChoices(), component_->getRevision()));

    busInterfaceValidator_ = QSharedPointer<BusInterfaceValidator>(new BusInterfaceValidator(expressions_.parser,
        component_->getChoices(), component_->getViews(), component_->getPorts(), component_->getAddressSpaces(),
        component_->getMemoryMaps(), component_->getBusInterfaces(), component_->getFileSets(),
        component_->getRemapStates(), component_->getModes(), portMapValidator, parameterValidator, libHandler_));
    
    busInterfacesValidator_ = QSharedPointer<BusInterfacesValidator>(new BusInterfacesValidator(busInterfaceValidator_));
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
		editor_ = new BusInterfacesEditor(libHandler_, component_, busInterface_, parameterFinder_);
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
	QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(new ComponentEditorBusInterfaceItem(
        busifs_->at(index), model_, libHandler_, component_, referenceCounter_, expressions_,
        busInterfaceValidator_, busInterface_, portMapInterface_, this, parentWnd_));
	busifItem->setLocked(locked_);

    connect(busifItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

	childItems_.insert(index, busifItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditorbusinterfacesitem::createChild()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentEditorItem> ComponentEditorBusInterfacesItem::getBusInterfaceItem(
    QString const& interfaceName) const
{
	for (auto const& child : childItems_)
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

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfacesItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorBusInterfacesItem::isValid() const
{
    auto interfacesAsNameGroup = CollectionValidators::itemListToNameGroupList(busifs_);
    busInterfacesValidator_->childrenHaveUniqueNames(interfacesAsNameGroup);
    return ComponentEditorItem::isValid();
}
