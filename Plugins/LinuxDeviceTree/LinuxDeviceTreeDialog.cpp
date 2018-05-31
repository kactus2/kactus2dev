//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.05.2018
//
// Description:
// Dialog for setting linux device tree generation options.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeDialog.h"

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>

#include <QFileDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::LinuxDeviceTreeDialog()
//-----------------------------------------------------------------------------
LinuxDeviceTreeDialog::LinuxDeviceTreeDialog(QString const& defaultPath, QSharedPointer<Component> component,
    QSharedPointer<Design> design, QWidget* parent):
QDialog(parent),
viewSelector_(new QComboBox(this)),
fileSetGroup_(new QGroupBox(tr("Add file to fileset"))),
fileSetSelector_(new QComboBox(this)),
fileEditor_(new QLineEdit(this))
{
    setWindowTitle("Linux Device Tree generator");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    fileEditor_->setText(defaultPath);

    fileSetGroup_->setCheckable(true);
    fileSetGroup_->setChecked(true);
    fileSetSelector_->setEditable(true);

    setupViewSelector(component, design);
    setupFileSetSelector(component->getFileSets());

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::~LinuxDeviceTreeDialog()
//-----------------------------------------------------------------------------
LinuxDeviceTreeDialog::~LinuxDeviceTreeDialog()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::getOutputPath()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreeDialog::getOutputPath() const
{
    return fileEditor_->text();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::getSelectedView()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreeDialog::getSelectedView() const
{
    return viewSelector_->currentText();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::saveFileToFileSet()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeDialog::saveFileToFileSet() const
{
    return fileSetGroup_->isChecked();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::getTargetFileSet()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreeDialog::getTargetFileSet() const
{
    return fileSetSelector_->currentText();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::accept()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeDialog::accept()
{
    if (fileEditor_->text().isEmpty())
    {
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::onBrowse()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeDialog::onBrowse()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Select output file for generation"),
        fileEditor_->text(), QStringLiteral("DTS files (.dts)"));

    if (!filePath.isEmpty())
    {
        fileEditor_->setText(filePath);
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::setupViewSelector()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeDialog::setupViewSelector(QSharedPointer<Component> component, QSharedPointer<Design> design)
{
    int currentViewIndex = 0;
    bool currentViewFound = false;
    for (int i = 0; i < component->getViews()->size(); ++i)
    {
        QSharedPointer<View> view = component->getViews()->at(i);
        viewSelector_->addItem(view->name());

        if (design)
        {
            QString instantiationReference = view->getDesignInstantiationRef();
            if (!currentViewFound && !instantiationReference.isEmpty())
            {
                foreach(QSharedPointer<DesignInstantiation> instantiation, *component->getDesignInstantiations())
                {
                    VLNV instantiationVLNVReference(*instantiation->getDesignReference());
                    if (instantiationVLNVReference.isValid() && instantiation->name() == instantiationReference &&
                        instantiationVLNVReference == design->getVlnv())
                    {
                        currentViewIndex = i;
                        currentViewFound = true;
                    }
                }
            }
        }
    }

    if (currentViewFound)
    {
        viewSelector_->setCurrentIndex(currentViewIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::setupFileSetSelector()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeDialog::setupFileSetSelector(
    QSharedPointer<QList<QSharedPointer<FileSet> > > componentFileSets)
{
    bool deviceTreeFileSetFound = false;
    QString deviceTreeSetName("deviceTree");

    for (int i = 0; i < componentFileSets->size(); ++i)
    {
        QSharedPointer<FileSet> fileSet = componentFileSets->at(i);
        fileSetSelector_->addItem(fileSet->name());

        if (!deviceTreeFileSetFound && fileSet->name() == deviceTreeSetName)
        {
            deviceTreeFileSetFound = true;
            fileSetSelector_->setCurrentIndex(i);
        }
    }

    if (!deviceTreeFileSetFound)
    {
        fileSetSelector_->addItem(deviceTreeSetName);
        fileSetSelector_->setCurrentIndex(componentFileSets->size());
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::setupLayout()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeDialog::setupLayout()
{
    QFormLayout* viewLayout = new QFormLayout();
    viewLayout->addRow(tr("Select view:"), viewSelector_);

    QFormLayout* fileSetLayout = new QFormLayout();
    fileSetLayout->addRow(tr("Select file set:"), fileSetSelector_);
    fileSetGroup_->setLayout(fileSetLayout);

    QPushButton* browseButton = new QPushButton(tr("Browse..."), this);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->addWidget(new QLabel(tr("Output file:"), this));
    fileLayout->addWidget(fileEditor_);
    fileLayout->addWidget(browseButton);

    QGroupBox* settingsGroup = new QGroupBox(tr("Settings"), this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);

    settingsLayout->addLayout(viewLayout);
    settingsLayout->addWidget(fileSetGroup_);
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
