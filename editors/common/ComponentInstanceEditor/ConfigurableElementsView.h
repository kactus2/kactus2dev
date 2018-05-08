//-----------------------------------------------------------------------------
// File: ConfigurableElementsView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A view for configurable element tree model.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTSVIEW_H
#define CONFIGURABLEELEMENTSVIEW_H

#include <common/views/EditableTreeView/EditableTreeView.h>

#include <QContextMenuEvent>
#include <QModelIndex>
#include <QAction>

//-----------------------------------------------------------------------------
//! A tree view for the configurable element tree model.
//-----------------------------------------------------------------------------
class ConfigurableElementsView : public EditableTreeView
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
     *  Emitted when a configurable element change command should be created.
     *
     *      @param [in] oldValue    Old value of the indexed element.
     *      @param [in] newValue    New value of the indexed element.
     *      @param [in] index       The selected index.
     *      @param [in] cevModel    Model containing the configurable elements.
     */
    void createElementChangeCommand(QString const& oldValue, QString const& newValue, QModelIndex const& index,
        QAbstractItemModel* cevModel) const;

protected:

    /*!
     *  Handler for context menu events.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event);

protected slots:

    /*!
     *  Handles the deleting of the selected cell.
     */
    virtual void onClearAction();

private:
   
    // Disable copying.
    ConfigurableElementsView(ConfigurableElementsView const& rhs);
    ConfigurableElementsView& operator=(ConfigurableElementsView const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------

#endif // CONFIGURABLEELEMENTSVIEW_H
