//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Delegate for field access policies.
//-----------------------------------------------------------------------------

#include "FieldAccessPoliciesDelegate.h"
#include "FieldAccessPolicyColumns.h"

#include <editors/ComponentEditor/common/ModeReferenceEditor.h>

#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/modWriteComboBox/modwritecombobox.h>

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate()
//-----------------------------------------------------------------------------
FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate(QWidget* parent):
    QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FieldAccessPoliciesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.column() == FieldAccessPolicyColumns::MODE)
    {
        auto modeRefs = index.data(Qt::UserRole).value<QList<QPair<QString, int> > >();

        ModeReferenceEditor* modeRefEditor = new ModeReferenceEditor(modeRefs, parent);
        connect(modeRefEditor, SIGNAL(finishEditing()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        connect(modeRefEditor, SIGNAL(cancelEditing()), this, SLOT(onEditingCanceled()), Qt::UniqueConnection);
        return modeRefEditor;
    }

    if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        AccessComboBox* accessEditor = new AccessComboBox(parent);

        connect(accessEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return accessEditor;
    }

    if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
    {
        ModWriteComboBox* modifiedWriteEditor = new ModWriteComboBox(parent);

        connect(modifiedWriteEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return modifiedWriteEditor;
    }

    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == FieldAccessPolicyColumns::MODE)
    {
        auto modeEditor = qobject_cast<ModeReferenceEditor*>(editor);
        if (modeEditor)
        {
            auto modeRefs = index.data(Qt::UserRole).value<QList<QPair<QString, int> > >();
            modeEditor->setModeRefs(modeRefs);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        auto castEditor = qobject_cast<AccessComboBox*>(editor);
        
        if (castEditor)
        {
            castEditor->setCurrentValue(AccessTypes::str2Access(index.data(Qt::DisplayRole).toString(), AccessTypes::ACCESS_COUNT));
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
    {
        auto modfiedWriteEditor = qobject_cast<ModWriteComboBox*>(editor);

        if (modfiedWriteEditor)
        {
            modfiedWriteEditor->setCurrentValue(General::str2ModifiedWrite(index.data(Qt::DisplayRole).toString()));
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::setModelData()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{

    if (index.column() == FieldAccessPolicyColumns::MODE)
    {
        auto modeEditor = qobject_cast<ModeReferenceEditor*>(editor);
        if (modeEditor)
        {
            auto modeRefs = modeEditor->getModeRefs();

            QVariant modeRefsVariant;
            modeRefsVariant.setValue(modeRefs);

            model->setData(index, modeRefsVariant, Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        auto accessEditor = qobject_cast<AccessComboBox*>(editor);

        if (accessEditor)
        {
            model->setData(index, accessEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
    {
        auto modifiedWriteEditor = qobject_cast<ModWriteComboBox*>(editor);

        if (modifiedWriteEditor)
        {
            model->setData(index, modifiedWriteEditor->currentText(), Qt::EditRole);
        }
    }

    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == FieldAccessPolicyColumns::MODE)
    {
        const int editorMinimumHeight = editor->sizeHint().height();

        int editorWidth = editor->sizeHint().width();
        if (editorWidth < 150)
        {
            editorWidth = 150;
        }
        else if (editorWidth > 250)
        {
            editorWidth = 250;
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
// Function: FieldAccessPoliciesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::commitAndCloseEditor()
{
    auto editor = qobject_cast<QWidget*>(sender());

    if (editor)
    {
        emit commitData(editor);
        emit closeEditor(editor);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::onEditingCanceled()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::onEditingCanceled()
{
    auto editor = qobject_cast<QWidget*>(sender());
    if (editor)
    {
        emit closeEditor(editor);
    }
}
