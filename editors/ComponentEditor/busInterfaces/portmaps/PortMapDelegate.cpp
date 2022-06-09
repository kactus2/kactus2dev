//-----------------------------------------------------------------------------
// File: PortMapDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.06.2022
//
// Description:
// Delegate to provide editors to edit bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapDelegate.h"


#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/PortMapInterface.h>
#include <KactusAPI/include/PortsInterface.h>

#include <QApplication>
#include <QMouseEvent>

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::PortMapDelegate()
//-----------------------------------------------------------------------------
PortMapDelegate::PortMapDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> finder,
    PortMapInterface* portMapInterface, QObject *parent):
ExpressionDelegate(parameterCompleter, finder, parent),
invertModify_(false),
invertCheckState_(Qt::Unchecked),
portMapInterface_(portMapInterface),
busMode_(General::INTERFACE_MODE_COUNT),
systemGroup_("")
{

}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::setBusMode()
//-----------------------------------------------------------------------------
void PortMapDelegate::setBusMode(General::InterfaceMode newMode)
{
    busMode_ = newMode;
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::setSystemGroup()
//-----------------------------------------------------------------------------
void PortMapDelegate::setSystemGroup(QString const& newSystemGroup)
{
    systemGroup_ = newSystemGroup;
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortMapDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT)
    {
        QComboBox* logicalSelector = new QComboBox(parent);
        logicalSelector->addItem("");

        PortAbstractionInterface* logicalInterface = portMapInterface_->getLogicalPortInterface();
        std::string busModeString = General::interfaceMode2Str(busMode_).toStdString();
        std::string systemGroupString = systemGroup_.toStdString();

        QStringList logicalPortNames;

        for (auto portName : logicalInterface->getItemNamesWithModeAndGroup(busModeString, systemGroupString))
        {
            logicalPortNames.append(QString::fromStdString(portName));
        }

        logicalSelector->addItems(logicalPortNames);

        setLogicalPortDirectionIcons(logicalPortNames, logicalSelector);

        return logicalSelector;
    }
    else if (index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        QComboBox* portSelector = new QComboBox(parent);
        portSelector->addItem("");

        QStringList physicalPortNames;
        for (auto portName : portMapInterface_->getPhysicalPortInterface()->getItemNames())
        {
            physicalPortNames.append(QString::fromStdString(portName));
        }

        portSelector->addItems(physicalPortNames);

        setPhysicalPortDirectionIcons(physicalPortNames, portSelector);

        return portSelector;
    }

    return ExpressionDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::setPhysicalPortDirectionIcons()
//-----------------------------------------------------------------------------
void PortMapDelegate::setPhysicalPortDirectionIcons(QStringList const& physicalPortNames, QComboBox* portSelector)
const
{
    for (auto portName : physicalPortNames)
    {
        QString iconPath(QString::fromStdString(
            portMapInterface_->getPhysicalPortInterface()->getIconPathForPort(portName.toStdString())));

        QIcon directionIcon(iconPath);
        int itemIndex = portSelector->findText(portName);

        portSelector->setItemIcon(itemIndex, directionIcon);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::setLogicalPortDirectionIcons()
//-----------------------------------------------------------------------------
void PortMapDelegate::setLogicalPortDirectionIcons(QStringList const& physicalPortNames, QComboBox* portSelector)
const
{
    PortAbstractionInterface* portInterface = portMapInterface_->getLogicalPortInterface();

    std::string busModeString = General::interfaceMode2Str(busMode_).toStdString();
    std::string systemGroup = systemGroup_.toStdString();

    for (auto portName : physicalPortNames)
    {
        QString iconPath(QString::fromStdString(portInterface->getIconPathForSignal(
            portName.toStdString(), busModeString, systemGroup)));

        QIcon directionIcon(iconPath);
        int itemIndex = portSelector->findText(portName);

        portSelector->setItemIcon(itemIndex, directionIcon);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortMapDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT || index.column() == PortMapsColumns::PHYSICAL_PORT)
    {
        QString text = index.data(Qt::DisplayRole).toString();

        QComboBox* portSelector = qobject_cast<QComboBox*>(editor);
        if (portSelector)
        {
            int portIndex = portSelector->findText(text);
            portSelector->setCurrentIndex(portIndex);
        }
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortMapDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == PortMapsColumns::LOGICAL_PORT || index.column() == PortMapsColumns::PHYSICAL_PORT)
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
// Function: PortMapDelegate::paint()
//-----------------------------------------------------------------------------
void PortMapDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem viewItemOption(option);

    if (index.column() == PortMapsColumns::INVERT || index.column() == PortMapsColumns::ISINFORMATIVE)
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
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool PortMapDelegate::columnAcceptsExpression(int column) const
{
    return column == PortMapsColumns::LOGICAL_LEFT || column == PortMapsColumns::LOGICAL_RIGHT ||
        column == PortMapsColumns::PHYSICAL_LEFT || column == PortMapsColumns::PHYSICAL_RIGHT ||
        column == PortMapsColumns::TIEOFF;
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int PortMapDelegate::descriptionColumn() const
{
    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortMapDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void PortMapDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
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
// Function: PortMapDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool PortMapDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
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
