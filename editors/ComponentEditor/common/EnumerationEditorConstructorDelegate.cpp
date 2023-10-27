//-----------------------------------------------------------------------------
// File: EnumerationEditorConstructorDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.01.2023
//
// Description:
// The delegate that provides enumeration editors.
//-----------------------------------------------------------------------------

#include "EnumerationEditorConstructorDelegate.h"

#include <common/widgets/EnumCollectionEditor/EnumerationEditor.h>

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::EnumerationEditorConstructorDelegate()
//-----------------------------------------------------------------------------
EnumerationEditorConstructorDelegate::EnumerationEditorConstructorDelegate(QObject* parent):
QStyledItemDelegate(parent),
hideCheckAll_(false)
{

}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::setHideCheckAll()
//-----------------------------------------------------------------------------
void EnumerationEditorConstructorDelegate::setHideCheckAll(bool newValue)
{
    hideCheckAll_ = newValue;
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* EnumerationEditorConstructorDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    if (isEnumerationEditorColumn(index))
    {
        EnumerationEditor* enumerationEditor = new EnumerationEditor(hideCheckAll_, parent);

        connect(enumerationEditor, SIGNAL(cancelEditing()), this, SLOT(onEditorCancel()), Qt::UniqueConnection);
        connect(enumerationEditor, SIGNAL(finishEditing()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);

        return enumerationEditor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::setEditorData()
//-----------------------------------------------------------------------------
void EnumerationEditorConstructorDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.isValid() && isEnumerationEditorColumn(index))
    {
        EnumerationEditor* enumerationEditor = dynamic_cast<EnumerationEditor*>(editor);
        if (enumerationEditor)
        {
            QStringList sortedAvailableItems = getAvailableItems();
            sortedAvailableItems.sort(Qt::CaseInsensitive);

            QStringList exclusiveItems = getExclusiveItems();

            enumerationEditor->setupItems(sortedAvailableItems, exclusiveItems, getCurrentSelection(index));
        }
    }

    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::getExclusionItems()
//-----------------------------------------------------------------------------
QStringList EnumerationEditorConstructorDelegate::getExclusiveItems() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::setModelData()
//-----------------------------------------------------------------------------
void EnumerationEditorConstructorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (isEnumerationEditorColumn(index)) 
    {
        EnumerationEditor* enumerationEditor = dynamic_cast<EnumerationEditor*>(editor);
        if (enumerationEditor)
        {
            QStringList selectedItems = enumerationEditor->getSelectedItems();
            setEnumerationDataToModel(index, model, selectedItems);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void EnumerationEditorConstructorDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::onEditorCancel()
//-----------------------------------------------------------------------------
void EnumerationEditorConstructorDelegate::onEditorCancel()
{
    QWidget* edit = qobject_cast<QWidget*>(sender());
    if (edit)
    {
        emit closeEditor(edit);
    }
}

//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void EnumerationEditorConstructorDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    if (isEnumerationEditorColumn(index))
    {
        repositionAndResizeEditor(editor, option, index);
    }
}


//-----------------------------------------------------------------------------
// Function: EnumerationEditorConstructorDelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void EnumerationEditorConstructorDelegate::repositionAndResizeEditor(QWidget* editor,
    QStyleOptionViewItem const& option, QModelIndex const& /*index*/) const
{
    int enumerationCount = getAvailableItems().count();
    int editorMinimumSize = 25 * (enumerationCount + 2);

    int editorWidth =  option.rect.right() - option.rect.left();
    if (editorWidth < 150)
    {
        editorWidth = 150;
    }
    else if (editorWidth > 350)
    {
        editorWidth = 350;
    }

    editor->setFixedWidth(editorWidth);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT - editorMinimumSize;
        if (editorNewY <= 0)
        {
            editorNewY = 0;
        }

        editor->move(option.rect.left(), editorNewY);
    }

    if (editorMinimumSize > PARENT_HEIGHT)
    {
        editor->setFixedHeight(PARENT_HEIGHT);
    }
    else
    {
        editor->setFixedHeight(editorMinimumSize);
    }
}
