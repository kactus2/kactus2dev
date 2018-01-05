//-----------------------------------------------------------------------------
// File: ListEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 30.05.2012
//
// Description:
// Editor for a list of items.
//-----------------------------------------------------------------------------

#include "listeditor.h"

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: listeditor::ListEditor()
//-----------------------------------------------------------------------------
ListEditor::ListEditor(QWidget *parent):
QWidget(parent),
model_(this),
view_(this)
{
	// the signals from the view
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(remove(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(addItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
		&model_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

	// the signals from the model
	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	QHBoxLayout* topLayout = new QHBoxLayout(this);
	topLayout->addWidget(&view_);
	topLayout->setContentsMargins(0, 0, 0, 0);

	// connect the model to the view
	view_.setModel(&model_);
}

//-----------------------------------------------------------------------------
// Function: listeditor::~ListEditor()
//-----------------------------------------------------------------------------
ListEditor::~ListEditor()
{

}

//-----------------------------------------------------------------------------
// Function: listeditor::items()
//-----------------------------------------------------------------------------
const QStringList& ListEditor::items() const
{
	return model_.items();
}

//-----------------------------------------------------------------------------
// Function: listeditor::setItems()
//-----------------------------------------------------------------------------
void ListEditor::setItems( const QStringList& items )
{
	model_.setItems(items);
}

//-----------------------------------------------------------------------------
// Function: listeditor::size()
//-----------------------------------------------------------------------------
int ListEditor::size() const
{
	return model_.rowCount();
}

//-----------------------------------------------------------------------------
// Function: listeditor::setItemDelegate()
//-----------------------------------------------------------------------------
void ListEditor::setItemDelegate( QStyledItemDelegate* delegate )
{
	view_.setItemDelegate(delegate);
}
