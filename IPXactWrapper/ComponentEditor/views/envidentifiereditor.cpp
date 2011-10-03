/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: envidentifiereditor.cpp
 */

#include "envidentifiereditor.h"

#include "envidentifiersdelegate.h"

#include <QHeaderView>
#include <QStringList>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>

EnvIdentifierEditor::EnvIdentifierEditor(View* view, 
										 QWidget *parent, 
										 const QString title): 
QGroupBox(title, parent),
addRowButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeRowButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
view_(this), model_(view, this), 
proxy_(this)  {

	//setMinimumHeight(150);

	// set view to be sortable
	view_.setSortingEnabled(true);
	view_.horizontalHeader()->setStretchLastSection(true);
	view_.horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setAlternatingRowColors(true);
	//view_.setSelectionBehavior(QAbstractItemView::SelectRows);
	view_.verticalHeader()->hide();
	view_.setEditTriggers(QAbstractItemView::AllEditTriggers);
	view_.setWordWrap(true);

	view_.setItemDelegate(new EnvIdentifiersDelegate(this));

	view_.setProperty("mandatoryField", true);

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&addRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&removeRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->addLayout(buttonLayout);

	restoreChanges();

	connect(&addRowButton_, SIGNAL(clicked(bool)),
		&model_, SLOT(onAddRow()), Qt::UniqueConnection);
	connect(&removeRowButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRemove()), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}

EnvIdentifierEditor::~EnvIdentifierEditor() {
}

void EnvIdentifierEditor::restoreChanges() {
	model_.restore();
}

void EnvIdentifierEditor::applyChanges() {
	model_.apply();
}

bool EnvIdentifierEditor::isValid() const {
	return model_.isValid();
}

void EnvIdentifierEditor::onRemove() {
	QModelIndex index = proxy_.mapToSource(view_.currentIndex());

	// if index is valid then remove the row
	if (index.isValid())
		model_.onRemoveRow(index.row());
}
