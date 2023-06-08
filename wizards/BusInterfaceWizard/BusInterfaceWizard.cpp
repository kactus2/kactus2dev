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
#include "BusInterfaceWizardAbsDefinitionPage.h"
#include "BusInterfaceWizardPortMapPage.h"
#include "BusInterfaceWizardConclusionPage.h"

#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>
#include <KactusAPI/include/PortMapInterface.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::BusInterfaceWizard()
//-----------------------------------------------------------------------------
BusInterfaceWizard::BusInterfaceWizard(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf,
    LibraryInterface* handler, QStringList portNames, QWidget* parent, VLNV absDefVLNV,
    bool descriptionAsLogicalName) :
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

    BusInterfaceWizardAbsDefinitionPage::SignalNamingPolicy namingPolicy =
        BusInterfaceWizardAbsDefinitionPage::NAME;

    if (descriptionAsLogicalName)
    {
        namingPolicy = BusInterfaceWizardAbsDefinitionPage::DESCRIPTION;
    }

    BusInterfaceInterface* busInterface = BusInterfaceInterfaceFactory::createBusInterface(
        parameterFinder, expressionFormatter, expressionParser, component, handler);

    AbstractionTypeInterface* absTypeInterface = busInterface->getAbstractionTypeInterface();
    if (absTypeInterface)
    {
        PortMapInterface* portMapInterface = absTypeInterface->getPortMapInterface();
        if (portMapInterface)
        {
            BusInterfaceWizardGeneralOptionsPage* optionsPage = new BusInterfaceWizardGeneralOptionsPage(
                component, busIf, handler, !absDefVLNV.isValid(), parameterFinder, expressionFormatter,
                expressionParser, busInterface, this);

            connect(optionsPage, SIGNAL(increaseReferences(QString)),
                this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
            connect(optionsPage, SIGNAL(decreaseReferences(QString)),
                this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

            setPage(PAGE_INTRO, new BusInterfaceWizardIntroPage(this));
            setPage(PAGE_GENERALOPTIONS, optionsPage);
            setPage(PAGE_BUSDEFINITION, new BusInterfaceWizardBusDefinitionEditorPage(
                component, busIf, handler, absDefVLNV.isValid(), this));
            setPage(PAGE_ABSDEFINITION, new BusInterfaceWizardAbsDefinitionPage(
                component, busIf, handler, portNames, this, absDefVLNV, expressionParser, namingPolicy));
            setPage(PAGE_PORTMAPS, new BusInterfaceWizardPortMapPage(component, busIf, handler, portNames,
                expressionParser, parameterFinder, busInterface, portMapInterface, this));
            setPage(PAGE_SUMMARY, new BusInterfaceWizardConclusionPage(busIf, portNames, this));
        }
    }
}
