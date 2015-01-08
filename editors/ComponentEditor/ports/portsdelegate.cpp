/* 
 *
 *  Created on: 1.4.2011
 *      Author: Antti Kamppi
 * 		filename: portsdelegate.cpp
 */

#include "portsdelegate.h"

#include <kactusGenerators/vhdlGenerator/vhdlgeneral.h>

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: PortsDelegate()
//-----------------------------------------------------------------------------
PortsDelegate::PortsDelegate(QObject *parent) : QStyledItemDelegate(parent), adhocGroupModify_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~PortsDelegate()
//-----------------------------------------------------------------------------
PortsDelegate::~PortsDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createEditor( QWidget* parent, 
									 const QStyleOptionViewItem& option, 
									 const QModelIndex& index ) const
{
	// if the column is the one specified for direction items 
    switch (index.column())
    {
    case PORT_COL_DIRECTION:
        {
		    QComboBox* combo = new QComboBox(parent);
		    combo->addItem(QString("in"));
		    combo->addItem(QString("out"));
		    combo->addItem(QString("inout"));
		    combo->addItem(QString("phantom"));
		    connect(combo, SIGNAL(currentIndexChanged(int)),
			    this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		    return combo;
        }

    case PORT_COL_LEFT:
    case PORT_COL_RIGHT:
        {
            QLineEdit* boundaryEdit = new QLineEdit(parent);
            connect(boundaryEdit, SIGNAL(editingFinished()), 
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return boundaryEdit;
        }
    case PORT_COL_WIDTH:
        {
            QSpinBox* spinBox = new QSpinBox(parent);
            spinBox->setRange(0, 2048);
            spinBox->setSingleStep(1);
            connect(spinBox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return spinBox;
        }

    case PORT_COL_DEFAULT:
        {
            QLineEdit* defaultEdit = new QLineEdit(parent);
            connect(defaultEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return defaultEdit;
        }

    case PORT_COL_TYPENAME:
        {
            QComboBox* combo = new QComboBox(parent);

            for (unsigned int i = 0; i < VhdlGeneral::VHDL_TYPE_COUNT; ++i) {
                combo->addItem(VhdlGeneral::VHDL_TYPES[i]);
            }
            combo->setEditable(true);
            combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

            connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return combo;
        }

    case PORT_COL_TYPEDEF:
        {
            QComboBox* combo = new QComboBox(parent);

            for (unsigned int i = 0;i < VhdlGeneral::VHDL_TYPEDEF_COUNT; ++i) {
                combo->addItem(VhdlGeneral::VHDL_TYPE_DEFINITIONS[i]);
            }
            combo->setEditable(true);
            combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

            connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return combo;
        }

    case PORT_COL_ADHOC_VISIBILITY:
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }

    default:
        {
            // use line edit to edit all other columns
            QLineEdit* editor = new QLineEdit(parent);
            connect(editor, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return editor;
        }	
	}
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortsDelegate::setEditorData( QWidget* editor, 
								  const QModelIndex& index ) const
{
	switch (index.column())
    {
    case PORT_COL_DIRECTION:
        {
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QComboBox* combo = qobject_cast<QComboBox*>(editor);

            int comboIndex = combo->findText(text);
            combo->setCurrentIndex(comboIndex);
            break;
        }

    case PORT_COL_LEFT:
    case PORT_COL_RIGHT:
        {
            QLineEdit* boundaryEdit = qobject_cast<QLineEdit*>(editor);
            QString value = index.model()->data(index, Qt::EditRole).toString().simplified();
            boundaryEdit->setText(value);
            break;
        }

    case PORT_COL_WIDTH:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
            int value = index.model()->data(index, Qt::DisplayRole).toInt();
            spinBox->setValue(value);
            break;
        }

    case PORT_COL_DEFAULT:
        {
            QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
            QString value = index.model()->data(index, Qt::EditRole).toString().simplified();
            defaultEdit->setText(value);
            break;
        }

    case PORT_COL_TYPENAME:
        {
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QComboBox* combo = qobject_cast<QComboBox*>(editor);

            int comboIndex = combo->findText(text);
            // if the text is not found
            if (comboIndex < 0) {
                combo->setEditText(text);
            }
            // if the text was found
            else {
                combo->setCurrentIndex(comboIndex);
            }

            break;
        }

    case PORT_COL_TYPEDEF:
        {
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QComboBox* combo = qobject_cast<QComboBox*>(editor);

            int comboIndex = combo->findText(text);
            // if the text is not found
            if (comboIndex < 0) {
                combo->setEditText(text);
            }
            // if the text was found
            else {
                combo->setCurrentIndex(comboIndex);
            }

            break;
        }

    case PORT_COL_ADHOC_VISIBILITY:
        {
            QStyledItemDelegate::setEditorData(editor, index);
            break;
        }

    default:
        {
            // use the line edit for other columns
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
            lineEdit->setText(text);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortsDelegate::setModelData( QWidget* editor, 
								 QAbstractItemModel* model, 
								 const QModelIndex& index ) const
{
    switch (index.column())
    {
    case PORT_COL_DIRECTION:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            QString text = combo->currentText();
            model->setData(index, text, Qt::EditRole);
            break;
        }

    case PORT_COL_LEFT:
    case PORT_COL_RIGHT:
        {
            QLineEdit* boundaryEdit = qobject_cast<QLineEdit*>(editor);
            QString value = boundaryEdit->text().simplified();
            model->setData(index, value, Qt::EditRole);
            break;
        }
    case PORT_COL_WIDTH:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
            int value = spinBox->value();
            model->setData(index, value, Qt::EditRole);
            break;
        }

    case PORT_COL_DEFAULT:
        {
            QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
            QString value = defaultEdit->text().simplified();
            model->setData(index, value, Qt::EditRole);
            break;
        }

    case PORT_COL_TYPENAME:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            QString text = combo->currentText();
            model->setData(index, text, Qt::EditRole);
            break;
        }

    case PORT_COL_TYPEDEF:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            QString text = combo->currentText();
            model->setData(index, text, Qt::EditRole);
            break;
        }

    case PORT_COL_ADHOC_VISIBILITY:
        {
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
        }

    default:
        {
            // get the text from line edit for other columns
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
            QString text = lineEdit->text();
            model->setData(index, text, Qt::EditRole);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void PortsDelegate::commitAndCloseEditor()
{
	// try to get pointer to editor in both cases
	QComboBox* combo = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
	QSpinBox* spinBox = qobject_cast<QSpinBox*>(sender());

	// if the editor was combo box
	if (combo) {
		emit commitData(combo);
	}

	// if the editor was spinBox
	else if (spinBox) {
		emit commitData(spinBox);
		emit closeEditor(spinBox);
	}

	// if editor was line edit
	else {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool PortsDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Always reset the ad-hoc group modify flag.
    if (event->type() == QEvent::MouseButtonRelease)
    {
        adhocGroupModify_ = false;
    }

    // Make sure that the item is checkable.
    Qt::ItemFlags flags = model->flags(index);

    if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
    {
        return false;
    }

    // Make sure that we have a check state.
    QVariant value = index.data(Qt::CheckStateRole);

    if (!value.isValid())
    {
        return false;
    }

    Qt::CheckState newState = static_cast<Qt::CheckState>(value.toInt());

    // Handle the mouse button events.
    if (event->type() == QEvent::MouseButtonPress)
    {
        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            option.decorationSize,
            QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
            option.rect.width() - (2 * textMargin),
            option.rect.height()));

        if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
        {
            return false;
        }

        newState = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        adhocGroupModify_ = true;
        adhocGroupState_ = newState;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!adhocGroupModify_ || static_cast<Qt::CheckState>(value.toInt()) == adhocGroupState_)
        {
            return false;
        }

        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            option.decorationSize,
            QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
            option.rect.width() - (2 * textMargin),
            option.rect.height()));

        if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
        {
            return false;
        }
        
        newState = adhocGroupState_;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
            static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return model->setData(index, newState, Qt::CheckStateRole);
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::paint()
//-----------------------------------------------------------------------------
void PortsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.column() == PORT_COL_ADHOC_VISIBILITY)
    {
        painter->fillRect(option.rect, Qt::white);

        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                            QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
                                            QRect(option.rect.x() + textMargin, option.rect.y(),
                                                  option.rect.width() - (2 * textMargin), option.rect.height()));
        viewItemOption.rect = newRect;
    }

    QStyledItemDelegate::paint(painter, viewItemOption, index);
}

