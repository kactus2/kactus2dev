/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: registereditor.cpp
 *		Project: Kactus 2
 */

#include "registereditor.h"
#include <common/views/EditableTableView/editabletableview.h>
#include "registertablemodel.h"
#include "registerdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

RegisterEditor::RegisterEditor(QSharedPointer<Register> reg, 
							   QSharedPointer<Component> component,
							   LibraryInterface* handler, 
							   QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
view_(new EditableTableView(this)),
proxy_(new QSortFilterProxyModel(this)),
model_(new RegisterTableModel(reg, this)) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Fields summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	//! \brief Enable import/export csv file
	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/fieldListing.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	// items can not be dragged
	view_->setItemsDraggable(false);

	view_->setSortingEnabled(true);

	view_->setItemDelegate(new RegisterDelegate(this));

	view_->sortByColumn(RegisterDelegate::OFFSET_COLUMN, Qt::AscendingOrder);

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(fieldAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(fieldRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

RegisterEditor::~RegisterEditor() {
}

bool RegisterEditor::isValid() const {
	return model_->isValid();
}

void RegisterEditor::refresh() {
	view_->update();
}

void RegisterEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/register.html");
}

QSize RegisterEditor::sizeHint() const {
	return QSize(RegisterEditor::WIDTH, RegisterEditor::HEIGHT);
}
