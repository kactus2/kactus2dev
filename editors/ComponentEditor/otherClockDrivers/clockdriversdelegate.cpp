//-----------------------------------------------------------------------------
// File: clockdriversdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// Delegate class to select editors used to edit otherClockDrivers of component.
//-----------------------------------------------------------------------------

#include "clockdriversdelegate.h"

#include "OtherClockDriverColumns.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ClockDriversDelegate::ClockDriversDelegate()
//-----------------------------------------------------------------------------
ClockDriversDelegate::ClockDriversDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ClockDriversDelegate::~ClockDriversDelegate()
//-----------------------------------------------------------------------------
ClockDriversDelegate::~ClockDriversDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: ClockDriversDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ClockDriversDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
	if (index.column() == OtherClockDriverColumns::NAME || index.column() == OtherClockDriverColumns::CLOCK_SOURCE)
    {
		QLineEdit* editor = new QLineEdit(parent);
		connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}

	else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD ||	
        index.column() == OtherClockDriverColumns::PULSE_OFFSET || 
        index.column() == OtherClockDriverColumns::PULSE_DURATION)
    {
			QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
			editor->setMinimum(0);
			editor->setSingleStep(0.1);
			connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return editor;
	}

	else if (index.column() == OtherClockDriverColumns::PULSE_VALUE)
    {
		QSpinBox* editor = new QSpinBox(parent);
		editor->setMinimum(0);
		editor->setMaximum(1);

		connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}

	else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD_UNIT ||
        index.column() == OtherClockDriverColumns::PULSE_OFFSET_UNIT ||
        index.column() == OtherClockDriverColumns::PULSE_DURATION_UNIT)
    {
		QComboBox* editor = new QComboBox(parent);
		editor->addItem(QString("ns"));
		editor->addItem(QString("ps"));

		connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}
	else
    {
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ClockDriversDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ClockDriversDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
	if (index.column() == OtherClockDriverColumns::NAME || index.column() == OtherClockDriverColumns::CLOCK_SOURCE)
    {
		QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
		QString text = index.data(Qt::DisplayRole).toString();
		lineEditor->setText(text);
	}

    else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD ||	
        index.column() == OtherClockDriverColumns::PULSE_OFFSET || 
        index.column() == OtherClockDriverColumns::PULSE_DURATION)
    {
        QDoubleSpinBox* spinEditor = qobject_cast<QDoubleSpinBox*>(editor);
        double value = index.data(Qt::DisplayRole).toDouble();
        spinEditor->setValue(value);
    }

	else if (index.column() == OtherClockDriverColumns::PULSE_VALUE)
    {
		QSpinBox* spinEditor = qobject_cast<QSpinBox*>(editor);
		int value = index.data(Qt::DisplayRole).toInt();
		spinEditor->setValue(value);
	}

    else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD_UNIT ||
        index.column() == OtherClockDriverColumns::PULSE_OFFSET_UNIT ||
        index.column() == OtherClockDriverColumns::PULSE_DURATION_UNIT)
    {

		QComboBox* comboEditor = qobject_cast<QComboBox*>(editor);
		QString value = index.data(Qt::DisplayRole).toString();
		int index = comboEditor->findText(value);
		comboEditor->setCurrentIndex(index);
	}
	else
    {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ClockDriversDelegate::setModelData()
//-----------------------------------------------------------------------------
void ClockDriversDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
	if (index.column() == OtherClockDriverColumns::NAME || index.column() == OtherClockDriverColumns::CLOCK_SOURCE)
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
		QString text = lineEditor->text();
		model->setData(index, text, Qt::EditRole);
	}

    else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD ||	
        index.column() == OtherClockDriverColumns::PULSE_OFFSET || 
        index.column() == OtherClockDriverColumns::PULSE_DURATION)
    {
			QDoubleSpinBox* spinEditor = qobject_cast<QDoubleSpinBox*>(editor);
			double value = spinEditor->value();
			model->setData(index, value, Qt::EditRole);
	}

	else if (index.column() == OtherClockDriverColumns::PULSE_VALUE)
    {
		QSpinBox* spinEditor = qobject_cast<QSpinBox*>(editor);
		int value = spinEditor->value();
		model->setData(index, value, Qt::EditRole);
	}

    else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD_UNIT ||
        index.column() == OtherClockDriverColumns::PULSE_OFFSET_UNIT ||
        index.column() == OtherClockDriverColumns::PULSE_DURATION_UNIT)
    {
		QComboBox* comboEditor = qobject_cast<QComboBox*>(editor);
		QString value = comboEditor->currentText();
		model->setData(index, value, Qt::EditRole);
	}
	else
    {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ClockDriversDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ClockDriversDelegate::commitAndCloseEditor()
{
	QSpinBox* spinBox = qobject_cast<QSpinBox*>(sender());
	QDoubleSpinBox* doubleSpin = qobject_cast<QDoubleSpinBox*>(sender());
	QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
	QWidget* widget = qobject_cast<QWidget*>(sender());

	if (comboBox)
    {
		emit commitData(comboBox);
		emit closeEditor(comboBox);
	}
	else if (doubleSpin)
    {
		emit commitData(doubleSpin);
		emit closeEditor(doubleSpin);
	}
	else if (spinBox)
    {
		emit commitData(spinBox);
		emit closeEditor(spinBox);
	}
	else if (lineEdit)
    {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
	else if (widget)
    {
		emit commitData(widget);
		emit closeEditor(widget);
	}
}
