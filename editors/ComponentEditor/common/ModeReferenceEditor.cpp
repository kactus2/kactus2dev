//-----------------------------------------------------------------------------
// File: ModeReferenceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 10.8.2023
//
// Description:
// Editor for mode references.
//-----------------------------------------------------------------------------

#include "ModeReferenceEditor.h"

#include <QMap>
#include <QVBoxLayout>
#include <QScrollArea>

#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::ModeReferenceEditor()
//-----------------------------------------------------------------------------
ModeReferenceEditor::ModeReferenceEditor(QList<QPair<QString, int> > modeRefs, QWidget* parent) :
QFrame(parent),
model_(new ModeReferenceModel(modeRefs, this)),
view_(new EditableTableView(this))
{
    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
    setAutoFillBackground(true);

    auto delegate = new ModeReferenceDelegate(this);
    auto proxy = new QSortFilterProxyModel(this);

    view_->setSortingEnabled(true);
    proxy->setSourceModel(model_);
    view_->setModel(proxy);
    view_->setItemDelegate(delegate);
    view_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    setupLayout();

    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width());

    connect(view_, SIGNAL(addItem(QModelIndex const&)), 
        model_, SLOT(onAddRow(QModelIndex const&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(QModelIndex const&)), 
        model_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::setModeRefs()
//-----------------------------------------------------------------------------
void ModeReferenceEditor::setModeRefs(QList<QPair<QString, int> > modeRefs) const
{
    model_->setModeRefs(modeRefs);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::getModeRefs()
//-----------------------------------------------------------------------------
QList<QPair<QString, int> > ModeReferenceEditor::getModeRefs() const
{
    return model_->getModeRefs();
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceEditor::setupLayout()
//-----------------------------------------------------------------------------
void ModeReferenceEditor::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    QScrollArea* scrollArea = new QScrollArea();

    topLayout->addWidget(scrollArea);
    scrollArea->setWidget(view_);

    QPushButton* okButton = new QPushButton();
    okButton->setIcon(QIcon(":/icons/common/graphics/checkMark.png"));
    okButton->setToolTip("Accept");

    QPushButton* cancelButton = new QPushButton();
    cancelButton->setIcon(QIcon(":/icons/common/graphics/grey_cross.png"));
    cancelButton->setToolTip("Cancel");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(10);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    topLayout->addLayout(buttonLayout, 0);

    connect(okButton, SIGNAL(clicked()), this, SIGNAL(finishEditing()), Qt::UniqueConnection);
    connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(cancelEditing()), Qt::UniqueConnection);
}
