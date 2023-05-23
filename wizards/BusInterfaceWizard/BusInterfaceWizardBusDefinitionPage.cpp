//-----------------------------------------------------------------------------
// File: BusInterfaceWizardBusDefinitionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for editing bus and abstraction definitions.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizardBusDefinitionPage.h"

#include "BusInterfaceWizard.h"

#include <editors/BusDefinitionEditor/busdefgroup.h>
#include <editors/ComponentEditor/ports/portsdelegate.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/common/Vector.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::BusInterfaceWizardBusDefinitionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardBusDefinitionEditorPage::BusInterfaceWizardBusDefinitionEditorPage(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, LibraryInterface* lh, bool editable, BusInterfaceWizard* parent):
QWizardPage(parent),
    handler_(lh),
    component_(component),
    busIf_(busIf),
    editor_(this, handler_, nullptr),
    editable_(editable)
{       
    setTitle(tr("Bus Definition"));
    setSubTitle(tr("Verify the options in the bus definition."));
    setFinalPage(false);

    connect(&editor_, SIGNAL(portRenamed(QString const&, QString const&)), 
        this, SLOT(portRenamed(QString const&, QString const&)), Qt::UniqueConnection);
    connect(&editor_, SIGNAL(portRemoved(QString const&, const General::InterfaceMode)), 
        this, SLOT(portRemoved(QString const&, const General::InterfaceMode)), Qt::UniqueConnection);
    
    setupLayout();    
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::nextId()
//-----------------------------------------------------------------------------
int BusInterfaceWizardBusDefinitionEditorPage::nextId() const
{
    return BusInterfaceWizard::PAGE_ABSDEFINITION;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::initializePage()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusDefinitionEditorPage::initializePage()
{   
    editor_.setBusDef(handler_->getModel<BusDefinition>(busIf_->getBusType()));
    editor_.setProtection(!editable_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::validatePage()
//-----------------------------------------------------------------------------
bool BusInterfaceWizardBusDefinitionEditorPage::validatePage()
{    
    QVector<QString> errors;
    bool valid = editor_.validate(errors);

    if (!valid)
    {
        QMessageBox warningDialog(QMessageBox::Warning, tr("Warning"),
            tr("Bus definition has the following error(s):\n") + QStringList(errors.toList()).join("\n"),
            QMessageBox::Ok, this);        
        warningDialog.exec();
        return false;
    }

    editor_.save();

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusDefinitionEditorPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&editor_);
}
