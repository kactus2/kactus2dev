//-----------------------------------------------------------------------------
// File: PortMapView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 31.05.2022
//
// Description:
// A view for bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPVIEW_H
#define PORTMAPVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

class PortMapInterface;

//-----------------------------------------------------------------------------
//! A view for bus interface port maps.
//-----------------------------------------------------------------------------
class PortMapView : public EditableTableView
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *    @param [in] portMapInterface    Interface for accessing port maps.
     *    @param [in] parent              The parent widget.
     */
    PortMapView(PortMapInterface* portMapInterface, QWidget* parent);

    /*!
     *  The destructor.
     */
    ~PortMapView() = default;

protected:

    /*!
     *  Handler for context menu events.
     *
     *    @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event) override final;

    /*!
     *  Handler for drag enter events
	 *
	 *    @param [in] event The event object.
     */
	virtual void dragEnterEvent(QDragEnterEvent* event) override final;

signals:

    /*!
     *  Automatically create port maps for the selected logical signals.
     *
     *    @param [in] logicalSignals  The selected logical signals.
     */
    void autoConnecteLogicalSignals(QStringList const& logicalSignals);

    /*!
     *  Automatically create port maps for all signals.
     */
    void createAllSignals();

    /*!
     *  Automatically create port maps for required signals.
     */
    void createRequiredSignals();

    /*!
     *  Automatically create port maps for optional signals.
     */
    void createOptionalSignals();

private slots:

    /*!
     *  Handler for automatically creating port maps for selected logical signals.
     */
    void onAutoConnect();

private:
   
    // Disable copying.
    PortMapView(PortMapView const& rhs);
    PortMapView& operator=(PortMapView const& rhs);

    /*!
     *  Setup the available context menu actions.
     */
    void setupActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The action for automatically connecting logical signals to physical ports.
    QAction autoConnectAction_;

    //! Action for creating port maps from all signals.
    QAction createAllSignalsAction_;

    //! Action for creating port maps from required signals.
    QAction createRequiredSignalsAction_;

    //! Action for creating port maps from optional signals.
    QAction createOptionalSignalsAction_;

    //! Interface for accessing port maps.
    PortMapInterface* mapInterface_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPVIEW_H
