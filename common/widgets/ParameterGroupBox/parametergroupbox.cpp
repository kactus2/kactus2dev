/* 
 *  	Created on: 23.2.2012
 *      Author: Antti Kamppi
 * 		filename: parametergroupbox.cpp
 *		Project: Kactus 2
 */

#include "parametergroupbox.h"

#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <QVBoxLayout>

ParameterGroupBox::ParameterGroupBox(QList<QSharedPointer<Parameter> >* parameters,
									 QWidget *parent):
QGroupBox(tr("Parameters"), parent),
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

ParameterGroupBox::~ParameterGroupBox() {
}

bool ParameterGroupBox::isValid() const {
	return model_.isValid();
}

void ParameterGroupBox::restore() {
	model_.restore();
}

void ParameterGroupBox::apply() {
	model_.apply();
}

void ParameterGroupBox::onAddItem( const QModelIndex& index ) {
	model_.onAddItem(proxy_.mapToSource(index));
}

void ParameterGroupBox::onRemoveItem( const QModelIndex& index ) {
	model_.onRemoveItem(proxy_.mapToSource(index));
}
