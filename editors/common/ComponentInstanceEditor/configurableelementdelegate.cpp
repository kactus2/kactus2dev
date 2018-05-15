//-----------------------------------------------------------------------------
// File: configurableelementdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Delegate that provides editors to edit the configurable element values.
//-----------------------------------------------------------------------------

#include "configurableelementdelegate.h"

#include <common/KactusColors.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/parameters/Array/ArrayView.h>
#include <editors/ComponentEditor/parameters/Array/ParameterArrayModel.h>
#include <editors/ComponentEditor/parameters/Array/ArrayDelegate.h>

#include <editors/common/ComponentInstanceEditor/ConfigurableElementsColumns.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsModel.h>
#include <editors/HWDesign/undoCommands/ConfigurableElementChangeCommand.h>
#include <editors/HWDesign/undoCommands/ConfigurableElementRemoveCommand.h>

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <QPen>
#include <QPainter>
#include <QComboBox>
#include <QScrollArea>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::ConfigurableElemenetDelegate()
//-----------------------------------------------------------------------------
ConfigurableElementDelegate::ConfigurableElementDelegate(QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent):
ChoiceCreatorDelegate(parameterCompleter, parameterFinder, parent),
expressionFormatter_(expressionFormatter),
editProvider_(0)
{

}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::~ConfigurableElementDelegate()
//-----------------------------------------------------------------------------
ConfigurableElementDelegate::~ConfigurableElementDelegate() 
{

}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setEditProvider()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setEditProvider(QSharedPointer<IEditProvider> newEditProvider)
{
    editProvider_ = newEditProvider;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ConfigurableElementDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == valueColumn() && valueIsArray(index))
    {
        ArrayView* editor = new ArrayView(parent);

        QScrollArea* scrollingWidget = new QScrollArea(parent);
        scrollingWidget->setWidgetResizable(true);
        scrollingWidget->setWidget(editor);
        scrollingWidget->parent()->installEventFilter(editor);

        return scrollingWidget;
    }

    else
    {
        return ChoiceCreatorDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setEditorData()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == valueColumn() && valueIsArray(index))
    {
        createArrayEditor(editor, index);
    }

    else
    {
        ChoiceCreatorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setModelData()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    QModelIndex const& index) const
{
    QString oldData = model->data(index, Qt::EditRole).toString();

    if (index.column() == valueColumn() && valueIsArray(index))
    {
        QScrollArea* scrollWidget = qobject_cast<QScrollArea*>(editor);
        ArrayView* arrayTable = qobject_cast<ArrayView*>(scrollWidget->widget());
        ParameterArrayModel* arrayModel = qobject_cast<ParameterArrayModel*>(arrayTable->model());

        QString arrayValue = arrayModel->getArrayData();
        model->setData(index, arrayValue, Qt::EditRole);
    }

    else
    {
        ChoiceCreatorDelegate::setModelData(editor, model, index);
    }

    QString newData = model->data(index, Qt::EditRole).toString();
    if (oldData != newData)
    {
        createElementChangeCommand(oldData, newData, index, model);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::createElementChangeCommand()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::createElementChangeCommand(QString const& oldValue, QString const& newValue,
    QModelIndex const& index, QAbstractItemModel* cevModel) const
{
    QString modifiedNewValue = newValue;
    if (newValue.isEmpty())
    {
        QModelIndex defaultValueIndex = index.sibling(index.row(), ConfigurableElementsColumns::DEFAULT_VALUE);
        modifiedNewValue = defaultValueIndex.data(Qt::EditRole).toString();

        if (modifiedNewValue == oldValue)
        {
            return;
        }
    }

    QVariant configurableElements =
        cevModel->data(index, ConfigurableElementsModel::getItemConfigurableElementValuesRole);

    if (configurableElements.canConvert<QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > > ())
    {
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > itemConfigurableElements =
            configurableElements.value<QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > >();

        QString elementID = cevModel->data(index, ConfigurableElementsModel::parameterIDRole).toString();

        QModelIndex defaultValueIndex = index.sibling(index.row(), ConfigurableElementsColumns::DEFAULT_VALUE);
        QString defaultValue = cevModel->data(defaultValueIndex, Qt::EditRole).toString();

        QSharedPointer<ConfigurableElementChangeCommand> elementChangeCommand(new ConfigurableElementChangeCommand(
            elementID, oldValue, modifiedNewValue, defaultValue, itemConfigurableElements));

        connect(elementChangeCommand.data(), SIGNAL(dataChangedInID(QString const&, QString const&)),
            this, SIGNAL(dataChangedInID(QString const&, QString const&)), Qt::UniqueConnection);

        if (newValue.isEmpty())
        {
            connect(elementChangeCommand.data(), SIGNAL(increaseReferencesInNewValue(QString const&)),
                this, SLOT(increaseReferencesInNewValue(QString const&)), Qt::UniqueConnection);
            connect(elementChangeCommand.data(), SIGNAL(decreaseReferencesInOldValue(QString const&)),
                this, SLOT(decreaseReferencesInOldValue(QString const&)), Qt::UniqueConnection);
        }

        addCommandToStackAndRedo(elementChangeCommand);

        if (!newValue.isEmpty())
        {
            connect(elementChangeCommand.data(), SIGNAL(increaseReferencesInNewValue(QString const&)),
                this, SLOT(increaseReferencesInNewValue(QString const&)), Qt::UniqueConnection);
            connect(elementChangeCommand.data(), SIGNAL(decreaseReferencesInOldValue(QString const&)),
                this, SLOT(decreaseReferencesInOldValue(QString const&)), Qt::UniqueConnection);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::onCreateRemoveElementCommands()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::onCreateRemoveElementCommands(QModelIndexList const& indexes)
{
    if (!indexes.isEmpty())
    {
        QSharedPointer<QUndoCommand> parentCommand(new QUndoCommand());

        foreach (QModelIndex index, indexes)
        {
            int indexRowCount = index.model()->rowCount(index);
            if (index.parent().isValid() || (!index.parent().isValid() && indexRowCount == 0))
            {
                ConfigurableElementRemoveCommand* elementRemoveCommand =
                    createElementRemoveCommand(index, parentCommand);

                connect(elementRemoveCommand, SIGNAL(increaseReferencesInNewValue(QString const&)),
                    this, SLOT(increaseReferencesInNewValue(QString const&)), Qt::UniqueConnection);
                connect(elementRemoveCommand, SIGNAL(decreaseReferencesInOldValue(QString const&)),
                    this, SLOT(decreaseReferencesInOldValue(QString const&)), Qt::UniqueConnection);
            }
        }

        addCommandToStackAndRedo(parentCommand);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::createElementRemoveCommand()
//-----------------------------------------------------------------------------
ConfigurableElementRemoveCommand* ConfigurableElementDelegate::createElementRemoveCommand(
    QModelIndex const& index, QSharedPointer<QUndoCommand> parentCommand)
{
    int filteredRow = getFilteredIndexRow(index);
    QString parentName = getIndexedParentName(index.parent());

    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > itemConfigurableElements =
        getIndexedConfigurableElements(index);

    QString elementID = index.data(ConfigurableElementsModel::parameterIDRole).toString();

    ConfigurableElementRemoveCommand* elementRemoveCommand(new ConfigurableElementRemoveCommand(
        elementID, filteredRow, parentName, itemConfigurableElements, parentCommand.data()));

    connectElementRemoveCommand(elementRemoveCommand);

    return elementRemoveCommand;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::getFilteredIndexRow()
//-----------------------------------------------------------------------------
int ConfigurableElementDelegate::getFilteredIndexRow(QModelIndex const& index) const
{
    QModelIndex modelIndex = index;

    const QSortFilterProxyModel* filter = dynamic_cast<const QSortFilterProxyModel*>(index.model());
    if (filter)
    {
        modelIndex = filter->mapToSource(index);
    }

    return modelIndex.row();
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::getIndexedParentName()
//-----------------------------------------------------------------------------
QString ConfigurableElementDelegate::getIndexedParentName(QModelIndex const& parentIndex) const
{
    QModelIndex parentNameIndex = parentIndex.sibling(parentIndex.row(), ConfigurableElementsColumns::NAME);
    return parentNameIndex.data().toString();
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::getIndexedConfigurableElements()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > ConfigurableElementDelegate::
    getIndexedConfigurableElements(QModelIndex const& index) const
{
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > itemConfigurableElements(0);

    QVariant configurableElements = index.data(ConfigurableElementsModel::getItemConfigurableElementValuesRole);
    if (configurableElements.canConvert<QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > > ())
    {
        itemConfigurableElements = configurableElements.value
            <QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > > ();
    }

    return itemConfigurableElements;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::connectElementRemoveCommand()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::connectElementRemoveCommand(ConfigurableElementRemoveCommand* removeCommand)
{
    connect(removeCommand, SIGNAL(addConfigurableElement(QString const&, QString const&, QString const&, int)),
        this, SIGNAL(addConfigurableElement(QString const&, QString const&, QString const&, int)),
        Qt::UniqueConnection);
    connect(removeCommand, SIGNAL(removeConfigurableElement(QString const&, QString const&, int)),
        this, SIGNAL(removeConfigurableElement(QString const&, QString const&, int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::onCreateMultipleElementRemoveCommands()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::onCreateMultipleElementRemoveCommands(QModelIndexList const& indexes)
{
    if (indexes.isEmpty())
    {
        return;
    }

    QSharedPointer<QUndoCommand> mainRemoveCommand(new QUndoCommand());

    foreach (QModelIndex index, indexes)
    {
        if (index.isValid() && !index.parent().isValid())
        {
            QUndoCommand* multipleRemoveCommand(new QUndoCommand(mainRemoveCommand.data()));
            int rowCount = index.model()->rowCount(index);

            for (int i = rowCount - 1; i >= 0; i = i - 1)
            {
                QModelIndex elementIndex = index.child(i, index.column());

                if (elementIndex.data(ConfigurableElementsModel::deletableElementCheckRole).toBool())
                {
                    int filteredRow = getFilteredIndexRow(elementIndex);
                    QString parentName = getIndexedParentName(elementIndex.parent());

                    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > itemConfigurableElements =
                        getIndexedConfigurableElements(elementIndex);

                    QString elementID = elementIndex.data(ConfigurableElementsModel::parameterIDRole).toString();

                    ConfigurableElementRemoveCommand* elementRemoveCommand(new ConfigurableElementRemoveCommand(
                        elementID, filteredRow, parentName, itemConfigurableElements, multipleRemoveCommand));

                    connect(elementRemoveCommand, SIGNAL(addConfigurableElement(QString const&, QString const&,
                        QString const&, int)), this, SIGNAL(addConfigurableElement(QString const&, QString const&,
                        QString const&, int)), Qt::UniqueConnection);
                    connect(elementRemoveCommand, SIGNAL(removeConfigurableElement(QString const&, QString const&,
                        int)), this, SIGNAL(removeConfigurableElement(QString const&, QString const&, int)),
                        Qt::UniqueConnection);
                }
            }
        }
    }

    addCommandToStackAndRedo(mainRemoveCommand);

    for (int i = 0; i < mainRemoveCommand->childCount(); ++i)
    {
        const QUndoCommand* multipleRemoveCommand = mainRemoveCommand->child(i);
        for (int j = 0; j < multipleRemoveCommand->childCount(); ++j)
        {
            const ConfigurableElementRemoveCommand* removeCommand =
                dynamic_cast<const ConfigurableElementRemoveCommand*>(multipleRemoveCommand->child(i));
            if (removeCommand)
            {
                connect(removeCommand, SIGNAL(increaseReferencesInNewValue(QString const&)),
                    this, SLOT(increaseReferencesInNewValue(QString const&)), Qt::UniqueConnection);
                connect(removeCommand, SIGNAL(decreaseReferencesInOldValue(QString const&)),
                    this, SLOT(decreaseReferencesInOldValue(QString const&)), Qt::UniqueConnection);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::addCommandToStackAndRedo()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::addCommandToStackAndRedo(QSharedPointer<QUndoCommand> newCommand) const
{
    if (editProvider_)
    {
        editProvider_->addCommand(newCommand);
    }

    newCommand->redo();
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::increaseReferencesInNewValue()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::increaseReferencesInNewValue(QString const& newValue)
{
    QStringList allParameterIDs = getParameterFinder()->getAllParameterIds();
    foreach (QString valueID, allParameterIDs)
    {
        int referencesToId = newValue.count(valueID);
        for (int i =  0; i < referencesToId; ++i)
        {
            emit increaseReferences(valueID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::decreaseReferencesInOldValue()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::decreaseReferencesInOldValue(QString const& oldValue)
{
    QStringList allParameterIDs = getParameterFinder()->getAllParameterIds();
    foreach (QString valueID, allParameterIDs)
    {
        int referencesToId = oldValue.count(valueID);
        for (int i =  0; i < referencesToId; ++i)
        {
            emit decreaseReferences(valueID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setChoices()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
    ChoiceCreatorDelegate::setChoices(choices);
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ConfigurableElementDelegate::columnAcceptsExpression(int column) const
{
    return column == ConfigurableElementsColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::choiceNameOnRow()
//-----------------------------------------------------------------------------
QString ConfigurableElementDelegate::choiceNameOnRow(QModelIndex const& index) const
{
    return index.sibling(index.row(), ConfigurableElementsColumns::CHOICE).data(Qt::DisplayRole).toString();
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::valueColumn()
//-----------------------------------------------------------------------------
int ConfigurableElementDelegate::valueColumn() const
{
    return ConfigurableElementsColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ConfigurableElementDelegate::descriptionColumn() const
{
    return -1;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::valueIsArray()
//-----------------------------------------------------------------------------
bool ConfigurableElementDelegate::valueIsArray(QModelIndex const& index) const
{
    QModelIndex arrayLeftIndex = index.sibling(index.row(), ConfigurableElementsColumns::ARRAY_LEFT);
    bool arrayLeftIsOk = true;
    int arrayLeft = arrayLeftIndex.data(Qt::DisplayRole).toInt(&arrayLeftIsOk);

    QModelIndex arrayRightIndex = index.sibling(index.row(), ConfigurableElementsColumns::ARRAY_RIGHT);
    bool arrayRightIsOk = true;
    int arrayRight = arrayRightIndex.data(Qt::DisplayRole).toInt(&arrayRightIsOk);

    int arraySize = getArraySize(arrayLeft, arrayRight);

    return arrayLeftIsOk && arrayRightIsOk && arraySize > 0;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    ExpressionDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == valueColumn() && valueIsArray(index))
    {
        repositionAndResizeEditor(editor, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    QModelIndex arrayLeftIndex = index.sibling(index.row(), ConfigurableElementsColumns::ARRAY_LEFT);
    int arrayLeft = arrayLeftIndex.data(Qt::DisplayRole).toInt();
    QModelIndex arrayRightIndex = index.sibling(index.row(), ConfigurableElementsColumns::ARRAY_RIGHT);
    int arrayRight = arrayRightIndex.data(Qt::DisplayRole).toInt();

    int arraySize = getArraySize(arrayLeft, arrayRight);
    int editorMinimumSize = 24 * (arraySize + 1);

    QPoint nameColumnTopRight = option.rect.topLeft();
    editor->setFixedWidth(editor->parentWidget()->width() - nameColumnTopRight.x());

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT - editorMinimumSize;
        if (editorNewY < 0)
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
// Function: configurableelementdelegate::createArrayEditor()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::createArrayEditor(QWidget* editor, QModelIndex const& index) const
{
    ArrayView* view = dynamic_cast<ArrayView*>(dynamic_cast<QScrollArea*>(editor)->widget());

    QModelIndex arrayLeftIndex = index.sibling(index.row(), ConfigurableElementsColumns::ARRAY_LEFT);
    int arrayLeft = arrayLeftIndex.data(Qt::DisplayRole).toInt();
    QModelIndex arrayRightIndex = index.sibling(index.row(), ConfigurableElementsColumns::ARRAY_RIGHT);
    int arrayRight = arrayRightIndex.data(Qt::DisplayRole).toInt();

    int arraySize = getArraySize(arrayLeft, arrayRight);

    int arrayStartIndex = arrayLeft;
    if (arrayRight < arrayLeft)
    {
        arrayStartIndex = arrayRight;
    }

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(getParameterFinder()));
    QSharedPointer<Choice> selectedChoice = findChoice(index);
    ParameterArrayModel* model = new ParameterArrayModel(arraySize, expressionParser, getParameterFinder(),
        expressionFormatter_, selectedChoice, KactusColors::MANDATORY_FIELD, arrayStartIndex, view);
    
    QModelIndex valueIndex = index.sibling(index.row(), valueColumn());
    QString parameterValue = valueIndex.data(Qt::EditRole).toString();
    model->setArrayData(parameterValue);

    view->setItemDelegate(new ArrayDelegate(getNameCompleter(), getParameterFinder(), selectedChoice, this->parent()));

    QModelIndex typeIndex = index.sibling(index.row(), ConfigurableElementsColumns::TYPE);
    QString parameterType = typeIndex.data(Qt::DisplayRole).toString();
    model->setParameterType(parameterType);

    view->setItemDelegate(new ArrayDelegate(getNameCompleter(), getParameterFinder(), selectedChoice,
        this->parent()));
    view->setModel(model);
    view->setSortingEnabled(false);
    view->resizeColumnsToContents();

    connect(view->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::getArraySize()
//-----------------------------------------------------------------------------
int ConfigurableElementDelegate::getArraySize(int arrayLeft, int arrayRight) const
{
    return abs(arrayLeft - arrayRight) + 1;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::paint()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    int indexRowCount = index.model()->rowCount(index);
    if (!index.parent().isValid() && indexRowCount > 0)
    {
        painter->fillRect(option.rect, KactusColors::STRONG_FIELD);
    }

	QStyleOptionViewItem overrideOptions(option);
    overrideOptions.decorationPosition = QStyleOptionViewItem::Right;

    ChoiceCreatorDelegate::paint(painter, overrideOptions, index);
    
    QPen oldPen = painter->pen();
    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);
    
    if (index.parent().isValid() || (!index.parent().isValid() && indexRowCount == 0))
    {
        painter->drawLine(overrideOptions.rect.topRight(), overrideOptions.rect.bottomRight());
    }

    painter->setPen(oldPen);
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize ConfigurableElementDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize givenSize = ChoiceCreatorDelegate::sizeHint(option, index);

    QSize newSize (givenSize.width(), 20);
    return newSize;
}
