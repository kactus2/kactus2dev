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

#include "BusInterfaceInterfaceFactory.h"

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

#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/AbstractionTypeInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/TransparentBridgeInterface.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileBuilderInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h>
#include <editors/ComponentEditor/parameters/ParametersInterface.h>
#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>

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

    FileSetInterface* fileSetInterface =
        createFileSetInterface(parameterFinder, expressionFormatter, expressionParser, component);
    fileSetInterface->setFileSets(component->getFileSets());

    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(expressionParser,
        component->getChoices()));

    ParametersInterface* parameterInterface =
        createParameterInterface(parameterValidator, expressionParser, expressionFormatter);

    MemoryMapInterface* mapInterface =
        createMapInterface(parameterFinder, expressionFormatter, expressionParser, parameterValidator, component);

    AbstractionTypeInterface* abstractionInterface = createAbstractionTypeInterface(
        parameterFinder, expressionFormatter, expressionParser, portMapValidator, component, library);

    TransparentBridgeInterface* bridgeInterface = createBridgeInterface(expressionFormatter, expressionParser);

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
FileSetInterface* BusInterfaceInterfaceFactory::createFileSetInterface(
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
MemoryMapInterface* BusInterfaceInterfaceFactory::createMapInterface(
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
AbstractionTypeInterface* BusInterfaceInterfaceFactory::createAbstractionTypeInterface(
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<PortMapValidator> portMapValidator,
    QSharedPointer<Component> component, LibraryInterface* library)
{
    PortMapInterface* portMapInterface = createPortMapInterface(parameterFinder, expressionFormatter,
        expressionParser, portMapValidator, component);

    QSharedPointer<AbstractionTypeValidator> validator(
        new AbstractionTypeValidator(expressionParser, component->getViews(), portMapValidator, library));

    AbstractionTypeInterface* abstractionInterface(new AbstractionTypeInterface(portMapInterface, validator));
    return abstractionInterface;
}


//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createPortMapInterface()
//-----------------------------------------------------------------------------
PortMapInterface* BusInterfaceInterfaceFactory::createPortMapInterface(
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
TransparentBridgeInterface* BusInterfaceInterfaceFactory::createBridgeInterface(
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser)
{
    TransparentBridgeInterface* bridgeInterface(
        new TransparentBridgeInterface(expressionParser, expressionFormatter));
    return bridgeInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterfaceFactory::createParameterInterface()
//-----------------------------------------------------------------------------
ParametersInterface* BusInterfaceInterfaceFactory::createParameterInterface(
    QSharedPointer<ParameterValidator> validator, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter)
{
    ParametersInterface* parameterInterface =
        new ParametersInterface(validator, expressionParser, expressionFormatter);

    return parameterInterface;
}
