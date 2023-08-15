//-----------------------------------------------------------------------------
// File: PeripheralTemplatesDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.05.2023
//
// Description:
// Dialog for editing templates for python peripherals in Renode generator.
//-----------------------------------------------------------------------------

#include "PeripheralTemplatesDialog.h"

#include <Plugins/RenodeGenerator/CPUDialog/TemplateEditor/PeripheralTemplateEditor.h>

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: PeripheralTemplatesDialog::PeripheralTemplatesDialog()
//-----------------------------------------------------------------------------
PeripheralTemplatesDialog::PeripheralTemplatesDialog(QString const& generationFolder, QWidget* parent):
QDialog(parent),
templatesEditor_(new PeripheralTemplateEditor(generationFolder, this))
{
    setWindowTitle("Peripheral Templates");

    setupLayout();

    setMinimumWidth(800);
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplatesDialog::setupLayout()
//-----------------------------------------------------------------------------
void PeripheralTemplatesDialog::setupLayout()
{
    auto dialogButtons(new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal));
    QPushButton* okButton = dialogButtons->addButton("Ok", QDialogButtonBox::AcceptRole);
    okButton->setDefault(true);
    okButton->setAutoDefault(true);
    
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
    
    auto layout(new QVBoxLayout(this));
    layout->addWidget(templatesEditor_, 1);
    layout->addWidget(dialogButtons);
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplatesDialog::setupTemplates()
//-----------------------------------------------------------------------------
void PeripheralTemplatesDialog::setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates)
{
    templatesEditor_->setupTemplates(newTemplates);
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplatesDialog::getTemplates()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > PeripheralTemplatesDialog::getTemplates() const
{
    return templatesEditor_->getTemplates();
}
