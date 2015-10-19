//-----------------------------------------------------------------------------
// File: cpusdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
QStyledItemDelegate(parent),
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
    else if (index.column() == CpuColumns::ADDRSPACE_COLUMN)
    {
        EnumCollectionEditor* editor = new EnumCollectionEditor(parent);
        return editor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
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
    else if (index.column() == CpuColumns::ADDRSPACE_COLUMN)
    {			
        EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
        Q_ASSERT(collectionEditor != 0);

        QStringList addrSpaces = index.data(CpuColumns::USER_DISPLAY_ROLE).toStringList();
        foreach (QString const& name, component_->getAddressSpaceNames())
        {
            collectionEditor->addItem(name, addrSpaces.contains(name));
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
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
    else if (index.column() == CpuColumns::ADDRSPACE_COLUMN)
    {
        EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
        Q_ASSERT(collectionEditor != 0);

        QStringList addrSpaces = collectionEditor->getSelectedItems();
        model->setData(index, addrSpaces, CpuColumns::USER_EDIT_ROLE);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void CpusDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
