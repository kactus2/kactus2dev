//-----------------------------------------------------------------------------
// File: SystemDetailsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.7.2012
//
// Description:
// Docking editor for system details.
//-----------------------------------------------------------------------------

#include "SystemDetailsEditor.h"

#include "SwitchHWDialog.h"
#include "ExportSWDialog.h"

#include <SystemDesign/SystemDesignWidget.h>

#include <common/DesignDiagram.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>

#include <models/SystemView.h>
#include <models/designconfiguration.h>
#include <models/design.h>
#include <models/SWInstance.h>

#include <LibraryManager/libraryinterface.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::SystemDetailsEditor()
//-----------------------------------------------------------------------------
SystemDetailsEditor::SystemDetailsEditor(LibraryInterface* handler, QWidget *parent)
    : QWidget(parent),
      handler_(handler),
      hwRefEditor_(new VLNVEditor(VLNV::COMPONENT, handler, parent, parent->parentWidget())),
      viewSelector_(new QComboBox(this)),
      applyButton_(new QPushButton(tr("Apply"), this)),
      revertButton_(new QPushButton(tr("Revert"), this)),
      removeMappingButton_(new QPushButton(tr("Remove HW"), this)),
      exportButton_(new QPushButton(tr("Export SW"), this)),
      component_(),
      designWidget_(NULL),
      systemView_(0)
{
    hwRefEditor_->setTitle(tr("HW component reference"));

    setupLayout();
    setupConnections();
    clear();
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::~SystemDetailsEditor()
//-----------------------------------------------------------------------------
SystemDetailsEditor::~SystemDetailsEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::setSystem()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::setSystem(DesignWidget* designWidget, bool locked)
{
    // if there was a previous design being displayed.
    if (designWidget_)
    {
        designWidget_->disconnect(this);

        disconnect(viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
                   this, SLOT(onViewRefChanged(const QString&)));
    }

    // connect the new design.
    designWidget_ = designWidget;
    connect(designWidget_, SIGNAL(refreshed()),
            this, SLOT(onRefresh()), Qt::UniqueConnection);
    
    // Update the HW reference VLNV and view combo box based on the component whose design is being edited.
    component_ = designWidget->getEditedComponent();
    systemView_ = component_->findSystemView(designWidget->getOpenViewName());

    // Clear the editors initially.
    hwRefEditor_->setVLNV(VLNV());
    viewSelector_->clear();

    // Fill them only if the component is not a system component (system component is always unmapped).
    if (component_->getComponentImplementation() != KactusAttribute::KTS_SYS)
    {
        hwRefEditor_->setVLNV(component_->getVlnv());
        viewSelector_->addItems(component_->getHierViews());

        if (systemView_ != 0)
        {
            viewSelector_->setCurrentIndex(component_->getHierViews().indexOf(systemView_->getHWViewRef()));
        }
    }

    hwRefEditor_->setEnabled(!locked);
    viewSelector_->setEnabled(!locked);
    removeMappingButton_->setEnabled(!locked && component_->getComponentImplementation() != KactusAttribute::KTS_SYS);
    exportButton_->setEnabled(component_->getComponentImplementation() != KactusAttribute::KTS_SYS);
    revertButton_->setDisabled(true);

    connect(viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onViewRefChanged(const QString&)), Qt::UniqueConnection);

    // display this widget
    parentWidget()->raise();
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::clear()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::clear()
{
    if (designWidget_)
    {
        designWidget_->disconnect(this);
        designWidget_ = 0;
        systemView_ = 0;

        disconnect(viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
                   this, SLOT(onViewRefChanged(const QString&)));
    }

    hwRefEditor_->setVLNV(VLNV());
    hwRefEditor_->setDisabled(true);

    viewSelector_->clear();
    viewSelector_->setDisabled(true);

    applyButton_->setDisabled(true);
    revertButton_->setDisabled(true);
    removeMappingButton_->setDisabled(true);
    exportButton_->setDisabled(true);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::onRefresh()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::onRefresh()
{
    setSystem(designWidget_, designWidget_->isProtected());
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::setLocked()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::setLocked(bool locked)
{
    hwRefEditor_->setEnabled(!locked);
    viewSelector_->setEnabled(!locked);
    removeMappingButton_->setEnabled(!locked && component_->getComponentImplementation() != KactusAttribute::KTS_SYS);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::setupLayout()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::setupLayout()
{
    QGridLayout* buttonLayout = new QGridLayout();
    buttonLayout->addWidget(applyButton_, 0, 0);
    buttonLayout->addWidget(revertButton_, 0, 1);
    buttonLayout->addWidget(exportButton_, 1, 0);
    buttonLayout->addWidget(removeMappingButton_, 1, 1);

    QFormLayout* selectorLayout = new QFormLayout();
    selectorLayout->addRow(tr("HW component view:"), viewSelector_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(hwRefEditor_);
    topLayout->addLayout(selectorLayout);
    topLayout->addLayout(buttonLayout);
    topLayout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::setupConnections()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::setupConnections()
{
    connect(hwRefEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHWRefChanged()), Qt::UniqueConnection);
    connect(applyButton_, SIGNAL(clicked()), this, SLOT(applyHW()), Qt::UniqueConnection);
    connect(revertButton_, SIGNAL(clicked()), this, SLOT(revert()), Qt::UniqueConnection);
    connect(removeMappingButton_, SIGNAL(clicked()), this, SLOT(removeMapping()), Qt::UniqueConnection);
    connect(exportButton_, SIGNAL(clicked()), this, SLOT(exportSW()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::onHWRefChanged()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::onHWRefChanged()
{
    disconnect(viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
               this, SLOT(onViewRefChanged(const QString&)));

    viewSelector_->clear();

    if (hwRefEditor_->isValid())
    {
        QSharedPointer<LibraryComponent const> libComp = handler_->getModelReadOnly(hwRefEditor_->getVLNV());
        QSharedPointer<Component const> component = libComp.dynamicCast<Component const>();

        if (component != 0)
        {
            viewSelector_->addItems(component->getHierViews());
        }
    }

    connect(viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onViewRefChanged(const QString&)), Qt::UniqueConnection);

    bool modified = hwRefEditor_->getVLNV() != *component_->getVlnv() ||
                    viewSelector_->currentText() != systemView_->getHWViewRef();
    applyButton_->setEnabled(modified && viewSelector_->count() > 0);
    revertButton_->setEnabled(modified);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::onViewRefChanged()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::onViewRefChanged(QString const& viewRef)
{
    bool modified = hwRefEditor_->getVLNV() != *component_->getVlnv() ||
                    viewSelector_->currentText() != systemView_->getHWViewRef();

    applyButton_->setEnabled(modified && viewSelector_->count() > 0);
    revertButton_->setEnabled(modified);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::applyHW()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::applyHW()
{
    VLNV componentVLNV = hwRefEditor_->getVLNV();

    // Check if the component is being switched.
    if (componentVLNV != *component_->getVlnv())
    {
        // Verify that the component is found.
        QSharedPointer<LibraryComponent> libComp = handler_->getModel(componentVLNV);
        QSharedPointer<Component> newComponent = libComp.dynamicCast<Component>();

        if (newComponent == 0)
        {
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("No HW component found with the given VLNV."),
                               QMessageBox::Ok, (QWidget*)parent());
            msgBox.exec();
            return;
        }

        // Determine a default suggestion for the view name.
        QSettings settings;
        QStringList suggestions = settings.value("Policies/SysViewNames").toStringList();

        QString baseViewName = "";
        QString viewName = "";

        if (!suggestions.isEmpty())
        {
            baseViewName = suggestions.first();

            viewName = baseViewName;
            unsigned int runningNumber = 1;

            while (newComponent->findSystemView(viewName) != 0)
            {
                ++runningNumber;
                viewName = baseViewName + QString::number(runningNumber);
            }
        }

        // Ask the user whether to move or copy the design under the given HW.
        SwitchHWDialog dialog(newComponent, viewName, handler_, this);
        dialog.setViewNameSuggestions(suggestions);

        if (dialog.exec() == QDialog::Rejected)
        {
            return;
        }

        // Based on the action, either perform copy or move.
        SystemView* newView = new SystemView(dialog.getSystemViewName());
        newView->setHWViewRef(viewSelector_->currentText());

        if (dialog.isCopyActionSelected())
        {
            // Save copies of the design and design configuration.
            VLNV vlnv = dialog.getVLNV();

            VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
                            vlnv.getName() + ".sysdesign", vlnv.getVersion());
            VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
                             vlnv.getName() + ".sysdesigncfg", vlnv.getVersion());

            QSharedPointer<DesignConfiguration>
                desConf(new DesignConfiguration(*designWidget_->getDiagram()->getDesignConfiguration()));
            desConf->setVlnv(desConfVLNV);
            desConf->setDesignRef(designVLNV);

            QSharedPointer<Design> design = designWidget_->getDiagram()->createDesign(designVLNV);

            handler_->writeModelToFile(dialog.getPath(), design);
            handler_->writeModelToFile(dialog.getPath(), desConf);

            // Set the new system view to point to the newly-saved design configuration.
            newView->setHierarchyRef(desConfVLNV);
        }
        else
        {
            // Move is performed by removing the system view from the previous HW component and using
            // the existing VLNV for the hierarchy reference.
            newView->setHierarchyRef(systemView_->getHierarchyRef());
            
            systemView_ = 0;
            component_->removeSystemView(designWidget_->getOpenViewName());
            handler_->writeModelToFile(component_);
        }

        // Add the system view to the new HW component and save.
        newComponent->addSystemView(newView);
        handler_->writeModelToFile(newComponent);

        // Refresh the design widget.
        designWidget_->setDesign(componentVLNV, dialog.getSystemViewName());
        designWidget_->setProtection(false);
        designWidget_->refresh();

        component_ = designWidget_->getEditedComponent();
        systemView_ = component_->findSystemView(dialog.getSystemViewName());
    }
    else
    {
        systemView_->setHWViewRef(viewSelector_->currentText());
        designWidget_->refresh();
    }

    applyButton_->setDisabled(true);
    revertButton_->setDisabled(true);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::revert()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::revert()
{
    if (component_->getComponentImplementation() != KactusAttribute::KTS_SYS)
    {
        hwRefEditor_->setVLNV(component_->getVlnv());
        onHWRefChanged();

        if (systemView_ != 0)
        {
            viewSelector_->setCurrentIndex(component_->getHierViews().indexOf(systemView_->getHWViewRef()));
        }
    }
    else
    {
        hwRefEditor_->setVLNV(VLNV());
        onHWRefChanged();
    }

    applyButton_->setDisabled(true);
    revertButton_->setDisabled(true);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::removeMapping()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::removeMapping()
{
    // Ask the user a VLNV for a system component.
    NewObjectDialog dialog(handler_, VLNV::COMPONENT, false, this);
    dialog.setWindowTitle(tr("Set System Component VLNV"));

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create a new system view for the system design.
    SystemView* newView = new SystemView(designWidget_->getOpenViewName());
    newView->setHierarchyRef(systemView_->getHierarchyRef());

    // Remove the system view from the previous HW component.
    systemView_ = 0;
    component_->removeSystemView(designWidget_->getOpenViewName());
    handler_->writeModelToFile(component_);

    // Create a new system component and add the system view to it.
    QSharedPointer<Component> newComponent(new Component(dialog.getVLNV()));
    newComponent->setComponentImplementation(KactusAttribute::KTS_SYS);

    newComponent->addSystemView(newView);
    handler_->writeModelToFile(dialog.getPath(), newComponent);

    // Refresh the design widget.
    designWidget_->setDesign(dialog.getVLNV(), designWidget_->getOpenViewName());
    designWidget_->setProtection(false);
    designWidget_->refresh();

    component_ = designWidget_->getEditedComponent();
    systemView_ = component_->findSystemView(designWidget_->getOpenViewName());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::exportSW()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::exportSW()
{
    // Ask the user a VLNV for a system component.
    ExportSWDialog dialog(handler_, this);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    handler_->beginSave();

    // Save copies of the design and design configuration.
    VLNV vlnv = dialog.getVLNV();

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
                    vlnv.getName() + ".sysdesign", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
                     vlnv.getName() + ".sysdesigncfg", vlnv.getVersion());

    QSharedPointer<DesignConfiguration>
        desConf(new DesignConfiguration(*designWidget_->getDiagram()->getDesignConfiguration()));
    desConf->setVlnv(desConfVLNV);
    desConf->setDesignRef(designVLNV);

    QSharedPointer<Design> design = designWidget_->getDiagram()->createDesign(designVLNV);

    // Export only SW instances.
    design->setComponentInstances(QList<ComponentInstance>());

    QList<SWInstance> swInstances = design->getSWInstances();

    for (int i = 0; i < swInstances.size(); ++i)
    {
        swInstances[i].setMapping("");
    }

    design->setSWInstances(swInstances);

    handler_->writeModelToFile(dialog.getPath(), design);
    handler_->writeModelToFile(dialog.getPath(), desConf);

    // Create a new system view for the system design.
    SystemView* newView = new SystemView("system");
    newView->setHierarchyRef(desConfVLNV);

    // Create a new system component and add the system view to it.
    QSharedPointer<Component> newComponent(new Component(vlnv));
    newComponent->setComponentImplementation(KactusAttribute::KTS_SYS);

    newComponent->addSystemView(newView);
    handler_->writeModelToFile(dialog.getPath(), newComponent);

    // Refresh the design widget.
//     designWidget_->setDesign(dialog.getVLNV(), designWidget_->getOpenViewName());
//     designWidget_->setProtection(false);
//     designWidget_->refresh();

    handler_->endSave();
}
