//-----------------------------------------------------------------------------
// File: PortsView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 2.12.2013
//
// Description:
// View for port editor.
//-----------------------------------------------------------------------------

#ifndef PORTSVIEW_H
#define PORTSVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

#include <QAction>

//-----------------------------------------------------------------------------
// View for port editor.
//-----------------------------------------------------------------------------
class PortsView : public EditableTableView
{
    Q_OBJECT
public:


	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent widget.	 
	 */
	PortsView(QWidget *parent);

	//! The destructor.
	virtual ~PortsView();

signals:

    //! Emitted when new bus interface is selected from the context menu and a new bus definition must be created.
    void createBus(QStringList const& portNames);

    //! Emitted when new bus interface is selected from the context menu.
    void createExistingBus(QStringList const& portNames);

    /*!
     *  Change the vendor extensions of the vendor extensions editor.
     *
     *      @param [in] itemIndex   Index of the selected port.
     */
    void changeExtensionsEditorItem(QModelIndex const& itemIndex);

private slots:

    //! Handler for new bus definition and interface creation.     
    virtual void onCreateNewBus();

    //! Handler for new bus interface creation.
    virtual void onCreateExistingBus();

protected:

    //! Handler for context menu events.
    virtual void contextMenuEvent(QContextMenuEvent* event);

    /*!
     *  Handler for mouse press events.
     */
    virtual void mousePressEvent(QMouseEvent* event);

    /*!
     *  Handler for index change events.
     *
     *      @param [in] current     The current index.
     *      @param [in] previous    The previous index.
     */
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:

    //! Action for creating a new bus definition and interface.
    QAction createBus_;

    //! Action for creating a new bus interface.
    QAction createExistingBus_;
};

#endif // PORTSVIEW_H
