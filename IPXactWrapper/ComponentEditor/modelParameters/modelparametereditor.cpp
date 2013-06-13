/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametereditor.cpp
 */

#include "modelparametereditor.h"

#include "usagedelegate.h"
#include <models/component.h>
#include <common/widgets/summaryLabel/summarylabel.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

ModelParameterEditor::ModelParameterEditor(QSharedPointer<Component> component,
	LibraryInterface* handler, 
										   QWidget *parent): 
ItemEditor(component, handler, parent), 
view_(this),
model_(component, this),
proxy_(this) {

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

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/modelParamList.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// set the delegate to edit the usage column
	view_.setItemDelegate(new UsageDelegate(2, this));

	// items can not be dragged
	view_.setItemsDraggable(false);

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);;

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Model parameters"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	refresh();
}

ModelParameterEditor::~ModelParameterEditor() {
}

bool ModelParameterEditor::isValid() const {
	return model_.isValid();
}

void ModelParameterEditor::refresh() {
	view_.update();
}

void ModelParameterEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/modelparams.html");
}

void ModelParameterEditor::setAllowImportExport( bool allow ) {
	view_.setAllowImportExport(allow);
}

void ModelParameterEditor::addModelParameter( QSharedPointer<ModelParameter> modelParam ) {
	model_.addModelParameter(modelParam);
}

void ModelParameterEditor::removeModelParameter( const QString& name ) {
	// find the index for the model parameter
	QModelIndex paramIndex = model_.index(name);
	
	// if the model parameter was found
	if (paramIndex.isValid()) {
		model_.onRemoveItem(paramIndex);
	}
}
