/* 
 *  	Created on: 12.6.2012
 *      Author: Antti Kamppi
 * 		filename: viewseditor.cpp
 *		Project: Kactus 2
 */

#include "viewseditor.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

ViewsEditor::ViewsEditor( QSharedPointer<Component> component,
	LibraryInterface* handler, 
						 QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Views summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);	
    proxy_.setDynamicSortFilter(false);

	view_.setModel(&proxy_);    
    view_.sortByColumn(ViewsModel::NAME_COLUMN, Qt::AscendingOrder);
    view_.sortByColumn(ViewsModel::TYPE_COLUMN, Qt::AscendingOrder);
    //view_.sortByColumn(ViewsModel::TYPE_COLUMN, Qt::AscendingOrder);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/viewListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new LineEditDelegate(this));

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(contentChanged()),
        this, SLOT(onAddItem()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(viewRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

ViewsEditor::~ViewsEditor() {
}

bool ViewsEditor::isValid() const {
	return model_.isValid();
}

void ViewsEditor::refresh() {    
    onAddItem();
    view_.update();
}

void ViewsEditor::onAddItem()
{
    view_.sortByColumn(ViewsModel::NAME_COLUMN, Qt::AscendingOrder);
    view_.sortByColumn(ViewsModel::TYPE_COLUMN, Qt::AscendingOrder);
    view_.update();
}

void ViewsEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/views.html");
}
