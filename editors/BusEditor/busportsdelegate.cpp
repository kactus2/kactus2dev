/* 
 *  Created on: 27.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsdelegate.cpp
 */

#include "busportsdelegate.h"
#include "LogicalPortColumns.h"

#include <QComboBox>
#include <QStringList>
#include <QLineEdit>
#include <QIntValidator>
#include <QSpinBox>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: BusPortsDelegate::BusPortsDelegate()
//-----------------------------------------------------------------------------
BusPortsDelegate::BusPortsDelegate(QObject *parent): QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsDelegate::~BusPortsDelegate()
//-----------------------------------------------------------------------------
BusPortsDelegate::~BusPortsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: BusPortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* BusPortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    const QModelIndex& index ) const
{
    if (index.column() == LogicalPortColumns::NAME || 
        index.column() == LogicalPortColumns::GROUP ||
        index.column() == LogicalPortColumns::DESCRIPTION)
    {
        QLineEdit* line = new QLineEdit(parent);
        connect(line, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return line;
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        QComboBox* box = new QComboBox(parent);
        QStringList list;
        list.append("address");
        list.append("data");
        list.append("data/address");
        list.append("clock");
        list.append("reset");
        list.append("any");

        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::WIDTH)
    {
        QLineEdit* line = new QLineEdit(parent);

        // the validator for editor input
        QRegExpValidator* validator = new QRegExpValidator(QRegExp("[0-9]{0,5}"), line);
        line->setValidator(validator);

        connect(line, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return line;
    }
    else if (index.column() == LogicalPortColumns::DEFAULT_VALUE)
    {
        // the editor
        QLineEdit* line = new QLineEdit(parent);

        // the validator for editor input
        QIntValidator* validator = new QIntValidator(line);
        validator->setRange(0, 99999);

        line->setValidator(validator);

        connect(line, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return line;
    }
    else if (index.column() == LogicalPortColumns::MODE)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list;
        list.append(QString("master"));
        list.append(QString("slave"));
        list.append(QString("system"));
        list.append(QString("any"));
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::DIRECTION)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list;
        list.append(QString("in"));
        list.append(QString("out"));
        list.append(QString("inout"));
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list;
        list.append(QString("required"));
        list.append(QString("optional"));
        list.append(QString("illegal"));
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::DRIVER)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list;
        list.append(QString("none"));
        list.append(QString("any"));
        list.append(QString("clock"));
        list.append(QString("singleShot"));
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void BusPortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index ) const
{
    if (index.column() == LogicalPortColumns::NAME)
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "BusPortsDelegate::setEditorData", "Type conversion failed for QLineEdit");

        QString text = index.data(Qt::DisplayRole).toString();
        if (text != "unnamed")
        {
            line->setText(text);
        }            
    }
    else if (index.column() == LogicalPortColumns::WIDTH ||
        index.column() == LogicalPortColumns::GROUP ||
        index.column() == LogicalPortColumns::DESCRIPTION ||
        index.column() == LogicalPortColumns::DEFAULT_VALUE)
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "BusPortsDelegate::setEditorData",	"Type conversion failed for QLineEdit");

        QString text = index.data(Qt::DisplayRole).toString();
        line->setText(text);
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER ||
        index.column() == LogicalPortColumns::MODE ||
        index.column() == LogicalPortColumns::DIRECTION ||
        index.column() == LogicalPortColumns::PRESENCE ||
        index.column() == LogicalPortColumns::DRIVER)
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(box, "BusPortsDelegate::setEditorData", "Type conversion failed for combo box");

        QString text = index.data(Qt::DisplayRole).toString();

        box->setCurrentIndex(box->findText(text));
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void BusPortsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == LogicalPortColumns::NAME ||
        index.column() == LogicalPortColumns::WIDTH ||
        index.column() == LogicalPortColumns::GROUP ||
        index.column() == LogicalPortColumns::DEFAULT_VALUE ||
        index.column() == LogicalPortColumns::DESCRIPTION)
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(lineEditor, "BusPortsDelegate::setModelData", "Type conversion failed for normal QLineEdit");

        model->setData(index, lineEditor->text(), Qt::EditRole);
    }
    else if (index.column() == LogicalPortColumns::QUALIFIER ||
        index.column() == LogicalPortColumns::MODE ||
        index.column() == LogicalPortColumns::DIRECTION ||
        index.column() == LogicalPortColumns::PRESENCE || 
        index.column() == LogicalPortColumns::DRIVER)
    {
        QComboBox* selector = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(selector, "BusPortsDelegate::setModelData", "Type conversion failed for combo box");

        model->setData(index, selector->currentText(), Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void BusPortsDelegate::commitAndCloseEditor()
{
	QComboBox* combo = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

	if (combo)
    {
		emit commitData(combo);
		emit closeEditor(combo);
	}
	else if (lineEdit)
    {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
}