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

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, 
    QSharedPointer<ExpressionParser> expressionParser, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addressSpaces_(component->getAddressSpaces()),
expressionParser_(expressionParser),
spaceValidator_()
{
    createAddressSpaceValidator();

    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	foreach (QSharedPointer<AddressSpace> addrSpace, *addressSpaces_)
    {
		QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
			new ComponentEditorAddrSpaceItem(addrSpace, model, libHandler, component, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, spaceValidator_, this));	
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
    font.setBold(!addressSpaces_->empty());
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
		editor_ = new AddressSpacesEditor(component_, libHandler_, parameterFinder_, expressionFormatter_,
            expressionParser_, spaceValidator_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
		connect(editor_, SIGNAL(selectBusInterface(const QString&)),
			model_, SLOT(onSelectBusInterface(const QString&)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(aubChangedOnRow(int)),
            this, SLOT(addressUnitBitsChangedInRow(int)), Qt::UniqueConnection);

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
		new ComponentEditorAddrSpaceItem(addressSpaces_->at(index), model_, libHandler_, component_,
        referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, spaceValidator_, this));
	addrItem->setLocked(locked_);
	childItems_.insert(index, addrItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditoraddrspacesitem::createAddressSpaceValidator()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpacesItem::createAddressSpaceValidator()
{
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(expressionParser_,
        component_->getChoices()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(expressionParser_, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(
        new RegisterValidator(expressionParser_, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> blockValidator(
        new AddressBlockValidator(expressionParser_, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMapValidator(
        new MemoryMapBaseValidator(expressionParser_, blockValidator));
    QSharedPointer<AddressSpaceValidator> addressSpaceValidator(
        new AddressSpaceValidator(expressionParser_, localMapValidator, parameterValidator));

    spaceValidator_ = addressSpaceValidator;
}

//-----------------------------------------------------------------------------
// Function: componenteditoraddrspacesitem::addressUnitBitsChangedInRow()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpacesItem::addressUnitBitsChangedInRow(int spaceIndex)
{
    QSharedPointer<ComponentEditorAddrSpaceItem> childSpace = 
        qobject_cast<QSharedPointer<ComponentEditorAddrSpaceItem> > ( child(spaceIndex) );

    if (!childSpace.isNull())
    {
        childSpace->changeAdressUnitBitsOnAddressBlocks();
    }
}