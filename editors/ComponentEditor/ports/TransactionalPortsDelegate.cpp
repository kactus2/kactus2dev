//-----------------------------------------------------------------------------
// File: TransactionalPortsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.05.2019
//
// Description:
// The delegate that provides editors to edit transactional ports of a component.
//-----------------------------------------------------------------------------

#include "TransactionalPortsDelegate.h"

#include <editors/ComponentEditor/ports/TransactionalPortColumns.h>

#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/Component/Component.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::TransactionalPortsDelegate()
//-----------------------------------------------------------------------------
TransactionalPortsDelegate::TransactionalPortsDelegate(QSharedPointer<Component> component,
    QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortTypeValidator> typeValidator, QObject* parent):
PortsDelegate(component, parameterCompleter, parameterFinder, typeValidator, parent)
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* TransactionalPortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == TransactionalPortColumns::INITIATIVE)
    {
        return createSelectorForInitiative(parent);
    }
    else if (index.column() == TransactionalPortColumns::KIND)
    {
        return createSelectorForKind(parent);
    }
    else
    {
        return PortsDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::createSelectorForInitiative()
//-----------------------------------------------------------------------------
QWidget* TransactionalPortsDelegate::createSelectorForInitiative(QWidget* parent) const
{
    QComboBox* initiativeSelector = new QComboBox(parent);

    for (auto initiative : TransactionalTypes::INITIATIVE_TYPES)
    {
        initiativeSelector->addItem(initiative);
    }

    return initiativeSelector;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::createSelectorForKind()
//-----------------------------------------------------------------------------
QWidget* TransactionalPortsDelegate::createSelectorForKind(QWidget* parent) const
{
    QComboBox* kindSelector = new QComboBox(parent);

    for (auto kind : TransactionalTypes::KIND_TYPES)
    {
        if (kind.compare("custom", Qt::CaseInsensitive) != 0)
        {
            kindSelector->addItem(kind);
        }
    }

    kindSelector->setEditable(true);
    return kindSelector;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void TransactionalPortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == TransactionalPortColumns::INITIATIVE || index.column() == TransactionalPortColumns::KIND)
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        int comboIndex = combo->findText(text);
        combo->setCurrentIndex(comboIndex);
    }
    else
    {
        PortsDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void TransactionalPortsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (index.column() == TransactionalPortColumns::INITIATIVE || index.column() == TransactionalPortColumns::KIND)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        PortsDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool TransactionalPortsDelegate::columnAcceptsExpression(int column) const
{
    return PortsDelegate::columnAcceptsExpression(column) || column == TransactionalPortColumns::WIDTH||
        column == TransactionalPortColumns::MAX_CONNECTIONS || column == TransactionalPortColumns::MIN_CONNECTIONS;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsDelegate::descriptionColumn() const
{
    return TransactionalPortColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::nameColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsDelegate::nameColumn() const
{
    return TransactionalPortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::typeColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsDelegate::typeColumn() const
{
    return TransactionalPortColumns::TYPE_NAME;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::tagColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsDelegate::tagColumn() const
{
    return TransactionalPortColumns::TAG_GROUP;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::adHocColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsDelegate::adHocColumn() const
{
    return TransactionalPortColumns::ADHOC_VISIBILITY;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::arrayLeftColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsDelegate::arrayLeftColumn() const
{
    return TransactionalPortColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsDelegate::arrayRightColumn()
//-----------------------------------------------------------------------------
int TransactionalPortsDelegate::arrayRightColumn() const
{
    return TransactionalPortColumns::ARRAY_RIGHT;
}
