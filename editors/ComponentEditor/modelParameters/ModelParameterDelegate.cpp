//-----------------------------------------------------------------------------
// File: ModelParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------

#include "ModelParameterDelegate.h"

#include "ModelParameterColumns.h"

#include <IPXACTmodels/choice.h>

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::ModelParameterDelegate()
//-----------------------------------------------------------------------------
ModelParameterDelegate::ModelParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, 
    QObject *parent /*= 0*/ ):
QStyledItemDelegate(parent), choices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::~ModelParameterDelegate()
//-----------------------------------------------------------------------------
ModelParameterDelegate::~ModelParameterDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ModelParameterDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
	if (index.column() == ModelParameterColumns::USAGE_TYPE) 
    {
		QComboBox* combo = new QComboBox(parent);
		combo->addItem(QString("typed"));
		combo->addItem(QString("nontyped"));
		return combo;
	}
    else if (index.column() == ModelParameterColumns::CHOICE) 
    {
        QComboBox* combo = new QComboBox(parent);
        combo->addItem(QString(""));
        foreach (QSharedPointer<Choice> choice, *choices_)
        {
            combo->addItem(choice->getName());
        }

        return combo;
    }
	else 
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ModelParameterDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
	if (index.column() == ModelParameterColumns::USAGE_TYPE ||
        index.column() == ModelParameterColumns::CHOICE) 
    {
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		
		int comboIndex = combo->findText(text);
		combo->setCurrentIndex(comboIndex);
	}
	else 
    {
        // use the line edit for other columns
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
        lineEdit->setText(text);
	}
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::setModelData()
//-----------------------------------------------------------------------------
void ModelParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const 
{
    if (index.column() == ModelParameterColumns::USAGE_TYPE ||
        index.column() == ModelParameterColumns::CHOICE)
    {
		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
	}
	else 
    {
        QStyledItemDelegate::setModelData(editor, model, index);
	}
}

