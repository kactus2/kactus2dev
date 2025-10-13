//-----------------------------------------------------------------------------
// File: InstanceInterfacesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 12.08.2025
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "InstanceInterfacesEditor.h"
#include <common/widgets/instanceWidgets/InstanceWidgets.h>

#include <common/widgets/EnumCollectionEditor/InterfaceEnumEditor.h>

#include <QLabel>
#include <QPushButton>
#include <QtGlobal>

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::InstanceInterfacesEditor()
//-----------------------------------------------------------------------------
InstanceInterfacesEditor::InstanceInterfacesEditor(ExpressionSet expressionSet, ComponentParameterModel* parameterCompleterModel, Document::Revision docRevision, QWidget* parent) :
    QFrame(parent),
    instanceNameLabel_(new QLabel("Instance name:")),
    instanceSelectorCombo_(new InstanceComboBox()),
    removeInstanceButton_(new QPushButton(QIcon(":/icons/common/graphics/cross.png"), "Remove")),
    interfacesEditor_(new InterfaceEnumEditor(expressionSet, parameterCompleterModel, docRevision))
{
    setLayout(&topLayout_);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    buttonRowLayout_.addWidget(instanceNameLabel_);
    buttonRowLayout_.addWidget(instanceSelectorCombo_);
    buttonRowLayout_.addWidget(removeInstanceButton_);

    topLayout_.addLayout(&buttonRowLayout_);
    topLayout_.addWidget(interfacesEditor_);

    connect(instanceSelectorCombo_, &QComboBox::currentIndexChanged, this, &InstanceInterfacesEditor::onInstanceSelected, Qt::UniqueConnection);
    connect(removeInstanceButton_, SIGNAL(clicked()), this, SIGNAL(instanceRemoved()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::updateAvailableInstances()
//-----------------------------------------------------------------------------
void InstanceInterfacesEditor::updateAvailableInstances(QSet<QString> const& availableInterfaces)
{
    auto currentSelection = instanceSelectorCombo_->currentText();
    instanceSelectorCombo_->clear();
    instanceSelectorCombo_->addItems(QStringList(availableInterfaces.cbegin(), availableInterfaces.cend()));
    
    // If there was a previous selection, add and select it again. Otherwise just pick the first option
    if (currentSelection.isEmpty() == false)
    {
        instanceSelectorCombo_->addItem(currentSelection);
        instanceSelectorCombo_->setCurrentText(currentSelection);
    }
    else
    {
        instanceSelectorCombo_->setCurrentIndex(0);
    }
}

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::getSelectedInstance()
//-----------------------------------------------------------------------------
QString InstanceInterfacesEditor::getSelectedInstance() const
{
    return instanceSelectorCombo_->currentText();
}

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::addInterfaceItems()
//-----------------------------------------------------------------------------
void InstanceInterfacesEditor::addInterfaceItems(const QList<InterfaceEnumEditor::InterfaceInput>& items, General::InterfaceMode interfaceMode)
{
    interfacesToAdd_.insert(interfaceMode, items);
}

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::createInterfaceItems()
//-----------------------------------------------------------------------------
void InstanceInterfacesEditor::createInterfaceItems(bool isChannel)
{
    if (interfacesToAdd_.contains(General::InterfaceMode::INITIATOR))
    {
        interfacesEditor_->addItems(interfacesToAdd_.value(General::InterfaceMode::INITIATOR), 
                General::InterfaceMode::INITIATOR, isChannel);
    }

    if (interfacesToAdd_.contains(General::InterfaceMode::TARGET))
    {
        interfacesEditor_->addItems(interfacesToAdd_.value(General::InterfaceMode::TARGET),
            General::InterfaceMode::TARGET, isChannel);
    }

    if (interfacesToAdd_.contains(General::InterfaceMode::MIRRORED_INITIATOR))
    {
        interfacesEditor_->addItems(interfacesToAdd_.value(General::InterfaceMode::MIRRORED_INITIATOR),
            General::InterfaceMode::MIRRORED_INITIATOR, isChannel);
    }

    if (interfacesToAdd_.contains(General::InterfaceMode::MIRRORED_TARGET))
    {
        interfacesEditor_->addItems(interfacesToAdd_.value(General::InterfaceMode::MIRRORED_TARGET),
            General::InterfaceMode::MIRRORED_TARGET, isChannel);
    }

    // Add stretch to editor if needed (determined in interfaces editor)
    interfacesEditor_->addStretchIfNeeded();
}

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::getSelectedInterfaces()
//-----------------------------------------------------------------------------
QList<InterfaceEnumEditor::InterfaceData> InstanceInterfacesEditor::getSelectedInterfaces() const
{
    return interfacesEditor_->getSelectedInterfaces();
}

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::clearInterfaceItems()
//-----------------------------------------------------------------------------
void InstanceInterfacesEditor::clearInterfaceItems()
{
    interfacesEditor_->clearAll();
    interfacesToAdd_.clear();
}

//-----------------------------------------------------------------------------
// Function: InstanceInterfacesEditor::onInstanceSelected()
//-----------------------------------------------------------------------------
void InstanceInterfacesEditor::onInstanceSelected()
{
    emit instanceSelected(instanceSelectorCombo_->currentText(), previousInstanceSelection_);
    previousInstanceSelection_ = instanceSelectorCombo_->currentText();
}
