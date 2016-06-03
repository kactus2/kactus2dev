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

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

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

    BusInterfaceWizardBusEditorPage::SignalNamingPolicy namingPolicy = BusInterfaceWizardBusEditorPage::NAME;
    if (descriptionAsLogicalName)
    {
        namingPolicy = BusInterfaceWizardBusEditorPage::DESCRIPTION;
    }

    QSharedPointer<BusInterfaceValidator> validator =
        createBusInterfaceValidator(component, expressionParser, handler);

    BusInterfaceWizardGeneralOptionsPage* optionsPage = new BusInterfaceWizardGeneralOptionsPage(component, busIf,
        handler, !absDefVLNV.isValid(), parameterFinder, expressionFormatter, expressionParser, validator, this);

    connect(optionsPage, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(optionsPage, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    setPage(PAGE_INTRO, new BusInterfaceWizardIntroPage(this));
    setPage(PAGE_GENERALOPTIONS, optionsPage);
    setPage(PAGE_BUSDEFINITION, new BusInterfaceWizardBusEditorPage(component, busIf, handler, portNames, 
        this, absDefVLNV, expressionParser, namingPolicy));
    setPage(PAGE_PORTMAPS, new BusInterfaceWizardPortMapPage(component, busIf, handler, portNames,
        expressionParser, expressionFormatter, parameterFinder, validator, this));
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
    QSharedPointer<Component> component, QSharedPointer<ExpressionParser> parser, LibraryInterface* handler)
{
    QSharedPointer<PortMapValidator> portMapValidator(new PortMapValidator(parser, component->getPorts(), handler));

    QSharedPointer<ParameterValidator2014> parameterValidator(
        new ParameterValidator2014(parser, component->getChoices()));

    QSharedPointer<BusInterfaceValidator> validator = QSharedPointer<BusInterfaceValidator>(
        new BusInterfaceValidator(parser, component->getChoices(), component->getViews(), component->getPorts(),
        component->getAddressSpaces(), component->getMemoryMaps(), component->getBusInterfaces(),
        component->getFileSets(), component->getRemapStates(), portMapValidator, parameterValidator, handler));

    return validator;
}
