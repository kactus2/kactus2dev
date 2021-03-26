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
#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterfaceFactory.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>

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

    BusInterfaceInterface* busInterface = BusInterfaceInterfaceFactory::createBusInterface(
        parameterFinder, expressionFormatter, expressionParser, component, handler);

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
