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
#include <IPXACTmodels/Enumeration.h>

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::ModelParameterDelegate()
//-----------------------------------------------------------------------------
ModelParameterDelegate::ModelParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, 
    QObject* parent):
ParameterDelegate(choices, parent)
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
    else if (index.column() == ModelParameterColumns::FORMAT) 
    {
        QComboBox* combo = new QComboBox(parent);
        combo->addItem(QString(""));
        combo->addItem(QString("bitString"));
        combo->addItem(QString("bool"));
        combo->addItem(QString("float"));
        combo->addItem(QString("long"));
        combo->addItem(QString("string"));
        return combo;
    }
    else
    {
        return ParameterDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ModelParameterDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
   QComboBox* combo = qobject_cast<QComboBox*>(editor);

	if (combo != 0) 
    {
		QString text = index.data(Qt::DisplayRole).toString();
		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		
		int comboIndex = combo->findText(text);
		combo->setCurrentIndex(comboIndex);
	}
    else 
    {
        ParameterDelegate::setEditorData(editor, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::setModelData()
//-----------------------------------------------------------------------------
void ModelParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const 
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);

    if (combo != 0)
    {
		QString text = combo->currentText();
        if (text == "<none>")
        {
            text = "";
        }
		model->setData(index, text, Qt::EditRole);
	}
    else
    {
        ParameterDelegate::setModelData(editor, model, index);
	}
}
//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::choiceColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::choiceColumn() const
{
    return ModelParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ModelParameterDelegate::valueColumn() const
{
    return ModelParameterColumns::VALUE;
}
