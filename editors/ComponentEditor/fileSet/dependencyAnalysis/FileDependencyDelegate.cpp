//-----------------------------------------------------------------------------
// File: FileDependencyDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
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
EnumerationEditorConstructorDelegate(parent),
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
// Function: FileDependencyDelegate::isEnumerationEditorColumn()
//-----------------------------------------------------------------------------
bool FileDependencyDelegate::isEnumerationEditorColumn(QModelIndex const& index) const
{
    return index.column() == FileDependencyColumns::FILESETS;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::getCurrentSelection()
//-----------------------------------------------------------------------------
QStringList FileDependencyDelegate::getCurrentSelection(QModelIndex const& index) const
{
    return index.data(Qt::UserRole).toStringList();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::getAvailableItems()
//-----------------------------------------------------------------------------
QStringList FileDependencyDelegate::getAvailableItems() const
{
    QStringList fileSetNames;
    for (auto fileSet : *component_->getFileSets())
    {
        fileSetNames.append(fileSet->name());
    }

    return fileSetNames;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::setEnumerationDataToModel()
//-----------------------------------------------------------------------------
void FileDependencyDelegate::setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const
{
    model->setData(index, selectedItems, Qt::EditRole);
}
