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

#include <models/component.h>
#include <models/fileset.h>

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <QPainter>
#include <QApplication>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::FileDependencyDelegate()
//-----------------------------------------------------------------------------
FileDependencyDelegate::FileDependencyDelegate(QSharedPointer<Component> component, QObject* parent /*= 0*/)
    : QStyledItemDelegate(parent),
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
// Function: FileDependencyDelegate::paint()
//-----------------------------------------------------------------------------
void FileDependencyDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option,
                                   QModelIndex const& index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

//     if (index.column() == FILE_DEPENDENCY_COLUMN_STATUS)
//     {
//         // Draw the background as usual.
//         drawBackground(painter, option, index);
// 
//         // Draw the icon centered.
//         QIcon icon = qvariant_cast<QIcon>(index.model()->data(index, Qt::DecorationRole));
//         const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
// 
//         QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
//                                             QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
//                                             QRect(option.rect.x() + textMargin, option.rect.y(),
//                                             option.rect.width() - (2 * textMargin), option.rect.height()));
// 
//         drawDecoration(painter, viewItemOption, viewItemOption.rect, icon.pixmap(option.decorationSize));
// 
//         // Draw the focus.
//         drawFocus(painter, option, option.rect);
//     }
//     else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize FileDependencyDelegate::sizeHint(QStyleOptionViewItem const& option,
                                       QModelIndex const& index) const
{
    return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 4);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FileDependencyDelegate::createEditor(QWidget *parent, QStyleOptionViewItem const& option,
                                              QModelIndex const& index) const
{
    if (index.column() == FILE_DEPENDENCY_COLUMN_FILESETS)
    {
        EnumCollectionEditor* editor = new EnumCollectionEditor(parent);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FileDependencyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == FILE_DEPENDENCY_COLUMN_FILESETS)
    {
        EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
        Q_ASSERT(collectionEditor != 0);

        // Retrieve the file sets used by the item.
        QStringList fileSetNames = index.model()->data(index, Qt::UserRole).toStringList();

        // Update the contents of the editor accordingly.
        if (fileSetNames.contains("[multiple]"))
        {
            collectionEditor->addItem("[multiple]", true);
        }

        foreach (QSharedPointer<FileSet> fileSet, component_->getFileSets())
        {
            collectionEditor->addItem(fileSet->getName(), fileSetNames.contains(fileSet->getName()));
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
    if (index.column() == FILE_DEPENDENCY_COLUMN_FILESETS)
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
