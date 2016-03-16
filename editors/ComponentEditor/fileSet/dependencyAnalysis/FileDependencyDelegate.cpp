//-----------------------------------------------------------------------------
// File: FileDependencyDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.02.2013
//
// Description:
// Delegate used in visualizing the file dependency model.
//-----------------------------------------------------------------------------

#include "FileDependencyDelegate.h"

#include "FileDependencyModel.h"
#include "FileDependencyItem.h"
#include "FileDependencyColumns.h"

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QPainter>
#include <QApplication>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::FileDependencyDelegate()
//-----------------------------------------------------------------------------
FileDependencyDelegate::FileDependencyDelegate(QSharedPointer<Component> component, QObject* parent):
QStyledItemDelegate(parent),
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::~FileDependencyDelegate()
//-----------------------------------------------------------------------------
FileDependencyDelegate::~FileDependencyDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize FileDependencyDelegate::sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 4);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FileDependencyDelegate::createEditor(QWidget *parent, QStyleOptionViewItem const& option,
                                              QModelIndex const& index) const
{
    if (index.column() == FileDependencyColumns::FILESETS)
    {
        EnumCollectionEditor* editor = new EnumCollectionEditor(parent);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FileDependencyDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == FileDependencyColumns::FILESETS)
    {
        EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
        Q_ASSERT(collectionEditor != 0);

        // Retrieve the file sets used by the item.
        QStringList fileSetNames = index.data(Qt::UserRole).toStringList();

        // Update the contents of the editor accordingly.
        if (fileSetNames.contains("[multiple]"))
        {
            collectionEditor->addItem("[multiple]", true);
        }

        foreach (QSharedPointer<FileSet> fileSet, *component_->getFileSets())
        {
            collectionEditor->addItem(fileSet->name(), fileSetNames.contains(fileSet->name()));
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }

}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::setModelData()
//-----------------------------------------------------------------------------
void FileDependencyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                          QModelIndex const& index) const
{
    if (index.column() == FileDependencyColumns::FILESETS)
    {
        EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
        Q_ASSERT(collectionEditor != 0);

        QStringList fileSetNames = collectionEditor->getSelectedItems();
        model->setData(index, fileSetNames, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
