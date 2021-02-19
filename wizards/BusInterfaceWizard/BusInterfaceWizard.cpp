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

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

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

    QSharedPointer<BusInterfaceValidator> busValidator =
        createBusInterfaceValidator(component, expressionParser, parameterValidator, handler);

    QSharedPointer<PortValidator> portValidator(new PortValidator(expressionParser, component->getViews()));
    QSharedPointer<PortMapValidator> portMapValidator =
        busValidator->getAbstractionValidator()->getPortMapValidator();

    PortsInterface* physicalPortInterface(
        new PortsInterface(portValidator, expressionParser, expressionFormatter));
    physicalPortInterface->setPorts(component);

    PortAbstractionInterface* logicalPortInterface(new PortAbstractionInterface());

    PortMapInterface* portMapInterface(new PortMapInterface(
        portMapValidator, expressionParser, expressionFormatter, physicalPortInterface, logicalPortInterface));

    BusInterfaceWizardGeneralOptionsPage* optionsPage =
        new BusInterfaceWizardGeneralOptionsPage(component, busIf, handler, !absDefVLNV.isValid(), parameterFinder,
        expressionFormatter, expressionParser, busValidator, this);

    connect(optionsPage, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(optionsPage, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    setPage(PAGE_INTRO, new BusInterfaceWizardIntroPage(this));
    setPage(PAGE_GENERALOPTIONS, optionsPage);
    setPage(PAGE_BUSDEFINITION, new BusInterfaceWizardBusDefinitionEditorPage(component, busIf, handler, portNames, 
        this, absDefVLNV, expressionParser, namingPolicy));
    setPage(PAGE_PORTMAPS, new BusInterfaceWizardPortMapPage(component, busIf, handler, portNames,
        expressionParser, parameterFinder, busValidator, portMapInterface, this));
    setPage(PAGE_SUMMARY, new BusInterfaceWizardConclusionPage(busIf, portNames, this));
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::~BusInterfaceWizard()
//-----------------------------------------------------------------------------
BusInterfaceWizard::~BusInterfaceWizard()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::createBusInterfaceValidator()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterfaceValidator> BusInterfaceWizard::createBusInterfaceValidator(
    QSharedPointer<Component> component, QSharedPointer<ExpressionParser> parser,
    QSharedPointer<ParameterValidator> parameterValidator, LibraryInterface* handler)
{
    QSharedPointer<PortMapValidator> portMapValidator(new PortMapValidator(parser, component->getPorts(), handler));

    QSharedPointer<BusInterfaceValidator> validator = QSharedPointer<BusInterfaceValidator>(
        new BusInterfaceValidator(parser, component->getChoices(), component->getViews(), component->getPorts(),
        component->getAddressSpaces(), component->getMemoryMaps(), component->getBusInterfaces(),
        component->getFileSets(), component->getRemapStates(), portMapValidator, parameterValidator, handler));

    return validator;
}
