//-----------------------------------------------------------------------------
// File: ComponentEditorTreeDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.05.2023
//
// Description:
// Component editor navigation tree delegate for custom painting of items.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITOR_TREE_DELEGATE_H 
#define COMPONENTEDITOR_TREE_DELEGATE_H

#include <QItemDelegate>

//-----------------------------------------------------------------------------
//! Component editor navigation tree delegate for custom painting of items.
//-----------------------------------------------------------------------------
class ComponentEditorTreeDelegate : public QItemDelegate
{
    Q_OBJECT

public:

    /*!
     * The constructor.
     *
     *     @param [in] parent  <Description>
     *
     *     @return 
     */
     ComponentEditorTreeDelegate(QObject* parent = nullptr);
    
    //! The destructor.
    ~ComponentEditorTreeDelegate() final = default;
    
    //! No copying.
    ComponentEditorTreeDelegate(const ComponentEditorTreeDelegate &other) = delete;

    //! No assignment.
    ComponentEditorTreeDelegate &operator=(const ComponentEditorTreeDelegate &other) = delete;

    /*!
     * Renders the item indicated by index.
     *
     *     @param [in] painter  The painter to use for rendering.
     *     @param [in] option   Options to guide the rendering.
     *     @param [in] index    The index of the item to render.
     */
     void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

};



#endif // COMPONENTEDITOR_TREE_DELEGATE_H