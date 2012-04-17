/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarysearchwidget.h"
#include "librarydata.h"
#include "libraryhandler.h"

#include <QObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>

LibrarySearchWidget::LibrarySearchWidget(LibraryHandler *parent,
		LibraryData* dataModel): 
QWidget(parent),
searchLabel_(tr("Enter search word"), this), 
searchWordEdit_(this),
typeBox_(this),	
vlnvBox_(this),
hideShowCheckBox_(tr("Show hidden items"), this),
filter_(this),
view_(parent, &filter_, this) {

	// set LibraryData as the source model for the filter
	filter_.setSourceModel(dataModel);

	// set filter to be used as  source model for the table view
	view_.setModel(&filter_);

	// set up the layout for the widget
	setupLayout();

	// connect the signals between the GUI items in the widget
	setupConnections();

	// connect the custom view signals to the data model
	connectDataModel(dataModel);
}

LibrarySearchWidget::~LibrarySearchWidget() {
}

void LibrarySearchWidget::setupLayout() {

	// create the top layout for the widget
	QVBoxLayout* topLayout = new QVBoxLayout(this);

	// add the label and the line edit to set search word
	topLayout->addWidget(&searchLabel_, 0);
	topLayout->addWidget(&searchWordEdit_, 0);

	// create horizontal layout for the two group boxes to be side by side
	QHBoxLayout* groupLayout = new QHBoxLayout();

	// add the group boxes
	groupLayout->addWidget(&typeBox_, 0);
	groupLayout->addWidget(&vlnvBox_, 0);

	// add the horizontal layout to the top layout
	topLayout->addLayout(groupLayout);

	// create layout to set the addItemButton and checkBox to be side by side
	QHBoxLayout* buttonCheckLayout = new QHBoxLayout();

	// add the check box
	buttonCheckLayout->addWidget(&hideShowCheckBox_, 0);

	// add the layout to top layout
	topLayout->addLayout(buttonCheckLayout);

	// finally add the table view and set the it to be the only stretching item
	// in this widget
	topLayout->addWidget(&view_, 1);
}

void LibrarySearchWidget::setupConnections() {

	// connect the checkBox to filter
	connect(&hideShowCheckBox_, SIGNAL(stateChanged(int)),
			&filter_, SLOT(onShowStatusChange(int)), Qt::UniqueConnection);

	// connect the search word line edit to filter
	connect(&searchWordEdit_, SIGNAL(textChanged(const QString&)),
			&filter_, SLOT(onSearchWordChanged(const QString&)),
			Qt::UniqueConnection);

	// connect the document type checkbox group to filter
	connect(&typeBox_, SIGNAL(stateChanged(
			const TypeBox::TypeSearchSettings*)),
			&filter_, SLOT(onTypeStateChanged(
					const TypeBox::TypeSearchSettings*)),
			Qt::UniqueConnection);

	// connect the vlnv element checkBox to filter
	connect(&vlnvBox_, SIGNAL(stateChanged(
			const VLNVBox::VLNVSearchSettings*)),
			&filter_, SLOT(onVLNVStateChanged(
					const VLNVBox::VLNVSearchSettings*)),
			Qt::UniqueConnection);

}

void LibrarySearchWidget::connectDataModel(LibraryData* dataModel) {

	connect(&view_, SIGNAL(openComponent(const QModelIndex&)),
		dataModel, SLOT(onOpenComponent(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(deleteItem(const QModelIndex&)),
			dataModel, SLOT(onDeleteItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(exportItem(const QModelIndex&)),
			dataModel, SLOT(onExportItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(hideItem(const QModelIndex&)),
			dataModel, SLOT(onHideItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(showItem(const QModelIndex&)),
			dataModel, SLOT(onShowItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(refreshLibrary()),
			dataModel, SLOT(checkIntegrity()), Qt::UniqueConnection);

	connect(&view_, SIGNAL(openDesign(const QModelIndex&)),
		dataModel, SLOT(onOpenDesign(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createNewComponent(const QModelIndex&)),
		dataModel, SLOT(onCreateNewComponent(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createNewDesign(const QModelIndex&)),
		dataModel, SLOT(onCreateNewDesign(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(openBusDef(const QModelIndex&)),
		dataModel, SLOT(onOpenBusDef(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createBusDef(const QModelIndex&)),
		dataModel, SLOT(onCreateBusDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openComDef(const QModelIndex&)),
        dataModel, SLOT(onOpenComDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createComDef(const QModelIndex&)),
        dataModel, SLOT(onCreateComDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openApiDef(const QModelIndex&)),
        dataModel, SLOT(onOpenApiDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createApiDef(const QModelIndex&)),
        dataModel, SLOT(onCreateApiDef(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(errorMessage(const QString&)),
		dataModel, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(noticeMessage(const QString&)),
		dataModel, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}
