/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: enumeratedvaluedelegate.cpp
 *		Project: Kactus 2
 */

#include "enumeratedvaluedelegate.h"
#include "EnumeratedValueColumns.h"

#include <QLineEdit>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: enumeratedvaluedelegate::EnumeratedValueDelegate()
//-----------------------------------------------------------------------------
EnumeratedValueDelegate::EnumeratedValueDelegate(QObject *parent):
QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluedelegate::~EnumeratedValueDelegate()
//-----------------------------------------------------------------------------
EnumeratedValueDelegate::~EnumeratedValueDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluedelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* EnumeratedValueDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index ) const
{
    if (index.column() == EnumeratedValueColumns::USAGE_COLUMN)
    {
        QComboBox* combo = new QComboBox(parent);
        combo->addItem(tr("read-write"));
        combo->addItem(tr("read"));
        combo->addItem(tr("write"));
        return combo;
    }

    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluedelegate::setEditorData()
//-----------------------------------------------------------------------------
void EnumeratedValueDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    if (index.column() == EnumeratedValueColumns::USAGE_COLUMN)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        int comboIndex = combo->findText(text);
        combo->setCurrentIndex(comboIndex);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluedelegate::setModelData()
//-----------------------------------------------------------------------------
void EnumeratedValueDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index )
    const
{
    if (index.column() == EnumeratedValueColumns::USAGE_COLUMN)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        QString text = combo->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: enumeratedvaluedelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void EnumeratedValueDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
