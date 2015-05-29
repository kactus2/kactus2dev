//-----------------------------------------------------------------------------
// File: SaveHierarchyDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 22.05.2015
//
// Description:
// Delegate class for save hierarchy document selection.
//-----------------------------------------------------------------------------

#include "SaveHierarchyDelegate.h"
#include "HierarchicalSaveColumns.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QPainter>

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDelegate::SaveHierarchyDelegate()
//-----------------------------------------------------------------------------
SaveHierarchyDelegate::SaveHierarchyDelegate(LibraryInterface* library, QObject* parent): 
QStyledItemDelegate(parent), library_(library)
{
    
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDelegate::~SaveHierarchyDelegate()
//-----------------------------------------------------------------------------
SaveHierarchyDelegate::~SaveHierarchyDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* SaveHierarchyDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == HierarchicalSaveColumns::SAVE_AS_VLNV)
    {
        VLNV originalVLNV(VLNV::COMPONENT, index.sibling(index.row(), 
            HierarchicalSaveColumns::VLNV).data().toString());
        VLNV::IPXactType type = library_->getDocumentType(originalVLNV);

        VLNVEditor* editor = new VLNVEditor(type, library_, parent->window(), parent);
        editor->setTitle(tr("Enter new VLNV"));
        editor->setFlat(false);

        editor->setFixedHeight(editor->sizeHint().height());
        editor->setAutoFillBackground(true);

        QPalette palette = editor->palette();
        palette.setColor(editor->backgroundRole(), QColor("whiteSmoke"));
        editor->setPalette(palette);

        return editor;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDelegate::setEditorData()
//-----------------------------------------------------------------------------
void SaveHierarchyDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() != HierarchicalSaveColumns::SAVE_AS_VLNV)
    {
        return;
    }
    else
    {
        VLNVEditor* vlnvEditor = dynamic_cast<VLNVEditor*>(editor);
        VLNV targetVlnv(VLNV::COMPONENT, index.data().toString());
        VLNV::IPXactType type = library_->getDocumentType(targetVlnv);
        targetVlnv.setType(type);
        vlnvEditor->setVLNV(targetVlnv);
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDelegate::setModelData()
//-----------------------------------------------------------------------------
void SaveHierarchyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() != HierarchicalSaveColumns::SAVE_AS_VLNV)
    {
        return;
    }
    else
    {
        VLNVEditor* vlnvEditor = dynamic_cast<VLNVEditor*>(editor);        

        model->setData(index, vlnvEditor->getVLNV().toString());

        if (!vlnvEditor->getVLNV().isValid() || library_->contains(vlnvEditor->getVLNV()))
        {
            model->setData(index, QColor("red"), Qt::ForegroundRole);
        }
        else
        {
            model->setData(index, QColor("black"), Qt::ForegroundRole);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize SaveHierarchyDelegate::sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 5);
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDelegate::paint()
//-----------------------------------------------------------------------------
void SaveHierarchyDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);

    painter->setPen(newPen);
    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

    painter->restore();
}
