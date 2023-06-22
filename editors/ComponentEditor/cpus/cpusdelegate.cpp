//-----------------------------------------------------------------------------
// File: cpusdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The delegate to provide editors to add/remove/edit the cpus of a component.
//-----------------------------------------------------------------------------

#include "cpusdelegate.h"

#include "CpuColumns.h"


#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <IPXACTmodels/Component/Component.h>

#include <QAbstractItemView>
#include <QLineEdit>
#include <QStringList>
#include <QString>

//-----------------------------------------------------------------------------
// Function: CpusDelegate::CpusDelegate()
//-----------------------------------------------------------------------------
CpusDelegate::CpusDelegate(QSharedPointer<Component> component, QCompleter* parameterNameCompleter,
    QSharedPointer<ParameterFinder> parameterFinder, QObject* parent):
EnumerationEditorConstructorDelegate(parent),
component_(component),
parameterNameCompleter_(parameterNameCompleter),
parameterFinder_(parameterFinder)
{
    Q_ASSERT(component);
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* CpusDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    const int column = index.column();
    if (column == CpuColumns::NAME || column == CpuColumns::DISPLAY_NAME ||
        column == CpuColumns::SHORT_DESCRIPTION || column ==  CpuColumns::DESCRIPTION)
    {
        auto lineEdit = new QLineEdit(parent);
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return lineEdit;
    }
    else if (columnAcceptsExpression(column))
    {
        auto editor = new ExpressionEditor(parameterFinder_, parent);
        editor->setAppendingCompleter(parameterNameCompleter_);

        connect(editor, SIGNAL(increaseReference(QString const&)),
            this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
        connect(editor, SIGNAL(decreaseReference(QString const&)),
            this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

        return editor;
    }
    else if (column == CpuColumns::MEMORY_MAP)
    {
        auto referenceSelector = new ReferenceSelector(parent);

        QStringList names = component_->getMemoryMapNames();
        QString selectedName = index.model()->data(index, Qt::DisplayRole).toString();
        if (selectedName.isEmpty() == false && names.contains(selectedName) == false)
        {
            names.append(selectedName);
        }

        referenceSelector->refresh(names);

        return referenceSelector;
    }
    else
    {
        return EnumerationEditorConstructorDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::setEditorData()
//-----------------------------------------------------------------------------
void CpusDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == CpuColumns::NAME ||
        index.column() == CpuColumns::DISPLAY_NAME ||
        index.column() == CpuColumns::SHORT_DESCRIPTION ||
        index.column() ==  CpuColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else if (columnAcceptsExpression(index.column()))
    {
        auto expressionEditor = qobject_cast<ExpressionEditor*>(editor);
        expressionEditor->setExpression(index.data(Qt::EditRole).toString());
    }
    else if (index.column() == CpuColumns::MEMORY_MAP)
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        ReferenceSelector* referenceSelector = qobject_cast<ReferenceSelector*>(editor);

        referenceSelector->selectItem(text);
    }
    else
    {
        EnumerationEditorConstructorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::setModelData()
//-----------------------------------------------------------------------------
void CpusDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == CpuColumns::NAME ||
        index.column() == CpuColumns::DISPLAY_NAME ||
        index.column() == CpuColumns::SHORT_DESCRIPTION ||
        index.column() ==  CpuColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else if (columnAcceptsExpression(index.column()))
    {
        auto expressionEditor = qobject_cast<ExpressionEditor*>(editor);
        expressionEditor->finishEditingCurrentWord();
        model->setData(index, expressionEditor->getExpression(), Qt::EditRole);

        parameterNameCompleter_->popup()->hide();
    }
    else if (index.column() == CpuColumns::MEMORY_MAP)
    {
        auto referenceSelector = qobject_cast<ReferenceSelector*>(editor);
        QString text = referenceSelector->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        EnumerationEditorConstructorDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::eventFilter()
//-----------------------------------------------------------------------------
bool CpusDelegate::eventFilter(QObject* editor, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto keyEvent = dynamic_cast<QKeyEvent*>(event);
        if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) &&
            keyEvent->modifiers() == Qt::NoModifier)
        {
            auto editorWidget = dynamic_cast<QWidget*>(editor);
            commitData(editorWidget);
            closeEditor(editorWidget);
            return true;
        }
    }

    return QStyledItemDelegate::eventFilter(editor, event);
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::isEnumerationEditorColumn()
//-----------------------------------------------------------------------------
bool CpusDelegate::isEnumerationEditorColumn(QModelIndex const& index) const
{
    return index.column() == CpuColumns::ADDRSPACE;
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::getCurrentSelection()
//-----------------------------------------------------------------------------
QStringList CpusDelegate::getCurrentSelection(QModelIndex const& index) const
{
    return index.data(CpuColumns::USER_DISPLAY_ROLE).toStringList();
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::getAvailableItems()
//-----------------------------------------------------------------------------
QStringList CpusDelegate::getAvailableItems() const
{
    return component_->getAddressSpaceNames();
}

//-----------------------------------------------------------------------------
// Function: cpusdelegate::setEnumerationDataToModel()
//-----------------------------------------------------------------------------
void CpusDelegate::setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model,
    QStringList const& selectedItems) const
{
    model->setData(index, selectedItems, CpuColumns::USER_EDIT_ROLE);
}

//-----------------------------------------------------------------------------
// Function: CpusDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool CpusDelegate::columnAcceptsExpression(int column) const
{
    return column == CpuColumns::RANGE || column == CpuColumns::WIDTH || column == CpuColumns::AUB;
}
