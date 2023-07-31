//-----------------------------------------------------------------------------
// File: ChoicesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.11.2014
//
// Description:
// Editor for component choices.
//-----------------------------------------------------------------------------

#include "ChoicesEditor.h"

#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/choices/ChoicesModel.h>
#include <editors/ComponentEditor/choices/EnumerationModel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Choice.h>

#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QGroupBox>

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::ChoicesEditor()
//-----------------------------------------------------------------------------
ChoicesEditor::ChoicesEditor(QSharedPointer<Component> component, QSharedPointer<ChoiceValidator> validator,
    QWidget* parent /* = 0 */):
ItemEditor(component, 0, parent),
choiceList_(this),
choiceModel_(new ChoicesModel(component->getChoices(), validator, this)),
enumerationView_(new EditableTableView(this)),
enumerationModel_(new EnumerationModel(this))
{
    choiceList_.setModel(choiceModel_);
    choiceList_.setItemDelegate(new LineEditDelegate(this));

    QSortFilterProxyModel* enumerationProxy(new QSortFilterProxyModel(this));
    enumerationProxy->setSourceModel(enumerationModel_);

    enumerationView_->setItemsDraggable(false);
    enumerationView_->setSortingEnabled(true);
    enumerationView_->setModel(enumerationProxy);
    enumerationView_->setDisabled(true); // Set disabled before a choice has been selected or created.

	connect(&choiceList_, SIGNAL(removeItem(const QModelIndex&)), choiceModel_, SLOT(remove(const QModelIndex&)), Qt::UniqueConnection);
	connect(&choiceList_, SIGNAL(addItem(const QModelIndex&)), choiceModel_, SLOT(addItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&choiceList_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)), choiceModel_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

	connect(&choiceList_, SIGNAL(selectionChanged(QModelIndex const&)),
		this, SLOT(selectionChoiceChanged(QModelIndex const&)), Qt::UniqueConnection);

    connect(choiceModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(choiceModel_, SIGNAL(choicesEmpty(bool)), this, SLOT(onChoiceListEmpty(bool)), Qt::UniqueConnection);

    connect(enumerationModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(enumerationModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(enumerationView_, SIGNAL(addItem(const QModelIndex&)), enumerationModel_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(enumerationView_, SIGNAL(removeItem(const QModelIndex&)), enumerationModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::selectionFunctionChanged()
//-----------------------------------------------------------------------------
void ChoicesEditor::selectionChoiceChanged(QModelIndex const& newIndex)
{
    QSharedPointer<Choice> currentChoice = choiceModel_->getChoice(newIndex);

    if (currentChoice)
    {
        enumerationModel_->setupEnumerations(currentChoice->enumerations());
        enumerationView_->setEnabled(true);
    }
    else
    {
        enumerationModel_->clearEnumerations();
    }
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::isValid()
//-----------------------------------------------------------------------------
bool ChoicesEditor::isValid() const
{
    return choiceModel_->validate();
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::refresh()
//-----------------------------------------------------------------------------
void ChoicesEditor::refresh()
{

}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::onChoiceListEmpty()
//-----------------------------------------------------------------------------
void ChoicesEditor::onChoiceListEmpty(bool isEmpty)
{
    enumerationView_->setDisabled(isEmpty);
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::showEvent()
//-----------------------------------------------------------------------------
void ChoicesEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/choices.html");
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::setupLayout()
//-----------------------------------------------------------------------------
void ChoicesEditor::setupLayout()
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Choices"), this);

    // create the layout, add widgets to it
    QVBoxLayout* choiceGroupLayout(new QVBoxLayout());
    choiceGroupLayout->addWidget(&choiceList_);

    QGroupBox* choiceGroup(new QGroupBox("Choice list"));
    choiceGroup->setLayout(choiceGroupLayout);

	QVBoxLayout* enumerationGroupLayout(new QVBoxLayout());
	enumerationGroupLayout->addWidget(enumerationView_);

	QGroupBox* enumerationGroup(new QGroupBox("Enumerations"));
    enumerationGroup->setLayout(enumerationGroupLayout);

    QHBoxLayout* choicesLayout(new QHBoxLayout());
    choicesLayout->addWidget(choiceGroup);
	choicesLayout->addWidget(enumerationGroup, 4);

    QVBoxLayout* mainLayout(new QVBoxLayout(this));
    mainLayout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    mainLayout->addLayout(choicesLayout);
	mainLayout->setContentsMargins(0, 0, 0, 0);
}
