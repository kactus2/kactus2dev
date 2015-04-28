/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: fieldeditor.cpp
 *		Project: Kactus 2
 */

#include "fieldeditor.h"
#include "enumeratedvaluemodel.h"
#include "enumeratedvaluedelegate.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: fieldeditor::FieldEditor()
//-----------------------------------------------------------------------------
FieldEditor::FieldEditor(QSharedPointer<Field> field, QSharedPointer<Component> component, 
                         LibraryInterface* handler, QWidget* parent /*= 0*/ ):
QGroupBox(tr("Enumerated values"), parent),
enumView_(new EditableTableView(this)),
enumProxy_(new QSortFilterProxyModel(this)),
enumModel_(new EnumeratedValueModel(field, this))
{
	QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(enumView_, 0);

	enumProxy_->setSourceModel(enumModel_);
	enumView_->setModel(enumProxy_);

    const QString compPath = handler->getDirectoryPath(*component->getVlnv());
	QString defPath = QString("%1/enumListing.csv").arg(compPath);
	enumView_->setDefaultImportExportPath(defPath);
	enumView_->setAllowImportExport(true);

	// items can not be dragged
	enumView_->setItemsDraggable(false);
	enumView_->setSortingEnabled(true);
	enumView_->setItemDelegate(new EnumeratedValueDelegate(this));

	connect(enumModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(enumView_, SIGNAL(addItem(const QModelIndex&)),
		enumModel_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(enumView_, SIGNAL(removeItem(const QModelIndex&)),
        enumModel_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: fieldeditor::~FieldEditor()
//-----------------------------------------------------------------------------
FieldEditor::~FieldEditor()
{

}

//-----------------------------------------------------------------------------
// Function: fieldeditor::isValid()
//-----------------------------------------------------------------------------
bool FieldEditor::isValid() const
{
	return enumModel_->isValid();
}

//-----------------------------------------------------------------------------
// Function: fieldeditor::refresh()
//-----------------------------------------------------------------------------
void FieldEditor::refresh()
{
	enumView_->update();
}
