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

FieldEditor::FieldEditor(QSharedPointer<Field> field, 
						 QSharedPointer<Component> component, 
						 QWidget* parent /*= 0*/ ):
ItemEditor(component, parent),
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
	// items can not be dragged
	enumView_->setItemsDraggable(false);
	enumView_->setSortingEnabled(true);
	enumView_->setItemDelegate(new EnumeratedValueDelegate(this));
	enumView_->setColumnWidth(EnumeratedValueDelegate::NAME_COLUMN, 200);
	enumView_->setColumnWidth(EnumeratedValueDelegate::VALUE_COLUMN, 200);

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
