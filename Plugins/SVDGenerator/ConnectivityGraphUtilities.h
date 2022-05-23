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

#include <KactusAPI/include/LibraryInterface.h>

#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Utility functions for connectivity graphs.
//-----------------------------------------------------------------------------
namespace ConnectivityGraphUtilities
{
    const QString REVISION_FORMAT = "rNpM(N, M = [0 - 99])";

    //! Container for routes connected to a master interface.
    struct cpuDetailRoutes
    {
        //! Name of the CPU.
        QString cpuName_;

        //! ID of the CPU.
        QString cpuID_;

        //! Flag for creating an SVD file.
        bool createFile_;

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

    /*!
     *  Get the CPUs of the selected component.
     *
     *      @param [in] library     The library interface containing the selected component.
     *      @param [in] component   The selected component.
     *      @param [in] viewName    Name of the selected view.
     *
     *      @return List of the component CPU routes.
     */
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > getDefaultCPUs(LibraryInterface* library,
        QSharedPointer<Component> component, QString const& viewName);

    /*!
     *  Check if a CPU exists for the selected interface.
     *
     *      @param [in] master      The selected interface.
     *      @param [in] cpuList     List of available CPUs.
     *
     *      @return True, if a check box exists, false otherwise.
     */
    bool interfacedCpuExists(QSharedPointer<const ConnectivityInterface> master,
        QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > cpuList);

    /*!
     *  Get the CPU container of the selected interface.
     *
     *      @param [in] master      The selected interface.
     *      @param [in] cpuList     List of available CPUs.
     *
     *      @return CPU check box container matching the selected interface.
     */
    QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> getMatchingCpuContainer(
        QSharedPointer<const ConnectivityInterface> master,
        QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > cpuList);
};

#endif //CONNECTIVITYGRAPHUTILITIES_H