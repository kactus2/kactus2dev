//-----------------------------------------------------------------------------
// File: componenteditoraddrspacesitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <KactusAPI/include/ResetInterface.h>
#include <KactusAPI/include/FieldInterface.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>

#include <KactusAPI/include/ParametersInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

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
spaceValidator_(),
blockInterface_()
{
    createAddressSpaceValidator();

    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    createAddressBlockInterface();

	foreach (QSharedPointer<AddressSpace> addrSpace, *addressSpaces_)
    {
		QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
			new ComponentEditorAddrSpaceItem(addrSpace, model, libHandler, component, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, spaceValidator_, blockInterface_, this));	
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
	QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(new ComponentEditorAddrSpaceItem(
        addressSpaces_->at(index), model_, libHandler_, component_, referenceCounter_, parameterFinder_,
        expressionFormatter_, expressionParser_, spaceValidator_, blockInterface_, this));
	addrItem->setLocked(locked_);
	childItems_.insert(index, addrItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditoraddrspacesitem::createAddressSpaceValidator()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpacesItem::createAddressSpaceValidator()
{
    QSharedPointer<ParameterValidator> parameterValidator (new ParameterValidator(expressionParser_,
        component_->getChoices(), component_->getRevision()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(expressionParser_, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(
        new RegisterValidator(expressionParser_, fieldValidator, parameterValidator));

    QSharedPointer<RegisterFileValidator> registerFileValidator(
        new RegisterFileValidator(expressionParser_, registerValidator, parameterValidator));


    QSharedPointer<AddressBlockValidator> blockValidator(
        new AddressBlockValidator(expressionParser_, registerValidator,registerFileValidator, parameterValidator, 
            component_->getRevision()));
    QSharedPointer<SubspaceMapValidator> subspaceValidator(
        new SubspaceMapValidator(expressionParser_, parameterValidator, component_->getRevision()));

    QSharedPointer<MemoryMapBaseValidator> localMapValidator(
        new MemoryMapBaseValidator(expressionParser_, blockValidator, subspaceValidator));

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

//-----------------------------------------------------------------------------
// Function: componenteditoraddrspacesitem::createAddressBlockInterface()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpacesItem::createAddressBlockInterface()
{
    QSharedPointer<AddressBlockValidator> blockValidator =
        spaceValidator_->getLocalMemoryMapValidator()->getAddressBlockValidator();
    QSharedPointer<RegisterValidator> registerValidator = blockValidator->getRegisterValidator();
    QSharedPointer<FieldValidator> fieldValidator = registerValidator->getFieldValidator();

    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(expressionParser_,
        component_->getChoices(), component_->getRevision()));

    ParametersInterface* parameterInterface(
        new ParametersInterface(parameterValidator, expressionParser_, expressionFormatter_));

    ResetInterface* resetInterface(new ResetInterface(fieldValidator, expressionParser_, expressionFormatter_));
    FieldInterface* fieldInterface(
        new FieldInterface(fieldValidator, expressionParser_, expressionFormatter_, resetInterface));

    RegisterInterface* registerInterface(
        new RegisterInterface(registerValidator, expressionParser_, expressionFormatter_, fieldInterface));

    BusInterfaceInterface* busInterface = createInterfaceForBus(parameterValidator);

    blockInterface_ = new AddressBlockInterface(blockValidator, expressionParser_, expressionFormatter_,
        busInterface, registerInterface, parameterInterface);
}


//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpacesItem::createInterfaceForBus()
//-----------------------------------------------------------------------------
BusInterfaceInterface* ComponentEditorAddrSpacesItem::createInterfaceForBus(
    QSharedPointer<ParameterValidator> parameterValidator)
{
    QSharedPointer<PortMapValidator> portMapValidator(
        new PortMapValidator(expressionParser_, component_->getPorts(), libHandler_));
    QSharedPointer<BusInterfaceValidator> busValidator(new BusInterfaceValidator(expressionParser_,
        component_->getChoices(), component_->getViews(), component_->getPorts(), component_->getAddressSpaces(),
        component_->getMemoryMaps(), component_->getBusInterfaces(), component_->getFileSets(),
        component_->getRemapStates(), portMapValidator, parameterValidator, libHandler_));

    BusInterfaceInterface* busInterface(
        new BusInterfaceInterface(busValidator, expressionParser_, expressionFormatter_));

    return busInterface;
}
