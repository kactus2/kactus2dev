/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: parametereditor.cpp
 *		Project: Kactus 2
 */

#include "parametereditor.h"

#include <common/delegates/lineeditdelegate.h>

#include <QVBoxLayout>

ParameterEditor::ParameterEditor( QList<QSharedPointer<Parameter> >* parameters, 
								 QWidget *parent):
QWidget(parent),
view_(this), 
model_(parameters, this),
proxy_(this) {

	Q_ASSERT(parameters);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		this, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		this, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new LineEditDelegate(this));

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	
	restore();
}

ParameterEditor::~ParameterEditor() {
}

bool ParameterEditor::isValid() const {
	return model_.isValid();
}

void ParameterEditor::restore() {
	model_.restore();
}

void ParameterEditor::apply() {
	model_.apply();
}

void ParameterEditor::onAddItem( const QModelIndex& index ) {
	model_.onAddItem(proxy_.mapToSource(index));
}

void ParameterEditor::onRemoveItem( const QModelIndex& index ) {
	model_.onRemoveItem(proxy_.mapToSource(index));
}
