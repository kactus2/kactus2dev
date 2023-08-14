//-----------------------------------------------------------------------------
// File: BusInterfaceInterfaceFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.03.2021
//
// Description:
// Constructs the bus interface interface.
//-----------------------------------------------------------------------------

#include <BusInterfaceInterfaceFactory.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <PortAbstractionInterface.h>
#include <BusInterfaceInterface.h>
#include <AbstractionTypeInterface.h>
#include <TransparentBridgeInterface.h>
#include <PortMapInterface.h>
#include <FileSetInterface.h>
#include <FileInterface.h>
#include <FileBuilderInterface.h>
#include <MemoryMapInterface.h>
#include <ParametersInterface.h>
#include <PortsInterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createBusInterface()
//-----------------------------------------------------------------------------
BusInterfaceInterface* BusInterfaceInterfaceFactory::createBusInterface(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<Component> component,
    LibraryInterface* library)
{
    QSharedPointer<PortMapValidator> portMapValidator(
        new PortMapValidator(expressionParser, component->getPorts(), library));

    AbstractionTypeInterface* abstractionInterface = Details::createAbstractionTypeInterface(
        parameterFinder, expressionFormatter, expressionParser, portMapValidator, component, library);

    return Details::createCommonBusInterfaceItems(parameterFinder, expressionFormatter, expressionParser,
        component, library, portMapValidator, abstractionInterface);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createCommonBusInterfaceItems()
//-----------------------------------------------------------------------------
BusInterfaceInterface* BusInterfaceInterfaceFactory::Details::createCommonBusInterfaceItems(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<Component> component,
    LibraryInterface* library, QSharedPointer<PortMapValidator> portMapValidator,
    AbstractionTypeInterface* abstractionInterface)
{
    FileSetInterface* fileSetInterface =
        Details::createFileSetInterface(parameterFinder, expressionFormatter, expressionParser, component);
    fileSetInterface->setFileSets(component->getFileSets());

    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(expressionParser,
        component->getChoices()));

    ParametersInterface* parameterInterface =
        Details::createParameterInterface(parameterValidator, expressionParser, expressionFormatter);

    MemoryMapInterface* mapInterface =
        Details::createMapInterface(parameterFinder, expressionFormatter, expressionParser, parameterValidator, component);

    TransparentBridgeInterface* bridgeInterface =
        Details::createBridgeInterface(expressionFormatter, expressionParser);

    QSharedPointer<BusInterfaceValidator> busValidator(new BusInterfaceValidator(expressionParser,
        component->getChoices(), component->getViews(), component->getPorts(), component->getAddressSpaces(),
        component->getMemoryMaps(), component->getBusInterfaces(), component->getFileSets(),
        component->getRemapStates(), portMapValidator, parameterValidator, library));

    BusInterfaceInterface* busInterface(new BusInterfaceInterface(busValidator, expressionParser,
        expressionFormatter));

    busInterface->setFileSetInterface(fileSetInterface);
    busInterface->setMemoryMapInterface(mapInterface);
    busInterface->setAbstractionTypeInterface(abstractionInterface);
    busInterface->setTransparentBridgeInterface(bridgeInterface);
    busInterface->setParameterInterface(parameterInterface);

    busInterface->setBusInterfaces(component);
    return busInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createFileSetInterface()
//-----------------------------------------------------------------------------
FileSetInterface* BusInterfaceInterfaceFactory::Details::createFileSetInterface(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<Component> component)
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
// Function: BusInterfaceInterfaceFactory::createMapInterface()
//-----------------------------------------------------------------------------
MemoryMapInterface* BusInterfaceInterfaceFactory::Details::createMapInterface(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterValidator> parameterValidator,
    QSharedPointer<Component> component)
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

    QSharedPointer<SubspaceMapValidator> subspaceValidator(
        new SubspaceMapValidator(expressionParser, parameterValidator));

    QSharedPointer<MemoryMapValidator> memoryMapValidator(
        new MemoryMapValidator(expressionParser, blockValidator, subspaceValidator, component));

    memoryMapValidator->componentChange(component);

    MemoryMapInterface* mapInterface =
        new MemoryMapInterface(memoryMapValidator, expressionParser, expressionFormatter);

    mapInterface->setMemoryMaps(component);

    return mapInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createAbstractionTypeInterface()
//-----------------------------------------------------------------------------
AbstractionTypeInterface* BusInterfaceInterfaceFactory::Details::createAbstractionTypeInterface(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<PortMapValidator> portMapValidator,
    QSharedPointer<Component> component, LibraryInterface* library)
{
    PortMapInterface* portMapInterface = createPortMapInterface(parameterFinder, expressionFormatter,
        expressionParser, portMapValidator, component);

    return createCommonAbstractionTypeInterface(
        expressionParser, component, portMapValidator, library, portMapInterface);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createCommonAbstractionTypeInterface()
//-----------------------------------------------------------------------------
AbstractionTypeInterface* BusInterfaceInterfaceFactory::Details::createCommonAbstractionTypeInterface(
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<Component> component,
    QSharedPointer<PortMapValidator> portMapValidator, LibraryInterface* library,
    PortMapInterface* portMapInterface)
{
    QSharedPointer<AbstractionTypeValidator> validator(
        new AbstractionTypeValidator(expressionParser, component->getViews(), portMapValidator, library));

    AbstractionTypeInterface* abstractionInterface(
        new AbstractionTypeInterface(portMapInterface, validator, library));
    return abstractionInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createPortMapInterface()
//-----------------------------------------------------------------------------
PortMapInterface* BusInterfaceInterfaceFactory::Details::createPortMapInterface(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<PortMapValidator> portMapValidator,
    QSharedPointer<Component> component)
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
// Function: BusInterfaceInterfaceFactory::createBridgeInterface()
//-----------------------------------------------------------------------------
TransparentBridgeInterface* BusInterfaceInterfaceFactory::Details::createBridgeInterface(
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser)
{
    TransparentBridgeInterface* bridgeInterface(
        new TransparentBridgeInterface(expressionParser, expressionFormatter));
    return bridgeInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createParameterInterface()
//-----------------------------------------------------------------------------
ParametersInterface* BusInterfaceInterfaceFactory::Details::createParameterInterface(
    QSharedPointer<ParameterValidator> validator, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter)
{
    ParametersInterface* parameterInterface =
        new ParametersInterface(validator, expressionParser, expressionFormatter);

    return parameterInterface;
}

