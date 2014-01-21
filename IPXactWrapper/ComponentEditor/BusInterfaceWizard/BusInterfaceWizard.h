//-----------------------------------------------------------------------------
// File: BusInterfaceWizard.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Wizard for generating bus interface from a set of ports.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARD_H
#define BUSINTERFACEWIZARD_H

#include <QWizard>

#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>
#include <models/businterface.h>

class Component;
class PluginManager;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Bus wizard dialog.
//-----------------------------------------------------------------------------
class BusInterfaceWizard : public QWizard
{
public:
    enum Page
    {
        PAGE_INTRO = 0,
        PAGE_VLNVSELECTION,
        PAGE_BUSDEFINITION,
        PAGE_PORTMAP,
        PAGE_SUMMARY
	 };

    

	 /*!
	 *  Constructor.
	 *
	 *      @param [in] component  The component for which the wizard is run.
	 *      @param [in] handler	   Pointer to the instance which manages the library.
	 *      @param [in] parent     The parent widget.
	 */
    BusInterfaceWizard(QSharedPointer<Component> component,  
        QSharedPointer<BusInterface> busIf,
        LibraryInterface* handler, 
        QWidget* parent);

	 /*!
	 *  Destructor.
	 */
	 ~BusInterfaceWizard();

    QStringList getPorts() const;

    void setPorts(QStringList const& ports);

    VLNV getCreatedAbsDef();

    void setCreatedAbsDef(VLNV& createdVLNV, bool useDescription);


private:
    // Disable copying.
    BusInterfaceWizard(BusInterfaceWizard const& rhs);
    BusInterfaceWizard& operator=(BusInterfaceWizard const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QStringList ports_;

    VLNV createdVLVN_;

    QSharedPointer<Component> component_;

    QSharedPointer<BusInterface> busIf_;

    LibraryInterface* handler_;

    bool namesFromDescription_;
};

#endif // BUSINTERFACEWIZARD_H
