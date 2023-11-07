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

#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/ModeReference.h>

#include <editors/ComponentEditor/common/FloatingModeReferenceEditor.h>
#include <editors/ComponentEditor/common/ModeReferenceModel.h>

#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/modWriteComboBox/modwritecombobox.h>
#include <common/widgets/readActionComboBox/readactioncombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/testConstraintComboBox/testconstraintcombobox.h>

#include <KactusAPI/include/ModeReferenceInterface.h>

#include <QCompleter>
#include <QLineEdit>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate()
//-----------------------------------------------------------------------------
FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate(QAbstractItemModel* completionModel,
    QSharedPointer<ParameterFinder> parameterFinder, ModeReferenceInterface* modeRefInterface, QWidget* parent) :
ExpressionDelegate(completionModel, parameterFinder, parent),
modeRefInterface_(modeRefInterface)
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
        using ModeRefsList = QSharedPointer<QList<QSharedPointer<ModeReference> > >;

        // Update the mode reference interface with the mode references of the current access policy
        // by copying mode references over to interface to be able to abort editing in floating mode ref editor.
        auto modeReferencesVariant = index.data(Qt::UserRole);

        auto [currentModeRefs, otherModeRefsInUse] = 
            modeReferencesVariant.value<QPair<ModeRefsList, ModeRefsList> >();

        ModeRefsList copiedModeRefs(new QList<QSharedPointer<ModeReference> >());

        Utilities::copyList(copiedModeRefs, currentModeRefs);

        // Set the mode references of the currently selected field access policy to the mode reference interface.
        modeRefInterface_->setModeReferences(copiedModeRefs);

        // Set the othcer mode references in use for validation purposes.
        modeRefInterface_->setContainingElementModeReferences(otherModeRefsInUse);

        FloatingModeReferenceEditor* modeRefEditor = new FloatingModeReferenceEditor(modeRefInterface_, parent);
        connect(modeRefEditor, SIGNAL(finishEditing()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        connect(modeRefEditor, SIGNAL(cancelEditing()), this, SLOT(onEditingCanceled()), Qt::UniqueConnection);
        return modeRefEditor;
    }

    else if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        AccessComboBox* accessEditor = new AccessComboBox(parent);

        connect(accessEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return accessEditor;
    }

    else if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
    {
        ModWriteComboBox* modifiedWriteEditor = new ModWriteComboBox(parent);

        connect(modifiedWriteEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return modifiedWriteEditor;
    }

    else if (index.column() == FieldAccessPolicyColumns::READ_ACTION)
    {
        ReadActionComboBox* readActionEditor = new ReadActionComboBox(parent);

        connect(readActionEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return readActionEditor;
    }

    else if (index.column() == FieldAccessPolicyColumns::TESTABLE)
    {
        BooleanComboBox* testableEditor = new BooleanComboBox(parent);

        connect(testableEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return testableEditor;
    }

    else if (index.column() == FieldAccessPolicyColumns::TEST_CONSTRAINT)
    {
        TestConstraintComboBox* testConstraintEditor = new TestConstraintComboBox(parent);

        connect(testConstraintEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return testConstraintEditor;
    }

    else if (index.column() == FieldAccessPolicyColumns::WRITE_VALUE_CONSTRAINT)
    {
        QComboBox* writeConstraintEditor = new QComboBox(parent);

        writeConstraintEditor->addItems(WriteValueConversions::getConstraintTypes());

        connect(writeConstraintEditor, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return writeConstraintEditor;
    }

    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == FieldAccessPolicyColumns::MODE)
    {

    }

    else if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        auto castEditor = qobject_cast<AccessComboBox*>(editor);
        
        if (castEditor)
        {
            castEditor->setCurrentValue(AccessTypes::str2Access(
                index.data(Qt::DisplayRole).toString(), AccessTypes::ACCESS_COUNT));
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

    else if (index.column() == FieldAccessPolicyColumns::READ_ACTION)
    {
        auto readActionEditor = qobject_cast<ReadActionComboBox*>(editor);

        if (readActionEditor)
        {
            readActionEditor->setCurrentValue(General::str2ReadAction(index.data(Qt::DisplayRole).toString()));
        }
    }
    
    else if (index.column() == FieldAccessPolicyColumns::TESTABLE)
    {
        auto testableEditor = qobject_cast<BooleanComboBox*>(editor);

        if (testableEditor)
        {
            testableEditor->setCurrentValue(index.data(Qt::DisplayRole).toString());
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::TEST_CONSTRAINT)
    {
        auto testConstraintEditor = qobject_cast<TestConstraintComboBox*>(editor);

        if (testConstraintEditor)
        {
            testConstraintEditor->setCurrentValue(General::str2TestConstraint(index.data(Qt::DisplayRole).toString()));
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::WRITE_VALUE_CONSTRAINT)
    {
        auto writeConstraintEditor = qobject_cast<QComboBox*>(editor);

        if (writeConstraintEditor)
        {
            writeConstraintEditor->setCurrentIndex(
                WriteValueConversions::stringToType(index.data(Qt::DisplayRole).toString()));
        }
    }

    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::setModelData()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == FieldAccessPolicyColumns::MODE)
    {
        if (auto modeEditor = qobject_cast<FloatingModeReferenceEditor*>(editor))
        {
            // Get the modified mode references from interface and send to access policy.
            auto updatedModeRefs = modeRefInterface_->getModeReferences();

            QVariant modeRefsVariant = QVariant::fromValue(updatedModeRefs);
            model->setData(index, modeRefsVariant);
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

    else if (index.column() == FieldAccessPolicyColumns::READ_ACTION)
    {
        auto readActionEditor = qobject_cast<ReadActionComboBox*>(editor);

        if (readActionEditor)
        {
            model->setData(index, readActionEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::TESTABLE)
    {
        auto testableEditor = qobject_cast<BooleanComboBox*>(editor);

        if (testableEditor)
        {
            model->setData(index, testableEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::TEST_CONSTRAINT)
    {
        auto testConstraintEditor = qobject_cast<TestConstraintComboBox*>(editor);

        if (testConstraintEditor)
        {
            model->setData(index, testConstraintEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::WRITE_VALUE_CONSTRAINT)
    {
        auto writeConstraintEditor = qobject_cast<QComboBox*>(editor);

        if (writeConstraintEditor)
        {
            model->setData(index, writeConstraintEditor->currentText(), Qt::EditRole);
        }
    }

    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.column() == FieldAccessPolicyColumns::MODE)
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

        modeRefEditor->setFixedWidth(editorWidth);

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
        ExpressionDelegate::updateEditorGeometry(editor, option, index);
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
// Function: FieldAccessPoliciesDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool FieldAccessPoliciesDelegate::columnAcceptsExpression(int column) const
{
    return column == FieldAccessPolicyColumns::READ_RESPONSE ||
        column == FieldAccessPolicyColumns::RESERVED ||
        column == FieldAccessPolicyColumns::WRITE_CONSTRAINT_MAXIMUM ||
        column == FieldAccessPolicyColumns::WRITE_CONSTRAINT_MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int FieldAccessPoliciesDelegate::descriptionColumn() const
{
    return FieldAccessPolicyColumns::COUNT;
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
