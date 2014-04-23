/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: fieldeditor.cpp
 *		Project: Kactus 2
 */

#include "fieldeditor.h"
#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/summaryLabel/summarylabel.h>
#include "enumeratedvaluemodel.h"
#include "enumeratedvaluedelegate.h"
#include "writevalueconstrainteditor.h"
#include <library/LibraryManager/libraryinterface.h>

FieldEditor::FieldEditor(QSharedPointer<Field> field, 
						 QSharedPointer<Component> component, 
						 LibraryInterface* handler, 
						 QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
enumView_(new EditableTableView(this)),
enumProxy_(new QSortFilterProxyModel(this)),
enumModel_(new EnumeratedValueModel(field, this)),
writeConstr_(new WriteValueConstraintEditor(field->getWriteConstraint(), tr("Write value constraints"), this)) {

	// display the labels on top the table
	SummaryLabel* enumLabel = new SummaryLabel(tr("Enumerated values"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(enumLabel, 0, Qt::AlignCenter);
	layout->addWidget(enumView_, 1);
	layout->addWidget(writeConstr_, 0);
	layout->setContentsMargins(0, 0, 0, 0);

	enumProxy_->setSourceModel(enumModel_);
	enumView_->setModel(enumProxy_);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/enumListing.csv").arg(compPath);
	enumView_->setDefaultImportExportPath(defPath);
	enumView_->setAllowImportExport(true);

	// items can not be dragged
	enumView_->setItemsDraggable(false);
	enumView_->setSortingEnabled(true);
	enumView_->setItemDelegate(new EnumeratedValueDelegate(this));

	connect(enumModel_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(enumView_, SIGNAL(addItem(const QModelIndex&)),
		enumModel_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(enumView_, SIGNAL(removeItem(const QModelIndex&)),
		enumModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(writeConstr_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

FieldEditor::~FieldEditor() {
}

bool FieldEditor::isValid() const {
	return enumModel_->isValid();
}

void FieldEditor::refresh() {
	enumView_->update();
}

void FieldEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/field.html");
}

QSize FieldEditor::sizeHint() const {
	return QSize(FieldEditor::WIDTH, FieldEditor::HEIGHT);
}
