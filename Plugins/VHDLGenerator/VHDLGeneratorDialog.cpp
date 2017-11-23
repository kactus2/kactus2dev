//-----------------------------------------------------------------------------
// File: VHDLGeneratorDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2017
//
// Description:
// Dialog for setting VHDL generation options.
//-----------------------------------------------------------------------------

#include "VHDLGeneratorDialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorDialog::VHDLGeneratorDialog()
//-----------------------------------------------------------------------------
VHDLGeneratorDialog::VHDLGeneratorDialog(QSharedPointer<ViewSelection> viewConfiguration, 
    QString const& defaultPath,
    QWidget* parent): QDialog(parent),
    viewSelector_(new ViewSelectionWidget(viewConfiguration, this)),
    fileEditor_(new QLineEdit(this))
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    fileEditor_->setText(defaultPath);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorDialog::~VHDLGeneratorDialog()
//-----------------------------------------------------------------------------
VHDLGeneratorDialog::~VHDLGeneratorDialog()
{
}


//-----------------------------------------------------------------------------
// Function: VHDLGeneratorDialog::getOutputPath()
//-----------------------------------------------------------------------------
QString VHDLGeneratorDialog::getOutputPath() const
{
    return fileEditor_->text();
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorDialog::accept()
//-----------------------------------------------------------------------------
void VHDLGeneratorDialog::accept()
{
    if (fileEditor_->text().isEmpty())
    {
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorDialog::onBrowse()
//-----------------------------------------------------------------------------
void VHDLGeneratorDialog::onBrowse()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Select output file for generation"), 
        fileEditor_->text(), 
        QStringLiteral("VHDL files (.vhd)"));

    if (!filePath.isEmpty())
    {
        fileEditor_->setText(filePath);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorDialog::setupLayout()
//-----------------------------------------------------------------------------
void VHDLGeneratorDialog::setupLayout()
{
    QPushButton* browseButton = new QPushButton(tr("Browse..."), this);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->addWidget(new QLabel(tr("Output file:"), this));
    fileLayout->addWidget(fileEditor_);
    fileLayout->addWidget(browseButton);
    
    QGroupBox* settingsGroup = new QGroupBox(tr("Settings"), this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);  

    settingsLayout->addWidget(viewSelector_);
    settingsLayout->addLayout(fileLayout);

    QDialogButtonBox* buttons = new QDialogButtonBox(Qt::Horizontal, this);
    buttons->addButton(tr("Write file"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);

    QVBoxLayout* topLayout = new QVBoxLayout(this);  
    topLayout->addWidget(settingsGroup);
    topLayout->addWidget(buttons);

    connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
}
