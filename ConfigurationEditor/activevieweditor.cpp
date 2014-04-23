/* 
 *  	Created on: 19.8.2011
 *      Author: Antti Kamppi
 * 		filename: activevieweditor.cpp
 *		Project: Kactus 2
 */

#include "activevieweditor.h"

#include "activeviewdelegate.h"

#include <designEditors/HWDesign/HWDesignWidget.h>

#include <models/designconfiguration.h>

#include <QVBoxLayout>
#include <QSharedPointer>
#include <QHeaderView>

ActiveViewEditor::ActiveViewEditor(QWidget *parent):
QGroupBox(parent),
view_(this),
filter_(this),
model_(this) {

	// set settings for the view
	view_.horizontalHeader()->setStretchLastSection(true);
	view_.horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setSelectionBehavior(QAbstractItemView::SelectItems);
	view_.setEditTriggers(QAbstractItemView::DoubleClicked | 
		QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
	view_.setWordWrap(true);
	view_.setSortingEnabled(true);
	view_.setItemDelegate(new ActiveViewDelegate(this));
	view_.verticalHeader()->hide();

	// set up connections between view and models
	filter_.setSourceModel(&model_);
	view_.setModel(&filter_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_, 1);

	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

ActiveViewEditor::~ActiveViewEditor() {
}

void ActiveViewEditor::clear() {
	model_.clear();
	view_.setDisabled(true);
}

void ActiveViewEditor::setDesign( DesignWidget* designWidget, bool locked) {

	if (designWidget) {
		QSharedPointer<DesignConfiguration> desConf = designWidget->getDiagram()->getDesignConfiguration();

		// if configuration is not used then view is always disabled
		if (!desConf)
			view_.setDisabled(true);

		// if configuration is used
		else 
			view_.setDisabled(locked);

		model_.setDesign(designWidget, desConf);
        view_.resizeRowsToContents();
	}
	else {
		clear();
	}
}

void ActiveViewEditor::setLocked( bool locked ) {
	// if configuration exists
	if (model_.hasConfiguration())
		view_.setDisabled(locked);

	// if no configuration exists then always disable the view
	else
		view_.setDisabled(true);
}
