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

#include <editors/ComponentEditor/common/ModeReferenceEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/modWriteComboBox/modwritecombobox.h>
#include <common/widgets/readActionComboBox/readactioncombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/testConstraintComboBox/testconstraintcombobox.h>

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate()
//-----------------------------------------------------------------------------
FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate(ParameterCompleter* parameterNameCompleter, 
    QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent)
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

    else if (index.column() == FieldAccessPolicyColumns::RESERVED)
    {
        auto reservedEditor = qobject_cast<QLineEdit*>(editor);

        if (reservedEditor)
        {
            reservedEditor->setText(index.data(Qt::DisplayRole).toString());
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

    else if (index.column() == FieldAccessPolicyColumns::RESERVED)
    {
        auto reservedEditor = qobject_cast<QLineEdit*>(editor);

        if (reservedEditor)
        {
            model->setData(index, reservedEditor->text(), Qt::EditRole);
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
