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

#include <SystemDesign/SystemDesignWidget.h>

#include <common/DesignDiagram.h>

#include <models/SystemView.h>
#include <models/designconfiguration.h>
#include <models/design.h>

#include <LibraryManager/libraryinterface.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::SystemDetailsEditor()
//-----------------------------------------------------------------------------
SystemDetailsEditor::SystemDetailsEditor(LibraryInterface* handler, QWidget *parent)
    : QWidget(parent),
      handler_(handler),
      hwRefEditor_(VLNV::COMPONENT, handler, parent, parent),
      viewSelector_(this),
      applyButton_(tr("Apply"), this),
      revertButton_(tr("Revert"), this),
      removeMappingButton_(tr("Remove HW"), this),
      component_(),
      designWidget_(NULL),
      systemView_(0)
{
    hwRefEditor_.setTitle(tr("HW component reference"));

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

        disconnect(&viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
                   this, SLOT(onViewRefChanged(const QString&)));
    }

    // connect the new design.
    designWidget_ = designWidget;
    connect(designWidget_, SIGNAL(refreshed()),
            this, SLOT(onRefresh()), Qt::UniqueConnection);

    hwRefEditor_.setEnabled(!locked);
    viewSelector_.setEnabled(!locked);
    removeMappingButton_.setEnabled(!locked);

    // Update the HW reference VLNV and view combo box based on the component whose design is being edited.
    component_ = designWidget->getEditedComponent();

    hwRefEditor_.setVLNV(component_->getVlnv());
    
    viewSelector_.clear();
    viewSelector_.addItems(component_->getHierViews());
    
    systemView_ = component_->findSystemView(designWidget->getOpenViewName());

    if (systemView_ != 0)
    {
        viewSelector_.setCurrentIndex(component_->getHierViews().indexOf(systemView_->getHWViewRef()));
    }

    connect(&viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
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

        disconnect(&viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
                   this, SLOT(onViewRefChanged(const QString&)));
    }

    hwRefEditor_.setVLNV(VLNV());
    hwRefEditor_.setDisabled(true);

    viewSelector_.clear();
    viewSelector_.setDisabled(true);

    applyButton_.setDisabled(true);
    revertButton_.setDefault(true);
    removeMappingButton_.setDisabled(true);
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
    hwRefEditor_.setEnabled(!locked);
    viewSelector_.setEnabled(!locked);
    removeMappingButton_.setEnabled(!locked);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::setupLayout()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::setupLayout()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(&applyButton_);
    buttonLayout->addWidget(&revertButton_);

    QFormLayout* selectorLayout = new QFormLayout();
    selectorLayout->addRow(tr("Selected HW view:"), &viewSelector_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&hwRefEditor_);
    topLayout->addLayout(selectorLayout);
    topLayout->addLayout(buttonLayout);
    topLayout->addWidget(&removeMappingButton_);
    topLayout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::setupConnections()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::setupConnections()
{
    connect(&hwRefEditor_, SIGNAL(vlnvEdited()), this, SLOT(onHWRefChanged()), Qt::UniqueConnection);
    connect(&applyButton_, SIGNAL(clicked()), this, SLOT(applyHW()), Qt::UniqueConnection);
    connect(&revertButton_, SIGNAL(clicked()), this, SLOT(revert()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::onHWRefChanged()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::onHWRefChanged()
{
    if (hwRefEditor_.isValid())
    {
        QSharedPointer<LibraryComponent const> libComp = handler_->getModelReadOnly(hwRefEditor_.getVLNV());
        QSharedPointer<Component const> component = libComp.dynamicCast<Component const>();

        disconnect(&viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onViewRefChanged(const QString&)));

        viewSelector_.clear();

        if (component != 0)
        {
            viewSelector_.addItems(component->getHierViews());
        }
        
        connect(&viewSelector_, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onViewRefChanged(const QString&)), Qt::UniqueConnection);
    }

    bool modified = hwRefEditor_.getVLNV() != *component_->getVlnv() ||
                    viewSelector_.currentText() != systemView_->getHWViewRef();
    applyButton_.setEnabled(modified && viewSelector_.count() > 0);
    revertButton_.setEnabled(modified);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::onViewRefChanged()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::onViewRefChanged(QString const& viewRef)
{
    bool modified = hwRefEditor_.getVLNV() != *component_->getVlnv() ||
                    viewSelector_.currentText() != systemView_->getHWViewRef();

    applyButton_.setEnabled(modified && viewSelector_.count() > 0);
    revertButton_.setEnabled(modified);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::applyHW()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::applyHW()
{
    VLNV componentVLNV = hwRefEditor_.getVLNV();

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

        // Ask the user whether to move or copy the design under the given HW.
        SwitchHWDialog dialog(newComponent, designWidget_->getOpenViewName(), handler_, this);

        if (dialog.exec() == QDialog::Rejected)
        {
            return;
        }

        // Based on the action, either perform copy or move.
        SystemView* newView = new SystemView();
        newView->setName(dialog.getViewName());
        newView->setHWViewRef(viewSelector_.currentText());

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
        designWidget_->setDesign(componentVLNV, dialog.getViewName());
        designWidget_->refresh();

        component_ = designWidget_->getEditedComponent();
        systemView_ = component_->findSystemView(dialog.getViewName());
    }
    else
    {
        systemView_->setHWViewRef(viewSelector_.currentText());
        designWidget_->refresh();
    }

    applyButton_.setDisabled(true);
    revertButton_.setDisabled(true);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::revert()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::revert()
{
    hwRefEditor_.setVLNV(component_->getVlnv());

    if (systemView_ != 0)
    {
        viewSelector_.setCurrentIndex(component_->getHierViews().indexOf(systemView_->getHWViewRef()));
    }

    applyButton_.setDisabled(true);
    revertButton_.setDisabled(true);
}
