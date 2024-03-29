//-----------------------------------------------------------------------------
// File: memorymapsdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The delegate to provide editor for adding/removing/editing the memory maps of a component.
//-----------------------------------------------------------------------------

#include "memorymapsdelegate.h"

#include <editors/ComponentEditor/memoryMaps/MemoryMapsColumns.h>
#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <QLineEdit>
#include <QPainter>
#include <QPen>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::MemoryMapsDelegate()
//-----------------------------------------------------------------------------
MemoryMapsDelegate::MemoryMapsDelegate(QAbstractItemModel* completionModel,
    QSharedPointer<ParameterFinder> parameterFinder,
    QStringList remapStateNames, QObject *parent):
ExpressionDelegate(completionModel, parameterFinder, parent),
    remapStateNames_(remapStateNames)
{

}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::~MemoryMapsDelegate()
//-----------------------------------------------------------------------------
MemoryMapsDelegate::~MemoryMapsDelegate()
{
    
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* MemoryMapsDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index ) const
{
    if (index.column() == MemoryMapsColumns::NAME_COLUMN)
    {
        QLineEdit* edit = new QLineEdit(parent);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return edit;
    }
    else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        ReferenceSelector* referenceSelector = new ReferenceSelector(parent);

        referenceSelector->refresh(remapStateNames_);
        return referenceSelector;
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::setEditorData()
//-----------------------------------------------------------------------------
void MemoryMapsDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    if (index.column() == MemoryMapsColumns::NAME_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        
        if (text == tr("No remap state selected."))
        {
            text = "";
        }

        ReferenceSelector* referenceSelector = qobject_cast<ReferenceSelector*>(editor);

        referenceSelector->selectItem(text);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::setModelData()
//-----------------------------------------------------------------------------
void MemoryMapsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
    if (index.column() == MemoryMapsColumns::NAME_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        ReferenceSelector* referenceSelector = qobject_cast<ReferenceSelector*>(editor);
        QString text = referenceSelector->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int MemoryMapsDelegate::descriptionColumn() const
{
    return MemoryMapsColumns::DESCRIPTION_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::paint()
//-----------------------------------------------------------------------------
void MemoryMapsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QPen oldPen = painter->pen();
    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);

    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

    painter->setPen(oldPen);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool MemoryMapsDelegate::columnAcceptsExpression(int column) const
{
    return column == MemoryMapsColumns::IS_PRESENT || column == MemoryMapsColumns::AUB_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void MemoryMapsDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::updateRemapStateNames()
//-----------------------------------------------------------------------------
void MemoryMapsDelegate::updateRemapStateNames(QStringList newRemapStateNames)
{
    remapStateNames_ = newRemapStateNames;
}