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

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUEditor.h>

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
    QSharedPointer<Design> design, LibraryInterface* library, QWidget* parent):
QDialog(parent),
library_(library),
topComponent_(component),
viewSelector_(new QComboBox(this)),
fileSetGroup_(new QGroupBox(tr("Add file to fileset"))),
fileSetSelector_(new QComboBox(this)),
fileEditor_(new QLineEdit(this)),
writeBlocks_(new QCheckBox("Write address blocks")),
cpuEditor_(new LinuxDeviceTreeCPUEditor(this)),
graphFactory_(library),
searchAlgorithm_()
{
    setWindowTitle("Linux Device Tree generator");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    fileEditor_->setText(defaultPath);
    fileEditor_->setToolTip(defaultPath);

    fileSetGroup_->setCheckable(true);
    fileSetGroup_->setChecked(true);
    fileSetSelector_->setEditable(true);

    setupViewSelector(component, design);
    setupFileSetSelector(component->getFileSets());

    setupCPUEditor(viewSelector_->currentText());

    connect(viewSelector_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(setupCPUEditor(QString const&)), Qt::UniqueConnection);

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
// Function: LinuxDeviceTreeDialog::allowAddressBlocks()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeDialog::allowAddressBlocks() const
{
    return writeBlocks_->isChecked();
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
    QString newDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), fileEditor_->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!newDir.isEmpty())
    {
        fileEditor_->setText(newDir);
        fileEditor_->setToolTip(newDir);
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
// Function: LinuxDeviceTreeDialog::setupCPUEditor()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeDialog::setupCPUEditor(QString const& view)
{
    QSharedPointer<ConnectivityGraph> graph =
        graphFactory_.createConnectivityGraph(topComponent_, view);

    QVector < QSharedPointer<ConnectivityInterface> > masterRoots = searchAlgorithm_.findMasterSlaveRoots(graph);

    QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > cpuContainers =
        LinuxDeviceTreeCPUDetails::getCPUContainers(topComponent_->getVlnv().getName(), masterRoots, library_);

    cpuEditor_->setupCPUDetails(cpuContainers);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::getDesignName()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreeDialog::getDesignName()
{
    QString viewName = viewSelector_->currentText();
    for (auto designView : *topComponent_->getViews())
    {
        if (designView->name() == viewName)
        {
            QString instantiationReference = designView->getDesignInstantiationRef();
            if (!instantiationReference.isEmpty())
            {
                for (auto instantiation : *topComponent_->getDesignInstantiations())
                {
                    if (instantiation->name() == instantiationReference)
                    {
                        QSharedPointer<ConfigurableVLNVReference> designVLNV =
                            instantiation->getDesignReference();
                        if (designVLNV)
                        {
                            return designVLNV->getName();
                        }

                        break;
                    }
                }
            }
            else
            {
                instantiationReference = designView->getDesignConfigurationInstantiationRef();
                if (!instantiationReference.isEmpty())
                {
                    for (auto instantiation : *topComponent_->getDesignConfigurationInstantiations())
                    {
                        if (instantiation->name() == instantiationReference)
                        {
                            QSharedPointer<ConfigurableVLNVReference> designConfigurationVLNV =
                                instantiation->getDesignConfigurationReference();

                            QSharedPointer<const Document> designDocument =
                                library_->getModelReadOnly(*designConfigurationVLNV.data());
                            if (designDocument)
                            {
                                QSharedPointer<const DesignConfiguration> designConfiguration =
                                    designDocument.dynamicCast<const DesignConfiguration>();
                                if (designConfiguration)
                                {
                                    return designConfiguration->getDesignRef().getName();
                                }
                            }

                            break;
                        }
                    }
                }
            }
        }
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::setupLayout()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeDialog::setupLayout()
{
    QFormLayout* viewLayout = new QFormLayout();
    viewLayout->addRow(tr("Select view:"), viewSelector_);

    viewLayout->addRow(writeBlocks_);

    QFormLayout* fileSetLayout = new QFormLayout();
    fileSetLayout->addRow(tr("Select file set:"), fileSetSelector_);
    fileSetGroup_->setLayout(fileSetLayout);

    QPushButton* openFolderButton(
        new QPushButton(QIcon(":icons/common/graphics/folder-horizontal-open.png"), QString(), this));

    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->addWidget(new QLabel(tr("Output file:"), this));
    fileLayout->addWidget(fileEditor_);
    fileLayout->addWidget(openFolderButton);

    QGroupBox* settingsGroup = new QGroupBox(tr("Settings"), this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);

    settingsLayout->addLayout(viewLayout);
    settingsLayout->addLayout(fileLayout);
    settingsLayout->addWidget(fileSetGroup_);

    QDialogButtonBox* buttons = new QDialogButtonBox(Qt::Horizontal, this);
    buttons->addButton(tr("Write file"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);

    QHBoxLayout* editorLayout = new QHBoxLayout();
    editorLayout->addWidget(settingsGroup, 2);
    editorLayout->addWidget(cpuEditor_, 3);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(editorLayout);
    topLayout->addWidget(buttons);

    connect(openFolderButton, SIGNAL(clicked()), this, SLOT(onBrowse()), Qt::UniqueConnection);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeDialog::getAcceptedContainers()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > LinuxDeviceTreeDialog::getAcceptedContainers()
const
{
    return cpuEditor_->getAcceptedContainers();
}
