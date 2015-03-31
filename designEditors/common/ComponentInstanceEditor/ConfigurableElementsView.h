//-----------------------------------------------------------------------------
// File: ConfigurableElementsView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A view for configurable element tree model.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTSVIEW_H
#define CONFIGURABLEELEMENTSVIEW_H

#include <QTreeView>

#include <QContextMenuEvent>
#include <QModelIndex>
#include <QAction>

//-----------------------------------------------------------------------------
//! A tree view for the configurable element tree model.
//-----------------------------------------------------------------------------
class ConfigurableElementsView : public QTreeView
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this view.
     */
    ConfigurableElementsView(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ConfigurableElementsView();

signals:

    /*!
     *  Emitted when the user attempts to remove an item.
     *
     *      @param [in] index   The index of the item to be removed.
     */
    void removeItem(QModelIndex const& index);

protected:
    
    /*!
     *  Draws a row and a separator line under it.
     *
     *      @param [in] painter   The painter used for the drawing.
     *      @param [in] option    Options for the drawing.
     *      @param [in] index     Index of the item to draw.
     */
    virtual void drawRow(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*!
     *  Handler for context menu events.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:
   
    /*!
     *  Called when removeAction is clicked from the context menu.
     */
    virtual void onRemoveItem();

private:
   
    // Disable copying.
    ConfigurableElementsView(ConfigurableElementsView const& rhs);
    ConfigurableElementsView& operator=(ConfigurableElementsView const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The file dependency item context menu was opened on
    QModelIndex contextMenuIndex_;

    //! Action for removing mappings.
    QAction* removeAction_;
};

//-----------------------------------------------------------------------------

#endif // CONFIGURABLEELEMENTSVIEW_H
