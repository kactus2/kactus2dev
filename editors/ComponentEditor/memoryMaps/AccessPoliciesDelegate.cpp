//-----------------------------------------------------------------------------
// File: AccessPoliciesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.8.2023
//
// Description:
// Provides editors for access policies.
//-----------------------------------------------------------------------------

#include "AccessPoliciesDelegate.h"
#include "FieldAccessPolicyColumns.h"

#include <common/widgets/accessComboBox/accesscombobox.h>

#include <editors/ComponentEditor/common/ModeReferenceModel.h>
#include <editors/ComponentEditor/common/FloatingModeReferenceEditor.h>

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::AccessPoliciesDelegate()
//-----------------------------------------------------------------------------
AccessPoliciesDelegate::AccessPoliciesDelegate(QWidget* parent):
    QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AccessPoliciesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        auto modeRefs = index.data(Qt::UserRole).value<QList<QPair<QString, int> > >();

        ModeReferenceModel* model = new ModeReferenceModel(modeRefs, parent);
        FloatingModeReferenceEditor* modeRefEditor = new FloatingModeReferenceEditor(model, parent);

        connect(modeRefEditor, SIGNAL(finishEditing()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        connect(modeRefEditor, SIGNAL(cancelEditing()), this, SLOT(onEditingCanceled()), Qt::UniqueConnection);
        return modeRefEditor;
    }
    else if (index.column() == 1)
    {
        AccessComboBox* accessEditor = new AccessComboBox(parent);

        connect(accessEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return accessEditor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AccessPoliciesDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        auto modeEditor = qobject_cast<FloatingModeReferenceEditor*>(editor);
        if (modeEditor)
        {
            auto modeRefs = index.data(Qt::UserRole).value<QList<QPair<QString, int> > >();
            modeEditor->setModeRefs(modeRefs);
        }
    }
    else if (index.column() == 1)
    {
        auto castEditor = qobject_cast<AccessComboBox*>(editor);

        if (castEditor)
        {
            castEditor->setCurrentValue(AccessTypes::str2Access(
                index.data(Qt::DisplayRole).toString(), AccessTypes::ACCESS_COUNT));
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::setModelData()
//-----------------------------------------------------------------------------
void AccessPoliciesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        auto modeEditor = qobject_cast<FloatingModeReferenceEditor*>(editor);
        if (modeEditor)
        {
            auto modeRefs = modeEditor->getModeRefs();

            QVariant modeRefsVariant;
            modeRefsVariant.setValue(modeRefs);

            model->setData(index, modeRefsVariant, Qt::EditRole);
        }
    }

    else if (index.column() == 1)
    {
        auto accessEditor = qobject_cast<AccessComboBox*>(editor);

        if (accessEditor)
        {
            model->setData(index, accessEditor->currentText(), Qt::EditRole);
        }
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void AccessPoliciesDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        auto modeRefEditor = qobject_cast<FloatingModeReferenceEditor*>(editor);
        int editorMinimumHeight = modeRefEditor->sizeHint().height();

        auto modeRefTableMargins = modeRefEditor->getTableMargins();

        int editorWidth = modeRefEditor->getTableSizeHint().width() + modeRefTableMargins.left()
            + modeRefTableMargins.right() + 2;

        if (editorWidth < 250)
        {
            editorWidth = 250;
        }
        else if (editorWidth > 300)
        {
            editorWidth = 300;
        }

        if (editorMinimumHeight < 250)
        {
            editorMinimumHeight = 250;
        }

        editor->setFixedWidth(editorWidth);

        const int PARENT_HEIGHT = editor->parentWidget()->height();
        const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

        if (AVAILABLE_HEIGHT_BELOW > editorMinimumHeight)
        {
            editor->move(option.rect.topLeft());
        }
        else
        {
            int editorNewY = PARENT_HEIGHT - editorMinimumHeight;
            if (editorNewY <= 0)
            {
                editorNewY = 0;
            }

            editor->move(option.rect.left(), editorNewY);
        }

        if (editorMinimumHeight > PARENT_HEIGHT)
        {
            editor->setFixedHeight(PARENT_HEIGHT);
        }
        else
        {
            editor->setFixedHeight(editorMinimumHeight);
        }
    }
    else
    {
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void AccessPoliciesDelegate::commitAndCloseEditor()
{
    auto editor = qobject_cast<QWidget*>(sender());

    if (editor)
    {
        emit commitData(editor);
        emit closeEditor(editor);
    }
}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::onEditingCanceled()
//-----------------------------------------------------------------------------
void AccessPoliciesDelegate::onEditingCanceled()
{
    auto editor = qobject_cast<QWidget*>(sender());
    if (editor)
    {
        emit closeEditor(editor);
    }
}
