//-----------------------------------------------------------------------------
// File: PortWireTypeDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// The delegate that provides editors to edit port wire type definition.
//-----------------------------------------------------------------------------

#include "PortWireTypeDelegate.h"

#include <editors/ComponentEditor/ports/PortWireTypeColumns.h>
#include <editors/ComponentEditor/ports/PortTypeDefinitionDelegate.h>
#include <editors/ComponentEditor/ports/PortTypeDefinitionEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <QLineEdit>
#include <QCompleter>
#include <QComboBox>
#include <QPen>
#include <QPainter>

namespace PortWireTypeConstants
{
    QString const DEFINITIONSEPARATOR = "; ";
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::PortWireTypeDelegate()
//-----------------------------------------------------------------------------
PortWireTypeDelegate::PortWireTypeDelegate(QSharedPointer<Component> component, QObject* parent):
QStyledItemDelegate(parent),
availableViews_(component->getViews()),
containedModel_()
{

}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::~PortWireTypeDelegate()
//-----------------------------------------------------------------------------
PortWireTypeDelegate::~PortWireTypeDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::setNewModel()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::setNewModel(QAbstractItemModel* newModel)
{
    containedModel_ = newModel;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortWireTypeDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.isValid())
    {
        if (index.column() == PortWireTypeColumns::TYPE)
        {
            if (!index.parent().isValid())
            {
                return createSelectorWithCommonTypes(parent);
            }
            else
            {
                return createViewSelector(parent);
            }
        }
        else if (!index.parent().isValid() && index.column() == PortWireTypeColumns::TYPE_DEFINITIONS)
        {
            return createDefinitionEditor(parent, index);
        }
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == PortWireTypeColumns::TYPE_DEFINITIONS)
    {
        ListEditor* definitionEditor = qobject_cast<ListEditor*>(editor);
        if (definitionEditor)
        {
            QString combinedDefinition = index.data(Qt::DisplayRole).toString();
            QStringList definitions;
            if (!combinedDefinition.isEmpty())
            {
                definitions = combinedDefinition.split(PortWireTypeConstants::DEFINITIONSEPARATOR);
            }
            definitionEditor->setItems(definitions);
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index )
    const
{
    if (index.column() == PortWireTypeColumns::TYPE_DEFINITIONS)
    {
        ListEditor* definitionEditor = qobject_cast<ListEditor*>(editor);

        QStringList definitionList = definitionEditor->items();
        model->setData(index, definitionList.join(PortWireTypeConstants::DEFINITIONSEPARATOR), Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::createSelectorWithCommonTypes()
//-----------------------------------------------------------------------------
QWidget* PortWireTypeDelegate::createSelectorWithCommonTypes(QWidget* parent) const
{
    QLineEdit* typeEditor = new QLineEdit(parent);

    QStringList types;
    types.append("bit");
    types.append("bit_vector");
    types.append("boolean");
    types.append("character");
    types.append("integer");
    types.append("natural");
    types.append("positive");
    types.append("real");
    types.append("signed");
    types.append("std_logic");
    types.append("std_logic_vector");
    types.append("std_ulogic");
    types.append("std_ulogic_vector");
    types.append("string");
    types.append("time");
    types.append("unsigned");
    types.append("reg");
    types.append("wire");

    types.sort(Qt::CaseInsensitive);

    QCompleter* typeCompleter = new QCompleter(types, parent);
    typeCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    typeEditor->setCompleter(typeCompleter);

    return typeEditor;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::createViewSelector()
//-----------------------------------------------------------------------------
QWidget* PortWireTypeDelegate::createViewSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    combo->addItem(QString(""));

    foreach (QSharedPointer<View> view, *availableViews_)
    {
        combo->addItem(view->name());
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::createDefinitionEditor()
//-----------------------------------------------------------------------------
QWidget* PortWireTypeDelegate::createDefinitionEditor(QWidget* parent, QModelIndex const& index) const
{
    PortTypeDefinitionEditor* definitionEditor = new PortTypeDefinitionEditor(index, parent);

    connect(definitionEditor, SIGNAL(closingEditor(QModelIndex const&)),
        this, SLOT(onDefinitionEditorClosed(QModelIndex const&)), Qt::UniqueConnection);
    connect(definitionEditor, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    return definitionEditor;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::onDefinitionEditorClosed()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::onDefinitionEditorClosed(QModelIndex const& editorIndex)
{
    PortTypeDefinitionEditor* definitionEditor = qobject_cast<PortTypeDefinitionEditor*>(sender());
    if (definitionEditor)
    {
        QStringList currentDefinitions = definitionEditor->items();

        setModelData(definitionEditor, containedModel_, editorIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::setComponent()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::setComponent(QSharedPointer<Component> newComponent)
{
    availableViews_ = newComponent->getViews();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize PortWireTypeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize givenSize = QStyledItemDelegate::sizeHint(option, index);

    QSize newSize (givenSize.width(), 20);
    return newSize;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::paint()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
    const
{
    QStyledItemDelegate::paint(painter, option, index);

    QPen oldPen = painter->pen();
    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);

    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

    painter->setPen(oldPen);
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    if (!index.parent().isValid() && index.column() == PortWireTypeColumns::TYPE_DEFINITIONS)
    {
        repositionAndResizeEditor(editor, option);
    }
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeDelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void PortWireTypeDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const
{
    int editorMinimumSize = 120;

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
