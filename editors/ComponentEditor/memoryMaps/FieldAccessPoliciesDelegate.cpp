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

using ModeRefsList = std::vector<std::pair<unsigned int, std::string> >;

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
        // Update the mode reference interface with the mode references of the current access policy
        // by copying mode references over to interface to be able to abort editing in floating mode ref editor.
        auto modeReferencesVariant = index.data(Qt::UserRole);

        auto [currentModeRefs, otherModeRefsInUse] = modeReferencesVariant.value<QPair<ModeRefsList, ModeRefsList> >();

        // Set the mode references of the currently selected field access policy to the mode reference interface.
        modeRefInterface_->setModeReferences(currentModeRefs);

        // Set the othcer mode references in use for validation purposes.
        modeRefInterface_->setContainingElementModeReferences(otherModeRefsInUse);

        modeRefInterface_->setContainingElementIsRemap(false);

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
    if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        if (auto castEditor = qobject_cast<AccessComboBox*>(editor))
        {
            castEditor->setCurrentValue(AccessTypes::str2Access(
                index.data(Qt::DisplayRole).toString(), AccessTypes::ACCESS_COUNT));
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
    {
        if (auto modfiedWriteEditor = qobject_cast<ModWriteComboBox*>(editor))
        {
            modfiedWriteEditor->setCurrentValue(General::str2ModifiedWrite(index.data(Qt::DisplayRole).toString()));
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::READ_ACTION)
    {
        if (auto readActionEditor = qobject_cast<ReadActionComboBox*>(editor))
        {
            readActionEditor->setCurrentValue(General::str2ReadAction(index.data(Qt::DisplayRole).toString()));
        }
    }
    
    else if (index.column() == FieldAccessPolicyColumns::TESTABLE)
    {
        if (auto testableEditor = qobject_cast<BooleanComboBox*>(editor))
        {
            testableEditor->setCurrentValue(index.data(Qt::DisplayRole).toString());
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::TEST_CONSTRAINT)
    {
        if (auto testConstraintEditor = qobject_cast<TestConstraintComboBox*>(editor))
        {
            testConstraintEditor->setCurrentValue(General::str2TestConstraint(index.data(Qt::DisplayRole).toString()));
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::WRITE_VALUE_CONSTRAINT)
    {
        if (auto writeConstraintEditor = qobject_cast<QComboBox*>(editor))
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
        // Get the modified mode references from interface and send to access policy.
        auto updatedModeRefs = modeRefInterface_->getModeReferences();

        QVariant modeRefsVariant = QVariant::fromValue(updatedModeRefs);
        model->setData(index, modeRefsVariant);
    }

    else if (index.column() == FieldAccessPolicyColumns::ACCESS)
    {
        if (auto accessEditor = qobject_cast<AccessComboBox*>(editor))
        {
            model->setData(index, accessEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::MODIFIED_WRITE)
    {
        if (auto modifiedWriteEditor = qobject_cast<ModWriteComboBox*>(editor))
        {
            model->setData(index, modifiedWriteEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::READ_ACTION)
    {
        if (auto readActionEditor = qobject_cast<ReadActionComboBox*>(editor))
        {
            model->setData(index, readActionEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::TESTABLE)
    {
        if (auto testableEditor = qobject_cast<BooleanComboBox*>(editor))
        {
            model->setData(index, testableEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::TEST_CONSTRAINT)
    {
        if (auto testConstraintEditor = qobject_cast<TestConstraintComboBox*>(editor))
        {
            model->setData(index, testConstraintEditor->currentText(), Qt::EditRole);
        }
    }

    else if (index.column() == FieldAccessPolicyColumns::WRITE_VALUE_CONSTRAINT)
    {
        if (auto writeConstraintEditor = qobject_cast<QComboBox*>(editor))
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
    if (auto editor = qobject_cast<QWidget*>(sender()))
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
    if (auto editor = qobject_cast<QWidget*>(sender()))
    {
        emit closeEditor(editor);
    }
}
