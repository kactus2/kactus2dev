//-----------------------------------------------------------------------------
// File: AbstractionPortsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.06.2018
//
// Description:
// Master delegate for abstraction definition wire and transactional ports.
//-----------------------------------------------------------------------------

#include "AbstractionPortsDelegate.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

#include <library/LibraryInterface.h>

#include <QComboBox>
#include <QStringList>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::AbstractionPortsDelegate()
//-----------------------------------------------------------------------------
AbstractionPortsDelegate::AbstractionPortsDelegate(LibraryInterface* libraryAcces, QObject *parent):
QStyledItemDelegate(parent),
libraryAccess_(libraryAcces),
busDefinition_(0)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AbstractionPortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    const QModelIndex& index ) const
{
    if (index.column() == LogicalPortColumns::QUALIFIER)
    {
        QComboBox* box = new QComboBox(parent);
        QStringList list = getQualifierList();
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::BUSWIDTH)
    {
        QLineEdit* line = new QLineEdit(parent);
        connect(line, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return line;
    }
    else if (index.column() == LogicalPortColumns::MODE)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list = { "master", "slave", "system" };
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::PRESENCE)
    {
        QComboBox* box = new QComboBox(parent);

        QStringList list = { "required", "optional", "illegal" };
        box->addItems(list);

        connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return box;
    }
    else if (index.column() == LogicalPortColumns::SYSTEM_GROUP)
    {
        if (busDefinition_)
        {
            QComboBox* box = new QComboBox(parent);
            box->addItem("");
            box->addItems(BusDefinitionUtils::getSystemGroups(busDefinition_, libraryAccess_));

            connect(box, SIGNAL(destroyed()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return box;
        }
        else
        {
            return 0;
        }
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index ) const
{
    if (editorIsLineEditor(index.column()))
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "BusPortsDelegate::setEditorData", "Type conversion failed for QLineEdit");

        QString text = index.data(Qt::DisplayRole).toString();
        line->setText(text);

        if (index.column() == LogicalPortColumns::NAME && text == QLatin1String("unnamed"))
        {
            line->clear();
        }
    }
    else if (editorIsComboBox(index.column()))
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(box, "BusPortsDelegate::setEditorData", "Type conversion failed for combo box");

        QString text = index.data(Qt::DisplayRole).toString();
        int textIndex = box->findText(text);
        if (textIndex >= 0)
        {
            box->setCurrentIndex(textIndex);
        }
        else
        {
            box->setCurrentText(text);
        }
    }

    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::editorIsLineEditor()
//-----------------------------------------------------------------------------
bool AbstractionPortsDelegate::editorIsLineEditor(int indexColumn) const
{
    return indexColumn == LogicalPortColumns::NAME || indexColumn == LogicalPortColumns::WIDTH ||
        indexColumn == LogicalPortColumns::BUSWIDTH || indexColumn == LogicalPortColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::editorIsComboBox()
//-----------------------------------------------------------------------------
bool AbstractionPortsDelegate::editorIsComboBox(int indexColumn) const
{
    return indexColumn == LogicalPortColumns::QUALIFIER || indexColumn == LogicalPortColumns::MODE ||
        indexColumn == LogicalPortColumns::PRESENCE || indexColumn == LogicalPortColumns::SYSTEM_GROUP;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (editorIsLineEditor(index.column()))
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(lineEditor, "BusPortsDelegate::setModelData", "Type conversion failed for normal QLineEdit");

        model->setData(index, lineEditor->text(), Qt::EditRole);
    }
    else if (editorIsComboBox(index.column()))
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
// Function: AbstractionPortsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::commitAndCloseEditor()
{
	QWidget* editor = qobject_cast<QWidget*>(sender());

	if (editor)
    {
		emit commitData(editor);
		emit closeEditor(editor);
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::setBusDef()
//-----------------------------------------------------------------------------
void AbstractionPortsDelegate::setBusDef(QSharedPointer<const BusDefinition> busDefinition)
{
    busDefinition_ = busDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionPortsDelegate::getQualifierList()
//-----------------------------------------------------------------------------
QStringList AbstractionPortsDelegate::getQualifierList() const
{
    QStringList list = { "none", "address", "data", "data/address" };
    return list;
}
