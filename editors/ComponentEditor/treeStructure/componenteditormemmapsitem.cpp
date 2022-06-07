//-----------------------------------------------------------------------------
// File: componenteditormemmapsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Memory maps-item in the component navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditormemmapsitem.h"
#include "componenteditormemmapitem.h"
#include <editors/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <editors/ComponentEditor/memoryMaps/memorymapseditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>

#include <KactusAPI/include/ResetInterface.h>
#include <KactusAPI/include/FieldInterface.h>
#include <KactusAPI/include/RegisterInterface.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/SubspaceMapInterface.h>
#include <KactusAPI/include/ParametersInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::ComponentEditorMemMapsItem()
//-----------------------------------------------------------------------------
ComponentEditorMemMapsItem::ComponentEditorMemMapsItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
    memoryMaps_(component->getMemoryMaps()),
    visualizer_(nullptr),
    expressionParser_(expressionParser),
    memoryMapValidator_(),
    mapInterface_()
{
    createMemoryMapValidator();

    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    createMemoryMapInterface();

	setObjectName(tr("ComponentEditorMemMapsItem"));

    const int childCount = memoryMaps_->count();
	for (int i = 0; i < childCount; ++i)
    {
        ComponentEditorMemMapsItem::createChild(i);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorMemMapsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!memoryMaps_->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorMemMapsItem::text() const
{
	return tr("Memory maps");
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorMemMapsItem::editor()
{
	if (!editor_)
    {
        editor_ =
            new MemoryMapsEditor(mapInterface_, component_, parameterFinder_, expressionParser_, libHandler_);
        editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
		connect(editor_, SIGNAL(selectBusInterface(const QString&)),
            model_, SLOT(onSelectBusInterface(const QString&)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(changeInAddressUnitBitsOnRow(int)), 
            this, SLOT(addressUnitBitsChangedOnMemoryMap(int)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(memoryRemapAdded(int, QString const&)),
            this, SIGNAL(memoryRemapAdded(int, QString const&)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(memoryRemapRemoved(int, QString const&)),
            this, SIGNAL(memoryRemapRemoved(int, QString const&)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(memoryMapNameChanged(QString const&, QString const&)),
            this, SIGNAL(memoryMapNameChanged(QString const&, QString const&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)),
            this, SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)),
            Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorMemMapsItem::getTooltip() const
{
	return tr("Contains the memory maps of the component");
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapsItem::createChild( int index )
{
	QSharedPointer<ComponentEditorMemMapItem> memoryMapItem(new ComponentEditorMemMapItem(memoryMaps_->at(index),
        model_, libHandler_, component_, referenceCounter_, parameterFinder_, expressionFormatter_,
        expressionParser_, memoryMapValidator_, mapInterface_, this));
	memoryMapItem->setLocked(locked_);
	childItems_.insert(index, memoryMapItem);
	
    connect(this, SIGNAL(memoryRemapAdded(int, QString const&)),
        memoryMapItem.data(), SLOT(onMemoryRemapAdded(int, QString const&)), Qt::UniqueConnection);

    connect(this, SIGNAL(memoryRemapRemoved(int, QString const&)),
        memoryMapItem.data(), SLOT(onMemoryRemapRemoved(int, QString const&)), Qt::UniqueConnection);

    connect(this, SIGNAL(memoryMapNameChanged(QString const&, QString const&)),
        memoryMapItem.data(), SIGNAL(memoryMapNameChanged(QString const&, QString const&)),
        Qt::UniqueConnection);
    connect(this, SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)),
        memoryMapItem.data(), SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)),
        Qt::UniqueConnection);

}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorMemMapsItem::visualizer()
{
	if (childItems_.isEmpty() == false)
	{
        return childItems_.at(0)->visualizer();
	}

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::addressUnitBitsChangedOnMemoryMap()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapsItem::addressUnitBitsChangedOnMemoryMap(int memoryMapIndex)
{
    QSharedPointer<ComponentEditorMemMapItem> childMemoryMap = 
        qobject_cast<QSharedPointer<ComponentEditorMemMapItem> > ( child(memoryMapIndex) );
    
    if (!childMemoryMap.isNull())
    {
        childMemoryMap->changeAdressUnitBitsOnAddressBlocks();
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::createMemoryMapValidator()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapsItem::createMemoryMapValidator()
{
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(expressionParser_, component_->getChoices()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(expressionParser_, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(expressionParser_, fieldValidator, parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator (
        new RegisterFileValidator(expressionParser_, registerValidator, parameterValidator));

    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(expressionParser_, registerValidator,registerFileValidator, parameterValidator));

    QSharedPointer<SubspaceMapValidator> subspaceValidator(
        new SubspaceMapValidator(expressionParser_, parameterValidator));

    QSharedPointer<MemoryMapValidator> memoryMapValidator(new MemoryMapValidator(
        expressionParser_, addressBlockValidator, subspaceValidator, component_));

    memoryMapValidator_ = memoryMapValidator;

    memoryMapValidator_->componentChange(component_);
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::createMemoryMapInterface()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapsItem::createMemoryMapInterface()
{
    QSharedPointer<SubspaceMapValidator> subspaceValidator = memoryMapValidator_->getSubspaceValidator();
    QSharedPointer<AddressBlockValidator> blockValidator = memoryMapValidator_->getAddressBlockValidator();
    QSharedPointer<RegisterValidator> registerValidator = blockValidator->getRegisterValidator();
    QSharedPointer<FieldValidator> fieldValidator = registerValidator->getFieldValidator();

    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(expressionParser_,
        component_->getChoices()));

    ParametersInterface* parameterInterface(
        new ParametersInterface(parameterValidator, expressionParser_, expressionFormatter_));

    ResetInterface* resetInterface(new ResetInterface(fieldValidator, expressionParser_, expressionFormatter_));
    FieldInterface* fieldInterface(
        new FieldInterface(fieldValidator, expressionParser_, expressionFormatter_, resetInterface));
    RegisterInterface* registerInterface(
        new RegisterInterface(registerValidator, expressionParser_, expressionFormatter_, fieldInterface));

    BusInterfaceInterface* busInterface = createInterfaceForBus(parameterValidator);

    AddressBlockInterface* blockInterface(new AddressBlockInterface(blockValidator, expressionParser_,
        expressionFormatter_, busInterface, registerInterface, parameterInterface));
    SubspaceMapInterface* subspaceInterface(new SubspaceMapInterface(
        subspaceValidator, expressionParser_, expressionFormatter_, busInterface, parameterInterface));

    mapInterface_ =
        new MemoryMapInterface(memoryMapValidator_, expressionParser_, expressionFormatter_);
    mapInterface_->setAddressBlockInterface(blockInterface);
    mapInterface_->setSubspaceMapInterface(subspaceInterface);
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::createInterfaceForBus()
//-----------------------------------------------------------------------------
BusInterfaceInterface* ComponentEditorMemMapsItem::createInterfaceForBus(
    QSharedPointer<ParameterValidator> parameterValidator)
{
    QSharedPointer<PortMapValidator> portMapValidator(
        new PortMapValidator(expressionParser_, component_->getPorts(), libHandler_));
    QSharedPointer<BusInterfaceValidator> busValidator(new BusInterfaceValidator(expressionParser_,
        component_->getChoices(), component_->getViews(), component_->getPorts(), component_->getAddressSpaces(),
        memoryMaps_, component_->getBusInterfaces(), component_->getFileSets(), component_->getRemapStates(),
        portMapValidator, parameterValidator, libHandler_));

    BusInterfaceInterface* busInterface(
        new BusInterfaceInterface(busValidator, expressionParser_, expressionFormatter_));

    return busInterface;
}
