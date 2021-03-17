//-----------------------------------------------------------------------------
// File: PortmapDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 10.6.2011
//
// Description:
// Port map dialog.
//-----------------------------------------------------------------------------

#include "PortmapDialog.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/BusInterfacePortMapTab.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>

#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/AbstractionTypeInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/AbstractionTypeInterface.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/TransparentBridgeInterface.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/interfaces/PortMapInterface.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileSetInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileInterface.h>
#include <editors/ComponentEditor/fileSet/interfaces/FileBuilderInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/ResetInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/FieldInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/AddressBlockInterface.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: PortmapDialog()
//-----------------------------------------------------------------------------
PortmapDialog::PortmapDialog(LibraryInterface* library, QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, QSharedPointer<BusInterface> otherBusIf, QWidget* parent) :
QDialog(parent),
busIf_(busIf),
otherBusIf_(otherBusIf)
{
    Q_ASSERT(library != 0);
    Q_ASSERT(component != 0);
    Q_ASSERT(busIf != 0);
    Q_ASSERT(otherBusIf != 0);

    setWindowTitle(tr("Define Port Maps"));

    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(component));
    QSharedPointer<ExpressionFormatter> expressionFormatter(new ExpressionFormatter(parameterFinder));
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    BusInterfaceInterface* busInterface =
        createBusInterface(parameterFinder, expressionFormatter, expressionParser, component, library);

    // Create the port map widget.
    portmapWidget_ = new BusInterfacePortMapTab(library,  component, busInterface, busIf->name().toStdString(),
        expressionParser, parameterFinder, busInterface->getAbstractionTypeInterface()->getPortMapInterface(),
        this);

    portmapWidget_->setAbstractionDefinitions();

    // Create a separator.
    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    // Create the buttons and their layout.
    QPushButton* btnOk = new QPushButton(tr("OK") , this);
    QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnOk);
    buttonLayout->addWidget(btnCancel);

    // Create the main layout.
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(portmapWidget_);
    mainLayout->addWidget(separator);
    mainLayout->addLayout(buttonLayout);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}

//-----------------------------------------------------------------------------
// Function: ~PortmapDialog()
//-----------------------------------------------------------------------------
PortmapDialog::~PortmapDialog()
{

}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void PortmapDialog::accept()
{
    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: PortmapDialog::createBusInterface()
//-----------------------------------------------------------------------------
BusInterfaceInterface* PortmapDialog::createBusInterface(QSharedPointer<ParameterFinder> parameterFinder,
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
// Function: PortmapDialog::createFileSetInterface()
//-----------------------------------------------------------------------------
FileSetInterface* PortmapDialog::createFileSetInterface(QSharedPointer<ParameterFinder> parameterFinder,
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
// Function: PortmapDialog::createMapInterface()
//-----------------------------------------------------------------------------
MemoryMapInterface* PortmapDialog::createMapInterface(QSharedPointer<ParameterFinder> parameterFinder,
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
// Function: PortmapDialog::creaetAbstractionTypeInterface()
//-----------------------------------------------------------------------------
AbstractionTypeInterface* PortmapDialog::creaetAbstractionTypeInterface(
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
// Function: PortmapDialog::createPortMapInterface()
//-----------------------------------------------------------------------------
PortMapInterface* PortmapDialog::createPortMapInterface(QSharedPointer<ParameterFinder> parameterFinder,
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
// Function: PortmapDialog::createBridgeInterface()
//-----------------------------------------------------------------------------
TransparentBridgeInterface* PortmapDialog::createBridgeInterface(
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<IPXactSystemVerilogParser> expressionParser) const
{
    TransparentBridgeInterface* bridgeInterface(
        new TransparentBridgeInterface(expressionParser, expressionFormatter));
    return bridgeInterface;
}
