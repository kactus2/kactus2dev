//-----------------------------------------------------------------------------
// File: portsdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.04.2011
//
// Description:
// The delegate that provides editors to edit ports of a component.
//-----------------------------------------------------------------------------

#include "portsdelegate.h"

#include "PortColumns.h"

#include <kactusGenerators/vhdlGenerator/vhdlgeneral.h>

#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: PortsDelegate::PortsDelegate()
//-----------------------------------------------------------------------------
PortsDelegate::PortsDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QObject *parent) : ExpressionDelegate(parameterCompleter, parameterFinder, parent), 
    adhocGroupModify_(false), adhocGroupState_(Qt::Unchecked)
{

}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::~PortsDelegate()
//-----------------------------------------------------------------------------
PortsDelegate::~PortsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == PortColumns::DIRECTION)
    {
        return createSelectorForDirection(parent);
    }
    else if (index.column() == PortColumns::TYPE_NAME)
    {
        return createSelectorWithVHDLTypes(parent);
    }
    else if (index.column() == PortColumns::TYPE_DEF)
    {
        return createSelectorWithVHDLStandardLibraries(parent);
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
if (index.column() == PortColumns::DIRECTION)
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        int comboIndex = combo->findText(text);
        combo->setCurrentIndex(comboIndex);
    }
    else if (index.column() == PortColumns::TYPE_NAME || index.column() == PortColumns::TYPE_DEF)
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        int comboIndex = combo->findText(text);
        // if the text is not found
        if (comboIndex < 0)
        {
            combo->setEditText(text);
        }
        else
        {
            combo->setCurrentIndex(comboIndex);
        }
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index ) const
{
    if (index.column() == PortColumns::DIRECTION ||
        index.column() == PortColumns::TYPE_NAME ||
        index.column() == PortColumns::TYPE_DEF)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool PortsDelegate::editorEvent(QEvent *event, QAbstractItemModel* model, 
    QStyleOptionViewItem const& option, QModelIndex const& index)
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
void PortsDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.column() == PortColumns::ADHOC_VISIBILITY)
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

//-----------------------------------------------------------------------------
// Function: PortsDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool PortsDelegate::columnAcceptsExpression(int column) const
{
    return column == PortColumns::DEFAULT_VALUE || column == PortColumns::LEFT_BOUND ||
        column == PortColumns::RIGHT_BOUND || column == PortColumns::ARRAY_LEFT ||
        column == PortColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::createSelectorForDirection()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createSelectorForDirection(QWidget* parent) const
{
    QComboBox* directionSelector = new QComboBox(parent);
    directionSelector->addItem("in");
    directionSelector->addItem("out");
    directionSelector->addItem("inout");
    directionSelector->addItem("phantom");

    return directionSelector;
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::createSelectorWithVHDLTypes()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createSelectorWithVHDLTypes(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->setEditable(true);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    for (unsigned int i = 0; i < VhdlGeneral::VHDL_TYPE_COUNT; ++i)
    {
        combo->addItem(VhdlGeneral::VHDL_TYPES[i]);
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::createSelectorWithVHDLStandardLibraries()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createSelectorWithVHDLStandardLibraries(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->setEditable(true);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    for (unsigned int i = 0;i < VhdlGeneral::VHDL_TYPEDEF_COUNT; ++i)
    {
        combo->addItem(VhdlGeneral::VHDL_TYPE_DEFINITIONS[i]);
    }

    return combo;
}
