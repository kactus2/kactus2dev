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

#include <IPXACTmodels/common/VLNV.h>

#include <QWizard>
#include <QSharedPointer>

class BusInterface;
class BusInterfaceValidator;
class Component;
class LibraryInterface;
class ExpressionParser;
class ParameterValidator;
class IPXactSystemVerilogParser;
class BusInterfaceInterface;
class FileSetInterface;
class MemoryMapInterface;
class AbstractionTypeInterface;
class PortMapInterface;
class TransparentBridgeInterface;
class PortMapValidator;
class ParameterFinder;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Bus wizard dialog.
//-----------------------------------------------------------------------------
class BusInterfaceWizard : public QWizard
{
    Q_OBJECT

public:
    
    //! Enumeration for the wizard pages.
    enum Pages
    {
        PAGE_INTRO = 0,
        PAGE_GENERALOPTIONS,
        PAGE_BUSDEFINITION,
        PAGE_ABSDEFINITION,
        PAGE_PORTMAPS,
        PAGE_SUMMARY
	 };    

    /*!
     *  The constructor.
     *
     *    @param [in] component                   The component for which the wizard is run.
     *    @param [in] busIf                       The bus interface for which the wizard is run.
     *    @param [in] handler                     Pointer to the instance which manages the library.
     *    @param [in] portNames                   Names of the physical ports used to generate the interface.
     *    @param [in] parent                      The parent widget.
     *    @param [in] absDefVLNV                  VLNV of the fixed abstraction definition of the bus interface.
     *                                              If invalid, VLNV can be chosen freely on general page.
     *    @param [in] descriptionAsLogicalName    Logical signal naming selection.
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
	 ~BusInterfaceWizard() final = default;

signals:

     /*!
      *  Increase the number of references made to a specific parameter.
      *
      *    @param [in] id  The id of the parameter whose reference count is to be increased.
      */
     void increaseReferences(QString id);

     /*!
      *  Decrease the number of references made to a specific parameter.
      *
      *    @param [in] id  The id of the parameter whose reference count is to be decreased.
      */
     void decreaseReferences(QString id);

private:
    // Disable copying.
    BusInterfaceWizard(BusInterfaceWizard const& rhs);
    BusInterfaceWizard& operator=(BusInterfaceWizard const& rhs);
};

#endif // BUSINTERFACEWIZARD_H
