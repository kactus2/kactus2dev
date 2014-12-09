/* 
 *  	Created on: 23.2.2012
 *      Author: Antti Kamppi
 * 		filename: parametergroupbox.cpp
 *		Project: Kactus 2
 */

#include "parametergroupbox.h"

#include <editors/ComponentEditor/parameters/ParameterDelegate.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/component.h>

#include <QVBoxLayout>

ParameterGroupBox::ParameterGroupBox(QList<QSharedPointer<Parameter> >& parameters,
                                     QSharedPointer<Component> component,
									 QWidget *parent):
QGroupBox(tr("Parameters"), parent),
view_(this), 
model_(parameters, component->getChoices(),
    QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(component)), this),
proxy_(this)
{

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new ParameterDelegate(component->getChoices(), this));

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
}

ParameterGroupBox::~ParameterGroupBox() {
}

bool ParameterGroupBox::isValid() const {
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::isValid()
//-----------------------------------------------------------------------------
bool ParameterGroupBox::isValid(QStringList& errorList, const QString& parentIdentifier) const
{
    return model_.isValid(errorList, parentIdentifier);
}

void ParameterGroupBox::refresh() {
	view_.update();
}
