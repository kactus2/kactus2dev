//-----------------------------------------------------------------------------
// File: componenteditoraddrspacesitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Address spaces-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditoraddrspacesitem.h"
#include "componenteditoraddrspaceitem.h"
#include <editors/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <editors/ComponentEditor/addressSpaces/addressspaceseditor.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, 
    QSharedPointer<ExpressionParser> expressionParser, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpaces_(component->getAddressSpaces()),
expressionParser_(expressionParser)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) {
		QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
			new ComponentEditorAddrSpaceItem(addrSpace, model, libHandler, component, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, this));	
		childItems_.append(addrItem);
	};
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::~ComponentEditorAddrSpacesItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrSpacesItem::~ComponentEditorAddrSpacesItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorAddrSpacesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!addrSpaces_.empty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrSpacesItem::text() const
{
	return tr("Address spaces");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorAddrSpacesItem::editor()
{
	if (!editor_)
    {
		editor_ = new AddressSpacesEditor(component_, libHandler_, parameterFinder_, 
            expressionFormatter_, expressionParser_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
		connect(editor_, SIGNAL(selectBusInterface(const QString&)),
			model_, SLOT(onSelectBusInterface(const QString&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrSpacesItem::getTooltip() const
{
	return tr("Contains the address spaces specified for the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpacesItem::createChild( int index )
{
	QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
		new ComponentEditorAddrSpaceItem(addrSpaces_.at(index), model_, libHandler_, component_, referenceCounter_,
        parameterFinder_, expressionFormatter_, expressionParser_, this));
	addrItem->setLocked(locked_);
	childItems_.insert(index, addrItem);
}
