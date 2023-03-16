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

#include <Plugins/common/CPUDetailRoutes.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Utility functions for connectivity graphs.
//-----------------------------------------------------------------------------
namespace ConnectivityGraphUtilities
{
    const QString REVISION_FORMAT = "rNpM(N, M = [0 - 99])";

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
    QVector<QSharedPointer<CPUDetailRoutes> > getDefaultCPUs(LibraryInterface* library, QSharedPointer<Component> component, QString const& viewName);

    /*!
     *  Check if a CPU exists for the selected interface.
     *
     *      @param [in] master      The selected interface.
     *      @param [in] cpuList     List of available CPUs.
     *
     *      @return True, if a check box exists, false otherwise.
     */
    bool interfacedCpuExists(QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<CPUDetailRoutes> > cpuList);

    /*!
     *  Get the CPU container of the selected interface.
     *
     *      @param [in] master      The selected interface.
     *      @param [in] cpuList     List of available CPUs.
     *
     *      @return CPU check box container matching the selected interface.
     */
    QSharedPointer<CPUDetailRoutes> getMatchingCpuContainer(QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<CPUDetailRoutes> > cpuList);
};

#endif //CONNECTIVITYGRAPHUTILITIES_H