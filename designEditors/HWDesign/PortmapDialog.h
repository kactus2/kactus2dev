//-----------------------------------------------------------------------------
// File: PortmapDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 10.6.2011
//
// Description:
// Port map dialog.
//-----------------------------------------------------------------------------

#ifndef PORTMAPDIALOG_H
#define PORTMAPDIALOG_H

#include <QDialog>

class BusInterface;
class Component;
class LibraryInterface;
class BusInterfacePortMapTab;

//-----------------------------------------------------------------------------
//! PortmapDialog class.
//-----------------------------------------------------------------------------
class PortmapDialog : public QDialog
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] libInterface  The library interface.
     *      @param [in] component     The parent component of the bus interface.
     *      @param [in] busIf         The bus interface whose port map will be edited.
     *      @param [in] otherBusIf    The bus interface in the other end of the connection.
     */
    PortmapDialog(LibraryInterface* libInterface, QSharedPointer<Component> component,
                  QSharedPointer<BusInterface> busIf, QSharedPointer<BusInterface> otherBusIf, QWidget* parent = 0);

    //! The destructor.
    ~PortmapDialog();

public slots:
    void accept();

private:
    // Disable copying.
    PortmapDialog(PortmapDialog const& rhs);
    PortmapDialog& operator=(PortmapDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Port map editor widget.
    BusInterfacePortMapTab* portmapWidget_;

    //! The bus interface whose port map to edit.
    QSharedPointer<BusInterface> busIf_;

    //! The bus interface in the other end of the connection (for validity checks).
    QSharedPointer<BusInterface> otherBusIf_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPDIALOG_H
