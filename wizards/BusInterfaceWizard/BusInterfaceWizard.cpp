//-----------------------------------------------------------------------------
// File: BusInterfaceWizard.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Wizard for generating bus interface from a set of ports.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizard.h"

#include <QLabel>
#include <QVBoxLayout>

#include "BusInterfaceWizardIntroPage.h"
#include "BusInterfaceWizardGeneralOptionsPage.h"
#include "BusInterfaceWizardBusDefinitionPage.h"
#include "BusInterfaceWizardPortMapPage.h"
#include "BusInterfaceWizardConclusionPage.h"

#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileBuilderInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/AbstractionTypeInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/TransparentBridgeInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/ResetInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/FieldInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/AddressBlockInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::BusInterfaceWizard()
//-----------------------------------------------------------------------------
BusInterfaceWizard::BusInterfaceWizard(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf,
    LibraryInterface* handler, QStringList portNames, QWidget* parent, VLNV absDefVLNV,
    bool descriptionAsLogicalName):
 QWizard(parent)
{
    setWindowTitle(tr("Bus Interface Wizard"));
    setWizardStyle(ModernStyle);
    setOption(NoCancelButton, true);
    setOption(NoDefaultButton, true);
    setOption(HaveFinishButtonOnEarlyPages, false);
    resize(800, 800);

    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(component));
    QSharedPointer<ExpressionFormatter> expressionFormatter(new ExpressionFormatter(parameterFinder));
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(
        expressionParser, component->getChoices()));

    BusInterfaceWizardBusDefinitionEditorPage::SignalNamingPolicy namingPolicy =
        BusInterfaceWizardBusDefinitionEditorPage::NAME;
    if (descriptionAsLogicalName)
    {
        namingPolicy = BusInterfaceWizardBusDefinitionEditorPage::DESCRIPTION;
    }

    BusInterfaceInterface* busInterface =
        createBusInterface(parameterFinder, expressionFormatter, expressionParser, component, handler);

    BusInterfaceWizardGeneralOptionsPage* optionsPage =
        new BusInterfaceWizardGeneralOptionsPage(component, busIf, handler, !absDefVLNV.isValid(), parameterFinder,
            expressionFormatter, expressionParser, busInterface, this);

    connect(optionsPage, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(optionsPage, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    setPage(PAGE_INTRO, new BusInterfaceWizardIntroPage(this));
    setPage(PAGE_GENERALOPTIONS, optionsPage);
    setPage(PAGE_BUSDEFINITION, new BusInterfaceWizardBusDefinitionEditorPage(component, busIf, handler, portNames, 
        this, absDefVLNV, expressionParser, namingPolicy));
    setPage(PAGE_PORTMAPS, new BusInterfaceWizardPortMapPage(component, busIf, handler, portNames,
        expressionParser, parameterFinder, busInterface, this));
    setPage(PAGE_SUMMARY, new BusInterfaceWizardConclusionPage(busIf, portNames, this));
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::~BusInterfaceWizard()
//-----------------------------------------------------------------------------
BusInterfaceWizard::~BusInterfaceWizard()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::createBusInterface()
//-----------------------------------------------------------------------------
BusInterfaceInterface* BusInterfaceWizard::createBusInterface(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<IPXactSystemVerilogParser> expressionParser, QSharedPointer<Component> component,
    LibraryInterface* library) const
{

    QSharedPointer<PortMapValidator> portMapValidator(
        new PortMapValidator(expressionParser, component->getPorts(), library));

    FileSetInterface* fileSetInterface =
        createFileSetInterface(parameterFinder, expressionFormatter, expressionParser, component);
    fileSetInterface->setFileSets(component->getFileSets());

    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(expressionParser,
        component->getChoices()));

    MemoryMapInterface* mapInterface = createMapInterface(parameterFinder, expressionFormatter, expressionParser,
        parameterValidator, component);

    AbstractionTypeInterface* abstractionInterface = creaetAbstractionTypeInterface(
        parameterFinder, expressionFormatter, expressionParser, portMapValidator, component, library);

    TransparentBridgeInterface* bridgeInterface = createBridgeInterface(expressionFormatter, expressionParser);

    QSharedPointer<BusInterfaceValidator> busValidator(new BusInterfaceValidator(expressionParser,
        component->getChoices(), component->getViews(), component->getPorts(), component->getAddressSpaces(),
        component->getMemoryMaps(), component->getBusInterfaces(), component->getFileSets(),
        component->getRemapStates(), portMapValidator, parameterValidator, library));

    BusInterfaceInterface* busInterface(new BusInterfaceInterface(busValidator, expressionParser,
        expressionFormatter, fileSetInterface, mapInterface, abstractionInterface, bridgeInterface));

    busInterface->setBusInterfaces(component);
    return busInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::createFileSetInterface()
//-----------------------------------------------------------------------------
FileSetInterface* BusInterfaceWizard::createFileSetInterface(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<IPXactSystemVerilogParser> expressionParser, QSharedPointer<Component> component) const
{
    QSharedPointer<FileValidator> fileValidator(new FileValidator(expressionParser));
    QSharedPointer<FileSetValidator> fileSetValidator(new FileSetValidator(fileValidator, expressionParser));

    FileInterface* fileInterface(new FileInterface(fileValidator, expressionParser, expressionFormatter));
    FileBuilderInterface* fileBuilderInterface(new FileBuilderInterface(expressionParser, expressionFormatter));

    FileSetInterface* fileSetInterface(new FileSetInterface(
        fileSetValidator, expressionParser, expressionFormatter, fileInterface, fileBuilderInterface));

    fileSetInterface->setFileSets(component->getFileSets());

    return fileSetInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::createMapInterface()
//-----------------------------------------------------------------------------
MemoryMapInterface* BusInterfaceWizard::createMapInterface(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<IPXactSystemVerilogParser> expressionParser,
    QSharedPointer<ParameterValidator> parameterValidator, QSharedPointer<Component> component) const
{
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(expressionParser));
    QSharedPointer<FieldValidator> fieldValidator(
        new FieldValidator(expressionParser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(
        new RegisterValidator(expressionParser, fieldValidator, parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator(
        new RegisterFileValidator(expressionParser, registerValidator, parameterValidator));

    QSharedPointer<AddressBlockValidator> blockValidator(
        new AddressBlockValidator(expressionParser, registerValidator, registerFileValidator, parameterValidator));
    QSharedPointer<MemoryMapValidator> memoryMapValidator(
        new MemoryMapValidator(expressionParser, blockValidator, component->getRemapStates()));

    memoryMapValidator->componentChange(component->getRemapStates(), component->getResetTypes());

    ResetInterface* resetInterface(new ResetInterface(fieldValidator, expressionParser, expressionFormatter));
    FieldInterface* fieldInterface(
        new FieldInterface(fieldValidator, expressionParser, expressionFormatter, resetInterface));
    RegisterInterface* registerInterface(
        new RegisterInterface(registerValidator, expressionParser, expressionFormatter, fieldInterface));
    AddressBlockInterface* blockInterface(
        new AddressBlockInterface(blockValidator, expressionParser, expressionFormatter, registerInterface));

    MemoryMapInterface* mapInterface =
        new MemoryMapInterface(memoryMapValidator, expressionParser, expressionFormatter, blockInterface);

    mapInterface->setMemoryMaps(component);

    return mapInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::creaetAbstractionTypeInterface()
//-----------------------------------------------------------------------------
AbstractionTypeInterface* BusInterfaceWizard::creaetAbstractionTypeInterface(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<IPXactSystemVerilogParser> expressionParser, QSharedPointer<PortMapValidator> portMapValidator,
    QSharedPointer<Component> component, LibraryInterface* library) const
{
    PortMapInterface* portMapInterface = createPortMapInterface(parameterFinder, expressionFormatter,
        expressionParser, portMapValidator, component);

    QSharedPointer<AbstractionTypeValidator> validator(
        new AbstractionTypeValidator(expressionParser, component->getViews(), portMapValidator, library));

    AbstractionTypeInterface* abstractionInterface(new AbstractionTypeInterface(portMapInterface, validator));
    return abstractionInterface;
}


//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::createPortMapInterface()
//-----------------------------------------------------------------------------
PortMapInterface* BusInterfaceWizard::createPortMapInterface(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<IPXactSystemVerilogParser> expressionParser, QSharedPointer<PortMapValidator> portMapValidator,
    QSharedPointer<Component> component) const
{
    QSharedPointer<PortValidator> portValidator(new PortValidator(expressionParser, component->getViews()));

    PortsInterface* physicalPortInterface(new PortsInterface(portValidator, expressionParser, expressionFormatter));
    physicalPortInterface->setPorts(component);

    PortAbstractionInterface* logicalPortInterface(new PortAbstractionInterface());
    PortMapInterface* portMapInterface(new PortMapInterface(
        portMapValidator, expressionParser, expressionFormatter, physicalPortInterface, logicalPortInterface));

    return portMapInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::createBridgeInterface()
//-----------------------------------------------------------------------------
TransparentBridgeInterface* BusInterfaceWizard::createBridgeInterface(
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<IPXactSystemVerilogParser> expressionParser) const
{
    TransparentBridgeInterface* bridgeInterface(
        new TransparentBridgeInterface(expressionParser, expressionFormatter));
    return bridgeInterface;
}
