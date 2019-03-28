//-----------------------------------------------------------------------------
// File: VendorExtensionsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.01.2019
//
// Description:
// The delegate to provide editors to add/remove/edit the vendor extensions.
//-----------------------------------------------------------------------------

#include "VendorExtensionsDelegate.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/common/VendorExtensionEditor/VendorExtensionsGeneral.h>
#include <editors/common/VendorExtensionEditor/VendorExtensionColumns.h>
#include <editors/common/VendorExtensionEditor/VendorExtensionAttributesEditor/VendorExtensionAttributesModel.h>

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QScrollArea>
#include <QLineEdit>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::VendorExtensionsDelegate()
//-----------------------------------------------------------------------------
VendorExtensionsDelegate::VendorExtensionsDelegate(QObject* parent):
QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::~VendorExtensionsDelegate()
//-----------------------------------------------------------------------------
VendorExtensionsDelegate::~VendorExtensionsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* VendorExtensionsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == VendorExtensionColumns::NAMESPACE || index.column() == VendorExtensionColumns::NAME ||
        index.column() == VendorExtensionColumns::VALUE || index.column() == VendorExtensionColumns::DESCRIPTION)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        return lineEdit;
    }
    else if (index.column() == VendorExtensionColumns::ATTRIBUTES)
    {
        EditableTableView* editor = new EditableTableView(parent);
        editor->setAlternatingRowColors(false);
        editor->setSelectionBehavior(QAbstractItemView::SelectItems);
        editor->setSelectionMode(QAbstractItemView::ContiguousSelection);
        editor->setSortingEnabled(false);
        editor->setItemsDraggable(false);

        QScrollArea* scrollingWidget = new QScrollArea(parent);
        scrollingWidget->setWidgetResizable(true);
        scrollingWidget->setWidget(editor);
        scrollingWidget->parent()->installEventFilter(editor);

        return scrollingWidget;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == VendorExtensionColumns::NAMESPACE || index.column() == VendorExtensionColumns::NAME ||
        index.column() == VendorExtensionColumns::VALUE || index.column() == VendorExtensionColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else if (index.column() == VendorExtensionColumns::ATTRIBUTES)
    {
        EditableTableView* view = dynamic_cast<EditableTableView*>(dynamic_cast<QScrollArea*>(editor)->widget());

        QVariant genericVariant = index.model()->data(index, VendorExtensionsGeneral::getGenericExtensionRole);
        if (genericVariant.canConvert<GenericVendorExtension*>())
        {
            GenericVendorExtension* genericExtension = genericVariant.value<GenericVendorExtension*>();
            VendorExtensionAttributesModel* attributesModel = new VendorExtensionAttributesModel(genericExtension);

            view->setModel(attributesModel);

            connect(attributesModel, SIGNAL(contentChanged()),
                this, SIGNAL(contentChanged()), Qt::UniqueConnection);
            connect(attributesModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                this, SIGNAL(contentChanged()), Qt::UniqueConnection);

            connect(view, SIGNAL(addItem(const QModelIndex&)),
                attributesModel, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
            connect(view, SIGNAL(removeItem(const QModelIndex&)),
                attributesModel, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::setModelData()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (index.column() == VendorExtensionColumns::NAMESPACE || index.column() == VendorExtensionColumns::NAME ||
        index.column() == VendorExtensionColumns::VALUE || index.column() == VendorExtensionColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == VendorExtensionColumns::ATTRIBUTES)
    {
        EditableTableView* view = dynamic_cast<EditableTableView*>(dynamic_cast<QScrollArea*>(editor)->widget());
        VendorExtensionAttributesModel* attributeModel =
            qobject_cast<VendorExtensionAttributesModel*>(view->model());

        QVector<QPair<QString, QString> > newAttributes = attributeModel->getAttributes();
        QVariant data = QVariant::fromValue(newAttributes);

        model->setData(index, data, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::paint()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QPen oldPen = painter->pen();
    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);

    if (index.column() != VendorExtensionColumns::DESCRIPTION)
    {
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    }

    painter->setPen(oldPen);
}


//-----------------------------------------------------------------------------
// Function: ParameterDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == VendorExtensionColumns::ATTRIBUTES)
    {
        repositionAndResizeEditor(editor, option);
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const
{
    int editorMinimumSize = 400;
    editor->setFixedWidth(180);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT - editorMinimumSize;
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