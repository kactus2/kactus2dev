/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorbusinterfacesitem.h"
#include "componenteditorbusinterfaceitem.h"
#include <editors/ComponentEditor/busInterfaces/businterfaceseditor.h>

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
expressionParser_(expressionParser)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);

	foreach (QSharedPointer<BusInterface> busif, busifs_)
    {
		QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
            new ComponentEditorBusInterfaceItem(busif, model, libHandler, component, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, this, parentWnd));

        connect(busifItem.data(), SIGNAL(openReferenceTree(QString)),
            this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

		childItems_.append(busifItem);
	}
}

ComponentEditorBusInterfacesItem::~ComponentEditorBusInterfacesItem() {
}

QFont ComponentEditorBusInterfacesItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    font.setBold(component_->hasInterfaces());
    return font;
}

QString ComponentEditorBusInterfacesItem::text() const {
	return tr("Bus interfaces");
}

ItemEditor* ComponentEditorBusInterfacesItem::editor() {
	if (!editor_) {
		editor_ = new BusInterfacesEditor(libHandler_, component_, parameterFinder_);
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

QString ComponentEditorBusInterfacesItem::getTooltip() const {
	return tr("Contains the bus interfaces specified for a component");
}

void ComponentEditorBusInterfacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
		new ComponentEditorBusInterfaceItem(busifs_.at(index), model_, libHandler_, component_, referenceCounter_,
        parameterFinder_, expressionFormatter_, expressionParser_, this, parentWnd_));
	busifItem->setLocked(locked_);

    connect(busifItem.data(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

	childItems_.insert(index, busifItem);
}

QSharedPointer<ComponentEditorItem> ComponentEditorBusInterfacesItem::getBusInterfaceItem( const QString& interfaceName ) const {
	foreach (const QSharedPointer<ComponentEditorItem> child, childItems_) {
		
		// if the bus interface name matches the searched interface name
		if (child->text() == interfaceName) {
			return child;
		}
	}

	// if child was not found
	return QSharedPointer<ComponentEditorItem>();
}
