/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsdelegate.cpp
 *		Project: Kactus 2
 */

#include "memorymapsdelegate.h"

#include <editors/ComponentEditor/memoryMaps/MemoryMapsColumns.h>
#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPen>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTextEdit>

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::MemoryMapsDelegate()
//-----------------------------------------------------------------------------
MemoryMapsDelegate::MemoryMapsDelegate(QStringList remapStateNames, QObject *parent):
MultilineDescriptionDelegate(parent),
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
    else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
    {
        QLineEdit* edit = new QLineEdit(parent);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        edit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\d*"), edit));
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
        return MultilineDescriptionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::setEditorData()
//-----------------------------------------------------------------------------
void MemoryMapsDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    if (index.column() == MemoryMapsColumns::NAME_COLUMN ||
        index.column() == MemoryMapsColumns::AUB_COLUMN)
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
        MultilineDescriptionDelegate::setEditorData(editor, index);
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
    else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        int editorValue = edit->text().toUInt();
        model->setData(index, editorValue, Qt::EditRole);
    }
    else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        ReferenceSelector* referenceSelector = qobject_cast<ReferenceSelector*>(editor);
        QString text = referenceSelector->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        MultilineDescriptionDelegate::setModelData(editor, model, index);
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