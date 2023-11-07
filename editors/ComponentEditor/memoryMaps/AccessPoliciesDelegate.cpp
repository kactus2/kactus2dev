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

#include <editors/ComponentEditor/common/FloatingModeReferenceEditor.h>

#include <KactusAPI/include/ModeReferenceInterface.h>

#include <IPXACTmodels/Component/ModeReference.h>

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::AccessPoliciesDelegate()
//-----------------------------------------------------------------------------
AccessPoliciesDelegate::AccessPoliciesDelegate(ModeReferenceInterface* modeReferenceInterface, QWidget* parent) :
    QStyledItemDelegate(parent),
    modeRefInterface_(modeReferenceInterface)
{

}

//-----------------------------------------------------------------------------
// Function: AccessPoliciesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AccessPoliciesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        using ModeRefsList = QSharedPointer<QList<QSharedPointer<ModeReference> > >;

        // Update the mode reference interface with the mode references of the currently selected access policy
        // by copying mode references over to interface to be able to abort editing in floating mode ref editor.
        auto modeReferencesVariant = index.data(Qt::UserRole);

        auto [currentModeRefs, otherModeRefsInUse] =
            modeReferencesVariant.value<QPair<ModeRefsList, ModeRefsList> >();

        QSharedPointer<QList<QSharedPointer<ModeReference> > > copiedModeRefs(
            new QList<QSharedPointer<ModeReference> >());

        Utilities::copyList(copiedModeRefs, currentModeRefs);

        // Set the mode references of the currently selected access policy to the mode reference interface.
        modeRefInterface_->setModeReferences(copiedModeRefs);

        // Set the othcer mode references in use for validation purposes.
        modeRefInterface_->setContainingElementModeReferences(otherModeRefsInUse);

        FloatingModeReferenceEditor* modeRefEditor = new FloatingModeReferenceEditor(modeRefInterface_, parent);

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
    if (index.column() == 1)
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
        // Get the modified mode references from interface and send to access policy.
        auto updatedModeRefs = modeRefInterface_->getModeReferences();

        QVariant modeRefsVariant = QVariant::fromValue(updatedModeRefs);
        model->setData(index, modeRefsVariant);
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
