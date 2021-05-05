//-----------------------------------------------------------------------------
// File: ConnectivityGraphUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.04.2021
//
// Description:
// Utility functions for connectivity graphs.
//-----------------------------------------------------------------------------

#ifndef CONNECTIVITYGRAPHUTILITIES_H
#define CONNECTIVITYGRAPHUTILITIES_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
//! Utility functions for connectivity graphs.
//-----------------------------------------------------------------------------
namespace ConnectivityGraphUtilities
{
    //! Container for routes connected to a master interface.
    struct interfaceRoutes 
    {
        //! The master interface.
        QSharedPointer<const ConnectivityInterface> masterInterface_;

        //! Routes connected to the master interface.
        QVector < QVector<QSharedPointer<const ConnectivityInterface> > > routes_;
    };

    /*!
     *  Get the component contained within the connectivity component.
     *
     *      @param [in] library             Interface for accessing the library.
     *      @param [in] masterComponent     The connectivity component.
     *
     *      @return Component contained within the connectivity component.
     */
    QSharedPointer<const Component> getInterfacedComponent(LibraryInterface* library,
        QSharedPointer<const ConnectivityComponent> masterComponent);

    /*!
     *  Get the CPU connected to the selected interface.
     *
     *      @param [in] containingComponent     Component containing the interface.
     *      @param [in] routeInterface          The selected interface.
     *
     *      @return CPU connected to the selected interface.
     */
    QSharedPointer<Cpu> getInterfacedCPU(QSharedPointer<const Component> containingComponent,
        QSharedPointer<const ConnectivityInterface> routeInterface);
};

#endif //CONNECTIVITYGRAPHUTILITIES_H