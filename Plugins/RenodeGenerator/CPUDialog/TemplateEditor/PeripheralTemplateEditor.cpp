//-----------------------------------------------------------------------------
// File: PeripheralTemplateEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.05.2023
//
// Description:
// Editor for peripheral templates in renode generator.
//-----------------------------------------------------------------------------

#include "PeripheralTemplateEditor.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <Plugins/RenodeGenerator/CPUDialog/TemplateEditor/PeripheralTemplateModel.h>
#include <Plugins/RenodeGenerator/CPUDialog/TemplateEditor/PeripheralTemplateDelegate.h>

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateEditor::PeripheralTemplateEditor()
//-----------------------------------------------------------------------------
PeripheralTemplateEditor::PeripheralTemplateEditor(QString const& generationFolder, QWidget *parent /* = 0 */):
QWidget(parent),
view_(new EditableTableView(this)),
model_(new PeripheralTemplateModel(this)),
templateDelegate_(new PeripheralTemplateDelegate(generationFolder, this, this))
{
    auto proxy(new QSortFilterProxyModel(this));
    proxy->setSourceModel(model_);

    view_->setModel(proxy);
    view_->setItemDelegate(templateDelegate_);

    auto layout(new QVBoxLayout(this));
    layout->addWidget(view_, 1);
    layout->setContentsMargins(0, 0, 0, 0);

    connectSignals();
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateEditor::connectSignals()
//-----------------------------------------------------------------------------
void PeripheralTemplateEditor::connectSignals() const
{
	connect(view_, SIGNAL(addItem(const QModelIndex&)), model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)), model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateEditor::setupTemplates()
//-----------------------------------------------------------------------------
void PeripheralTemplateEditor::setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplars)
{
    model_->setupTemplates(newTemplars);
}

//-----------------------------------------------------------------------------
// Function: PeripheralTemplateEditor::getTemplates()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > PeripheralTemplateEditor::getTemplates() const
{
    return model_->getTemplates();
}
