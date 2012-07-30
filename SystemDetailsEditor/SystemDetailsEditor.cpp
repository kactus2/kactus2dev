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

#include <SystemDesign/SystemDesignWidget.h>
#include <models/SystemView.h>

#include <QHBoxLayout>
#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::SystemDetailsEditor()
//-----------------------------------------------------------------------------
SystemDetailsEditor::SystemDetailsEditor(LibraryInterface* handler, QWidget *parent)
    : QWidget(parent),
      handler_(handler),
      hwRefEditor_(VLNV::COMPONENT, handler, parent, parent),
      viewSelector_(this),
      removeMappingButton_(tr("Remove HW"), this),
      component_(),
      designWidget_(NULL),
      systemView_(0)
{
    hwRefEditor_.setTitle(tr("HW reference"));

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

    // get the component being edited
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
    QFormLayout* selectorLayout = new QFormLayout();
    selectorLayout->addRow(tr("Selected HW view:"), &viewSelector_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&hwRefEditor_);
    topLayout->addLayout(selectorLayout);
    topLayout->addWidget(&removeMappingButton_);
    topLayout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::setupConnections()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::setupConnections()
{
}

//-----------------------------------------------------------------------------
// Function: SystemDetailsEditor::onViewRefChanged()
//-----------------------------------------------------------------------------
void SystemDetailsEditor::onViewRefChanged(QString const& viewRef)
{
    systemView_->setHWViewRef(viewRef);
    designWidget_->refresh();
    emit contentChanged();
}
