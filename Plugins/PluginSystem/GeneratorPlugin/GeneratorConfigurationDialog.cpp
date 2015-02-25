//-----------------------------------------------------------------------------
// File: GeneratorConfigurationDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Dialog for setting file generation options.
//-----------------------------------------------------------------------------

#include "GeneratorConfigurationDialog.h"
#include "GeneratorConfiguration.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::GeneratorConfigurationDialog(QSharedPointer<GeneratorConfiguration> configuration, 
    QWidget *parent)
    : QDialog(parent), configuration_(), addToFileset_(new QCheckBox(this)), 
    viewSelection_(new QComboBox(this)), pathEditor_(new QLineEdit(this))
{    
    setWindowTitle(tr("Configure file generation"));

    QFormLayout* filesetLayout = new QFormLayout();
    filesetLayout->addRow(tr("Add file to fileset"), addToFileset_);

    filesetLayout->addRow(tr("Select view"), viewSelection_);

    QHBoxLayout* pathSelectionLayout = new QHBoxLayout();
    pathSelectionLayout->addWidget(new QLabel(tr("Select output file")));
    pathSelectionLayout->addWidget(pathEditor_);
   
    QPushButton* browseButton = new QPushButton(tr("Browse"), this);
    pathSelectionLayout->addWidget(browseButton);

    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal, this);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(filesetLayout);
    topLayout->addLayout(pathSelectionLayout);
    topLayout->addStretch(1);
    topLayout->addWidget(dialogButtons);

    loadConfiguration(configuration);

    connect(addToFileset_, SIGNAL(stateChanged(int)), 
        this, SLOT(onFileSetStateChanged(int)), Qt::UniqueConnection);
    connect(viewSelection_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onViewChanged(QString const&)), Qt::UniqueConnection);
    connect(pathEditor_, SIGNAL(editingFinished()), this, SLOT(onPathEdited()), Qt::UniqueConnection);
    connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
}


//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::~GeneratorConfigurationDialog()
//-----------------------------------------------------------------------------
GeneratorConfigurationDialog::~GeneratorConfigurationDialog()
{

}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::loadConfiguration()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::loadConfiguration(QSharedPointer<GeneratorConfiguration> configuration)
{
    configuration_ = configuration;

    addToFileset_->setChecked(configuration->getSaveToFileset() != 0);
    pathEditor_->setText(configuration_->getOutputPath());
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::setViewNames()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::setViewNames(QStringList const& viewNames)
{
    viewSelection_->clear();
    viewSelection_->addItems(viewNames);
    viewSelection_->setCurrentIndex(0);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::accept()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::accept()
{
    if (pathEditor_->text().isEmpty())
    {
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onFileSetStateChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onFileSetStateChanged(int state)
{
    bool addToFileset = state;
    configuration_->setSaveToFileset(addToFileset);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onPathEdited()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onPathEdited()
{
    configuration_->setOutputPath(pathEditor_->text());
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onBrowse()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onBrowse()
{
    QString selectedPath = QFileDialog::getSaveFileName(this,
        tr("Select output file for generation"), pathEditor_->text(),
        tr("Verilog files (*.v)"));

    if (!selectedPath.isEmpty())
    {
        pathEditor_->setText(selectedPath);
        configuration_->setOutputPath(selectedPath);
    }
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfigurationDialog::onViewChanged()
//-----------------------------------------------------------------------------
void GeneratorConfigurationDialog::onViewChanged(QString const& selectedView)
{
    configuration_->setActiveView(selectedView);
}
