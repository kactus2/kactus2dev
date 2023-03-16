//-----------------------------------------------------------------------------
// File: cpusdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The delegate to provide editors to add/remove/edit the cpus of a component.
//-----------------------------------------------------------------------------

#include "cpusdelegate.h"

#include "CpuColumns.h"

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <QLineEdit>
#include <QStringList>
#include <QString>

//-----------------------------------------------------------------------------
// Function: CpusDelegate::CpusDelegate()
//-----------------------------------------------------------------------------
CpusDelegate::CpusDelegate(QSharedPointer<Component> component, QObject* parent):
EnumerationEditorConstructorDelegate(parent),
component_(component)
{
    Q_ASSERT(component);
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::~CpusDelegate()
//-----------------------------------------------------------------------------
CpusDelegate::~CpusDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* CpusDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == CpuColumns::NAME_COLUMN ||
        index.column() == CpuColumns::DISPLAY_NAME_COLUMN ||
        index.column() ==  CpuColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return lineEdit;
    }
    else
    {
        return EnumerationEditorConstructorDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::setEditorData()
//-----------------------------------------------------------------------------
void CpusDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == CpuColumns::NAME_COLUMN ||
        index.column() == CpuColumns::DISPLAY_NAME_COLUMN ||
        index.column() ==  CpuColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else
    {
        EnumerationEditorConstructorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::setModelData()
//-----------------------------------------------------------------------------
void CpusDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == CpuColumns::NAME_COLUMN ||
        index.column() == CpuColumns::DISPLAY_NAME_COLUMN ||
        index.column() ==  CpuColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        EnumerationEditorConstructorDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::isEnumerationEditorColumn()
//-----------------------------------------------------------------------------
bool CpusDelegate::isEnumerationEditorColumn(QModelIndex const& index) const
{
    return index.column() == CpuColumns::ADDRSPACE_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::getCurrentSelection()
//-----------------------------------------------------------------------------
QStringList CpusDelegate::getCurrentSelection(QModelIndex const& index) const
{
    return index.data(CpuColumns::USER_DISPLAY_ROLE).toStringList();
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::getAvailableItems()
//-----------------------------------------------------------------------------
QStringList CpusDelegate::getAvailableItems() const
{
    return component_->getAddressSpaceNames();;
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::setEnumerationDataToModel()
//-----------------------------------------------------------------------------
void CpusDelegate::setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const
{
    model->setData(index, selectedItems, CpuColumns::USER_EDIT_ROLE);
}
