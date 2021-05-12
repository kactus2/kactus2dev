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

#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Utility functions for connectivity graphs.
//-----------------------------------------------------------------------------
namespace ConnectivityGraphUtilities
{
    //! Container for routes connected to a master interface.
    struct cpuCheckInterface 
    {
        QString cpuID_;

        //! Check box for the CPU.
        QCheckBox* cpuCheck_;

        //! Revision of the CPU.
        QString revision_;

        //! Endian data of the CPU.
        QString endian_;

        //! MPU presence of the CPU.
        bool mpuPresent_;

        //! FPU presence of the CPU.
        bool fpuPresent_;

        //! Number of NVIC bits of the CPU.
        QString nvicPrioBits_;

        //! Vendor-specific system tick timer of the CPU.
        bool vendorSystickConfig_;

        //! Interface containing the CPU.
        QSharedPointer<const ConnectivityInterface> cpuInterface_;

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