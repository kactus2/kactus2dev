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

#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/busdefinition.h>
#include <IPXACTmodels/businterface.h>

class Component;
class PluginManager;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Bus wizard dialog.
//-----------------------------------------------------------------------------
class BusInterfaceWizard : public QWizard
{
public:
    
    //! Enumeration for the wizard pages.
    enum Pages
    {
        PAGE_INTRO = 0,
        PAGE_GENERALOPTIONS,
        PAGE_BUSDEFINITION,
        PAGE_PORTMAPS,
        PAGE_SUMMARY
	 };    

    /*!
     *  The constructor.
     *
     *      @param [in] component                   The component for which the wizard is run.
     *      @param [in] busIf                       The bus interface for which the wizard is run.
     *      @param [in] handler                     Pointer to the instance which manages the library.
     *      @param [in] portNames                   Names of the physical ports used to generate the interface.
     *      @param [in] parent                      The parent widget.
     *      @param [in] absDefVLNV                  VLNV of the fixed abstraction definition of the bus interface.
     *                                              If invalid, VLNV can be chosen freely on general page.
     *      @param [in] descriptionAsLogicalName    Logical signal naming selection.
     */
    BusInterfaceWizard(QSharedPointer<Component> component,  
        QSharedPointer<BusInterface> busIf,
        LibraryInterface* handler, QStringList portNames,         
        QWidget* parent = 0, 
        VLNV absDefVLNV = VLNV(), 
        bool descriptionAsLogicalName = false);

	 /*!
	 *  Destructor.
	 */
	 ~BusInterfaceWizard();

private:
    // Disable copying.
    BusInterfaceWizard(BusInterfaceWizard const& rhs);
    BusInterfaceWizard& operator=(BusInterfaceWizard const& rhs);
};

#endif // BUSINTERFACEWIZARD_H
