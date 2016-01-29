//-----------------------------------------------------------------------------
// File: ComponentInstantiationDisplayer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.01.2016
//
// Description:
// Display widget for component instantiations.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationDisplayer.h"

#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <QFormLayout>
//-----------------------------------------------------------------------------
// Function: ComponentInstantiationDisplayer::ComponentInstantiationDisplayer()
//-----------------------------------------------------------------------------
ComponentInstantiationDisplayer::ComponentInstantiationDisplayer(QWidget* parent):
QGroupBox(tr("Implementation details"), parent), 
    instantiation_(QSharedPointer<ComponentInstantiation>()),
    languageLabel_(new QLabel(this)),
    libraryLabel_(new QLabel(this)),
    packageLabel_(new QLabel(this)),
    moduleNameLabel_(new QLabel(this)),
    architectureLabel_(new QLabel(this)),
    configurationLabel_(new QLabel(this)),
    filesetsLabel_(new QLabel(this))
{
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationDisplayer::~ComponentInstantiationDisplayer()
//-----------------------------------------------------------------------------
ComponentInstantiationDisplayer::~ComponentInstantiationDisplayer()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationDisplayer::refresh()
//-----------------------------------------------------------------------------
void ComponentInstantiationDisplayer::refresh()
{    
    if (instantiation_)
    {
        QString language = instantiation_->getLanguage();
        if (instantiation_->isLanguageStrict())
        {
            language.append(" (strict)");
        }

        languageLabel_->setText(language);
        libraryLabel_->setText(instantiation_->getLibraryName());
        packageLabel_->setText(instantiation_->getPackageName());
        moduleNameLabel_->setText(instantiation_->getModuleName());
        architectureLabel_->setText(instantiation_->getArchitectureName());
        configurationLabel_->setText(instantiation_->getConfigurationName());
        filesetsLabel_->setText(instantiation_->getFileSetReferences()->join(", "));
    }

    setVisible(instantiation_);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationDisplayer::setInstantiation()
//-----------------------------------------------------------------------------
void ComponentInstantiationDisplayer::setInstantiation(QSharedPointer<ComponentInstantiation> instantiation)
{
    instantiation_ = instantiation;
    refresh();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationDisplayer::setupLayout()
//-----------------------------------------------------------------------------
void ComponentInstantiationDisplayer::setupLayout()
{
    QFormLayout* implementationLayout = new QFormLayout(this);

    implementationLayout->addRow(tr("Language:"), languageLabel_);
    implementationLayout->addRow(tr("Library:"), libraryLabel_);
    implementationLayout->addRow(tr("Package:"), packageLabel_);
    implementationLayout->addRow(tr("Module name:"), moduleNameLabel_);
    implementationLayout->addRow(tr("Architecture:"), architectureLabel_);
    implementationLayout->addRow(tr("Configuration:"), configurationLabel_);
    implementationLayout->addRow(tr("File sets:"), filesetsLabel_);
}
