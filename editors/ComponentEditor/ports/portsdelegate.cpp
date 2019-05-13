//-----------------------------------------------------------------------------
// File: portsdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 01.04.2011
//
// Description:
// The delegate that provides editors to edit ports of a component.
//-----------------------------------------------------------------------------

#include "portsdelegate.h"

#include <common/widgets/listManager/listeditor.h>

#include <editors/ComponentEditor/ports/PortTagEditorDelegate.h>
#include <editors/ComponentEditor/ports/PortWireTypeEditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: PortsDelegate::PortsDelegate()
//-----------------------------------------------------------------------------
PortsDelegate::PortsDelegate(QSharedPointer<Component> component, QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<PortTypeValidator> typeValidator,
    QObject* parent):
ExpressionDelegate(parameterCompleter, parameterFinder, parent),
component_(component),
adhocGroupModify_(false),
adhocGroupState_(Qt::Unchecked),
typeValidator_(typeValidator)
{

}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == typeColumn())
    {
        return createTypeEditor(parent);
    }
    else if (index.column() == tagColumn())
    {
        return createListEditorForPortTags(index, parent);
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: portsdelegate::createTypeEditor()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createTypeEditor(QWidget* parent) const
{
    PortWireTypeEditor* editor = new PortWireTypeEditor(component_, typeValidator_, parent);

    connect(editor, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    return editor;
}

//-----------------------------------------------------------------------------
// Function: PortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == typeColumn())
    {
        PortWireTypeEditor* typeEditor = dynamic_cast<PortWireTypeEditor*>(editor);
        if (typeEditor)
        {
            QModelIndex portNameIndex = index.sibling(index.row(), nameColumn());
            QString portName = portNameIndex.data(Qt::DisplayRole).toString();
            QSharedPointer<Port> currentPort = component_->getPort(portName);

            typeEditor->setPortForModel(currentPort);
        }
    }
    else if (index.column() == tagColumn())
    {
        ListEditor* tagEditor = qobject_cast<ListEditor*>(editor);
        Q_ASSERT(tagEditor);

        QString portTagGroup = index.model()->data(index, Qt::DisplayRole).toString();

        if (!portTagGroup.isEmpty())
        {
            QStringList portTags = portTagGroup.split(", ");
            tagEditor->setItems(portTags);
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
    if (index.column() == typeColumn())
    {
        return;
    }
    else if (index.column() == tagColumn())
    {
        ListEditor* tagEditor = qobject_cast<ListEditor*>(editor);
        Q_ASSERT(tagEditor);

        QStringList tagGroup = tagEditor->items();
        tagGroup.removeDuplicates();
        tagGroup.removeAll("");

        QString portTags = tagGroup.join(", ");

        model->setData(index, portTags);
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
	QStyleOptionViewItem viewItemOption(option);

    if (index.column() == adHocColumn())
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
    return column == arrayLeftColumn() || column == arrayRightColumn();
}

//-----------------------------------------------------------------------------
// Function: portsdelegate::createListEditorForPortTags()
//-----------------------------------------------------------------------------
QWidget* PortsDelegate::createListEditorForPortTags(const QModelIndex& currentIndex, QWidget* parent) const
{
    ListEditor* tagEditor = new ListEditor(parent);
    tagEditor->setMinimumHeight(100);

    QStringList existingTags;

    int portCount = currentIndex.model()->rowCount();
    for (int i = 0; i < portCount; ++i)
    {
        QModelIndex portIndex = currentIndex.sibling(i, tagColumn());
        QString portTags = portIndex.data(Qt::ToolTipRole).toString();

        if (!portTags.isEmpty())
        {
            QStringList newTags = portTags.split(", ");
            existingTags.append(newTags);
        }
    }
    existingTags.removeDuplicates();

    tagEditor->setItemDelegate(new PortTagEditorDelegate(existingTags, parent));
    return tagEditor;
}

//-----------------------------------------------------------------------------
// Function: portsdelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void PortsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    ExpressionDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == typeColumn())
    {
        repositionAndResizeEditor(editor, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: portsdelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void PortsDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    int editorMinimumSize = 30 * (getRowCountForPortTypes(index) + 5);
    editor->setFixedWidth(300);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT-editorMinimumSize;
        if (editorNewY <= 0)
        {
            editorNewY = 0;
        }

        editor->move(option.rect.left(), editorNewY);
    }

    if (editorMinimumSize > PARENT_HEIGHT)
    {
        editor->setFixedHeight(PARENT_HEIGHT);
    }
    else
    {
        editor->setFixedHeight(editorMinimumSize);
    }
}

//-----------------------------------------------------------------------------
// Function: portsdelegate::getRowCountForPortTypes()
//-----------------------------------------------------------------------------
int PortsDelegate::getRowCountForPortTypes(QModelIndex const& index) const
{
    QModelIndex portNameIndex = index.sibling(index.row(), nameColumn());
    QString portName = portNameIndex.data(Qt::DisplayRole).toString();
    QSharedPointer<Port> currentPort = component_->getPort(portName);

    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > typeDefinitions;
    if (currentPort->getWire() && currentPort->getWire()->getWireTypeDefs())
    {
        typeDefinitions = currentPort->getWire()->getWireTypeDefs();
    }
    else if (currentPort->getTransactional() && currentPort->getTransactional()->getTransTypeDef())
    {
        typeDefinitions = currentPort->getTransactional()->getTransTypeDef();
    }

    int portTypeRowCount = 0;
    foreach (QSharedPointer<WireTypeDef> singleTypeDefinition, *typeDefinitions)
    {
        portTypeRowCount += singleTypeDefinition->getViewRefs()->count() + 1;
    }

    return portTypeRowCount;
}

//-----------------------------------------------------------------------------
// Function: portsdelegate::setComponent()
//-----------------------------------------------------------------------------
void PortsDelegate::setComponent(QSharedPointer<Component> newComponent)
{
    component_ = newComponent;
}
