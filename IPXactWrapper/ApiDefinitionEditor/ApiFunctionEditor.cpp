//-----------------------------------------------------------------------------
// File: ApiFunctionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Editor for API functions.
//-----------------------------------------------------------------------------

#include "ApiFunctionEditor.h"

#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <models/ApiFunction.h>
#include <models/ApiDefinition.h>
#include <models/ComDefinition.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::ApiFunctionEditor()
//-----------------------------------------------------------------------------
ApiFunctionEditor::ApiFunctionEditor(QWidget* parent)
    : QGroupBox(tr("Functions"), parent),
      functionList_(this),
      functionModel_(this),
      descEdit_(this),
      returnTypeCombo_(this),
      returnValueDesc_(this),
      paramView_(this),
      paramDelegate_(this),
      paramModel_(this),
      curFunc_()
{
    // Disable the function editor widgets by default
    selectionFunctionChanged(QModelIndex());

    // Set widget settings.
    returnTypeCombo_.setEditable(true);
    returnTypeCombo_.setMaximumWidth(180);
    returnTypeCombo_.setInsertPolicy(QComboBox::InsertAlphabetically);
    returnTypeCombo_.setEditText("");

    functionList_.setModel(&functionModel_);
    functionList_.setItemDelegate(new LineEditDelegate(this));

    paramView_.setModel(&paramModel_);
    paramView_.setSortingEnabled(false);
    paramView_.setItemsDraggable(false);
    paramView_.setItemDelegate(&paramDelegate_);
    paramView_.verticalHeader()->show();

    // Create the layouts.
    QVBoxLayout* funcLayout = new QVBoxLayout();
    funcLayout->addWidget(new QLabel(tr("Description:"), this));
    funcLayout->addWidget(&descEdit_);
    funcLayout->addSpacing(12);
    funcLayout->addWidget(new QLabel(tr("Return value type:"), this));
    funcLayout->addWidget(&returnTypeCombo_);
    funcLayout->addWidget(new QLabel(tr("Return value description:"), this));
    funcLayout->addWidget(&returnValueDesc_);
    funcLayout->addSpacing(12);
    funcLayout->addWidget(new QLabel(tr("Function parameters:"), this));
    funcLayout->addWidget(&paramView_, 1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&functionList_);
    layout->addSpacing(6);
    layout->addLayout(funcLayout, 1);

    connect(&paramModel_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&functionModel_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&paramView_, SIGNAL(addItem(const QModelIndex&)),
            &paramModel_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&paramView_, SIGNAL(removeItem(const QModelIndex&)),
            &paramModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(&functionList_, SIGNAL(removeItem(const QModelIndex&)),
            &functionModel_, SLOT(remove(const QModelIndex&)), Qt::UniqueConnection);
    connect(&functionList_, SIGNAL(addItem(const QModelIndex&)),
            &functionModel_, SLOT(addItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&functionList_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
            &functionModel_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

    connect(&functionList_, SIGNAL(selectionChanged(QModelIndex const&)),
            this, SLOT(selectionFunctionChanged(QModelIndex const&)), Qt::UniqueConnection);
    connect(&descEdit_, SIGNAL(textEdited(QString const&)),
            this, SLOT(descriptionEdited(QString const&)), Qt::UniqueConnection);
    connect(&returnTypeCombo_, SIGNAL(editTextChanged(QString const&)),
        this, SLOT(returnTypeEdited(QString const&)), Qt::UniqueConnection);
    connect(&returnValueDesc_, SIGNAL(textEdited(QString const&)),
        this, SLOT(returnValueDescriptionEdited(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::~ApiFunctionEditor()
//-----------------------------------------------------------------------------
ApiFunctionEditor::~ApiFunctionEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::updateDataTypes()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::updateDataTypes(QStringList const& dataTypes)
{
    paramDelegate_.updateDataTypes(dataTypes);

    QString value = returnTypeCombo_.currentText();
    returnTypeCombo_.clear();
    returnTypeCombo_.addItems(paramDelegate_.getDataTypesList());
    returnTypeCombo_.setEditText(value);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::restore()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::restore(ApiDefinition const& apiDef)
{
    functionModel_.restore(apiDef);
    selectionFunctionChanged(functionList_.currentIndex());
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::save()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::save(ApiDefinition& apiDef)
{
    functionModel_.save(apiDef);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::selectionFunctionChanged()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::selectionFunctionChanged(QModelIndex const& cur)
{
    curFunc_ = functionModel_.getFunction(cur);

    returnTypeCombo_.disconnect(this);

    if (curFunc_ != 0)
    {
        descEdit_.setText(curFunc_->getDescription());
        returnTypeCombo_.setEditText(curFunc_->getReturnValueType());
        returnValueDesc_.setText(curFunc_->getReturnValueDescription());
    }
    else
    {
        descEdit_.setText("");
        returnTypeCombo_.setEditText("");
        returnValueDesc_.setText("");
    }

    descEdit_.setEnabled(curFunc_ != 0);
    returnTypeCombo_.setEnabled(curFunc_ != 0);
    returnValueDesc_.setEnabled(curFunc_ != 0);
    paramView_.setEnabled(curFunc_ != 0);

    paramModel_.setFunction(curFunc_);

    connect(&returnTypeCombo_, SIGNAL(editTextChanged(QString const&)),
            this, SLOT(returnTypeEdited(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::descriptionEdited()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::descriptionEdited(QString const& text)
{
    if (curFunc_ != 0)
    {
        emit contentChanged();
        curFunc_->setDescription(text);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::returnTypeEdited()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::returnTypeEdited(QString const& text)
{
    if (curFunc_ != 0)
    {
        emit contentChanged();
        curFunc_->setReturnValueType(text);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::returnValueDescriptionEdited()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::returnValueDescriptionEdited(QString const& text)
{
    if (curFunc_ != 0)
    {
        emit contentChanged();
        curFunc_->setReturnValueDescription(text);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionEditor::setComDefinition()
//-----------------------------------------------------------------------------
void ApiFunctionEditor::setComDefinition(QSharedPointer<ComDefinition const> comDefinition)
{
    paramDelegate_.setComDefinition(comDefinition);
}
