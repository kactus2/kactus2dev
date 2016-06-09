//-----------------------------------------------------------------------------
// File: PortMapTreeDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// Delegate to provide editors to edit bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapTreeDelegate.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <QPen>
#include <QIcon>
#include <QApplication>
#include <QMouseEvent>

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::PortMapTreeDelegate()
//-----------------------------------------------------------------------------
PortMapTreeDelegate::PortMapTreeDelegate(QSharedPointer<Component> component, QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> finder, QStringList portNames, LibraryInterface* libraryHandler,
    QObject *parent):
ExpressionDelegate(parameterCompleter, finder, parent),
physicalPortNames_(portNames),
logicalPortNames_(),
component_(component),
libraryHandler_(libraryHandler),
invertModify_(false),
invertCheckState_(Qt::Unchecked)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::~PortMapTreeDelegate()
//-----------------------------------------------------------------------------
PortMapTreeDelegate::~PortMapTreeDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::updatePortNames()
//-----------------------------------------------------------------------------
void PortMapTreeDelegate::updatePortNames(QStringList newPortNames)
{
    physicalPortNames_ = newPortNames;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::updateLogicalPortNames()
//-----------------------------------------------------------------------------
void PortMapTreeDelegate::updateLogicalPortNames(VLNV const& vlnv, General::InterfaceMode mode)
{
    logicalPortNames_.clear();

    if (vlnv.isValid())
    {
        QSharedPointer<Document> libComb = libraryHandler_->getModel(vlnv);
        if (libComb && libraryHandler_->getDocumentType(vlnv) == VLNV::ABSTRACTIONDEFINITION)
        {
            QSharedPointer<AbstractionDefinition> abstractionDefinition
                = libComb.dynamicCast<AbstractionDefinition>();

            if (abstractionDefinition)
            {
                foreach(QSharedPointer<PortAbstraction> logicalPort, *abstractionDefinition->getLogicalPorts())
                {
                    if (logicalPort->hasMode(mode))
                    {
                        logicalPortNames_.append(logicalPort->name());
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortMapTreeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT && index.parent().isValid())
    {
        QComboBox* logicalSelector = new QComboBox(parent);

        logicalSelector->addItems(logicalPortNames_);

        return logicalSelector;
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        QComboBox* portSelector = new QComboBox(parent);

        portSelector->addItem("");
        portSelector->addItems(physicalPortNames_);

        setPortDirectionIcons(portSelector);

        return portSelector;
    }

    return ExpressionDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::setPortDirectionIcons()
//-----------------------------------------------------------------------------
void PortMapTreeDelegate::setPortDirectionIcons(QComboBox* portSelector) const
{
    for (int portIndex = 0; portIndex < physicalPortNames_.size(); ++portIndex)
    {
        QString portName = physicalPortNames_.at(portIndex);
        QString iconPath = ":icons/common/graphics/cross.png";
        QSharedPointer<Port> currentPort = component_->getPort(portName);
        if (currentPort)
        {
            DirectionTypes::Direction portDirection = currentPort->getDirection();
            if (portDirection == DirectionTypes::IN)
            {
                iconPath = ":icons/common/graphics/control-180.png";
            }
            else if (portDirection == DirectionTypes::OUT)
            {
                iconPath = ":icons/common/graphics/control.png";
            }
            else if (portDirection == DirectionTypes::INOUT)
            {
                iconPath = ":icons/common/graphics/control-dual.png";
            }
        }

        QIcon directionIcon(iconPath);
        int itemIndex = portSelector->findText(portName);

        portSelector->setItemIcon(itemIndex, directionIcon);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortMapTreeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT && index.parent().isValid())
    {
        QModelIndex parentIndex = index.parent();
        if (parentIndex.column() != PortMapsColumns::LOGICAL_PORT)
        {
            parentIndex = parentIndex.sibling(parentIndex.row(), PortMapsColumns::LOGICAL_PORT);
        }
        QString text = index.model()->data(parentIndex, Qt::DisplayRole).toString();

        QComboBox* logicalSelector = qobject_cast<QComboBox*>(editor);

        int logicalIndex = logicalSelector->findText(text);
        logicalSelector->setCurrentIndex(logicalIndex);
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();

        QComboBox* portSelector = qobject_cast<QComboBox*>(editor);

        int portIndex = portSelector->findText(text);
        portSelector->setCurrentIndex(portIndex);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortMapTreeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT)
    {
        QComboBox* logicalSelector = qobject_cast<QComboBox*>(editor);

        QString text = logicalSelector->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        QComboBox* portSelector = qobject_cast<QComboBox*>(editor);

        QString text = portSelector->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::paint()
//-----------------------------------------------------------------------------
void PortMapTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
    const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.parent().isValid() && index.column() == PortMapsColumns::INVERT)
    {
        QVariant colourVariant = index.data(Qt::BackgroundRole);
        QColor backgroundColour = colourVariant.value<QColor>();

        painter->fillRect(option.rect, backgroundColour);

        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
            QRect(option.rect.x() + textMargin, option.rect.y(),
            option.rect.width() - (2 * textMargin), option.rect.height()));

        viewItemOption.rect = newRect;
    }

    QStyledItemDelegate::paint(painter, viewItemOption, index);

    QPen oldPen = painter->pen();
    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);

    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

    newPen.setWidth(2);
    painter->setPen(newPen);
    
    if (!index.parent().isValid())
    {
        painter->drawLine(option.rect.topLeft(), option.rect.topRight());
    }

    if (index.column() == PortMapsColumns::LOGICAL_PRESENCE || index.column() == PortMapsColumns::INVERT)
    {
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    }

    painter->setPen(oldPen);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool PortMapTreeDelegate::columnAcceptsExpression(int column) const
{
    return column == PortMapsColumns::LOGICAL_LEFT || column == PortMapsColumns::LOGICAL_RIGHT ||
        column == PortMapsColumns::PHYSICAL_LEFT || column == PortMapsColumns::PHYSICAL_RIGHT ||
        column == PortMapsColumns::TIEOFF;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int PortMapTreeDelegate::descriptionColumn() const
{
    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void PortMapTreeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    ExpressionDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        QComboBox* portSelector = dynamic_cast<QComboBox*>(editor);
        if (portSelector)
        {
            int optionWidth = option.rect.width();

            portSelector->move(option.rect.topLeft());
            portSelector->setMinimumWidth(optionWidth);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool PortMapTreeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
    const QModelIndex &index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Always reset the ad-hoc group modify flag.
    if (event->type() == QEvent::MouseButtonRelease)
    {
        invertModify_ = false;
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
        invertModify_ = true;
        invertCheckState_ = newState;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!invertModify_ || static_cast<Qt::CheckState>(value.toInt()) == invertCheckState_)
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

        newState = invertCheckState_;
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
