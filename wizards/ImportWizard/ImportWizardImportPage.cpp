//-----------------------------------------------------------------------------
// File: ImportWizardImportPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.10.2014
//
// Description:
// Import wizard page for importing elements from a file.
//-----------------------------------------------------------------------------

#include "ImportWizardImportPage.h"
#include "ImportWizardPages.h"

//-----------------------------------------------------------------------------
// Function: ImportWizardImportPage::ImportWizardImportPage()
//-----------------------------------------------------------------------------
ImportWizardImportPage::ImportWizardImportPage(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ComponentParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, BusInterfaceInterface* busInterface, QWidget* parent):
ComponentWizardImportPage(component, handler, parameterFinder, expressionFormatter, busInterface, parent)
{
    setFinalPage(false);
}

//-----------------------------------------------------------------------------
// Function: ImportWizardImportPage::~ImportWizardImportPage()
//-----------------------------------------------------------------------------
ImportWizardImportPage::~ImportWizardImportPage()
{
}

//-----------------------------------------------------------------------------
// Function: ImportWizardImportPage::nextId()
//-----------------------------------------------------------------------------
int ImportWizardImportPage::nextId() const
{
    return ImportWizardPages::INSTANCES;
}
