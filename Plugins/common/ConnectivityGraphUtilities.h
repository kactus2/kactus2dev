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

#include <Plugins/common/SingleCpuRoutesContainer.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QCheckBox>

class Cpu;
class IPluginUtility;

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
     *  Get the list of CPUs connected to the selected interface.
     *
     *      @param [in] containingComponent     Component containing the interface.
     *      @param [in] routeInterface          The selected interface.
     *
     *      @return List of CPUs connected to the selected interface.
     */
    QVector<QSharedPointer<Cpu> > getInterfacedCPUs(QSharedPointer<const Component> containingComponent,
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
    QVector<QSharedPointer<SingleCpuRoutesContainer> > getDefaultCPUs(LibraryInterface* library, QSharedPointer<Component> component, QString const& viewName);

    /*!
     *  Check if a CPU exists for the selected interface.
     *
     *      @param [in] master      The selected interface.
     *      @param [in] cpuList     List of available CPUs.
     *
     *      @return True, if a check box exists, false otherwise.
     */
    bool interfacedCpuExists(QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<SingleCpuRoutesContainer> > cpuList);

    /*!
     *  Get the route container for the selected interface.
     *
     *      @param [in] masterInterface     The selected interface.
     *      @param [in] cpuRoutes           List of the available route containers.
     *
     *      @return The route container for the selected interface.
     */
    QSharedPointer<CpuRouteStructs::CpuRoute> getRouteForInterface(QSharedPointer<const ConnectivityInterface> masterInterface, QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > cpuRoutes);

    /*!
     *  Get the CPU containers of the selected interface.
     *
     *      @param [in] master      The selected interface.
     *      @param [in] cpuList     List of available CPUs.
     *
     *      @return CPU containers matching the selected interface.
     */
    QVector<QSharedPointer<SingleCpuRoutesContainer> > getMatchingCpuContainers(QSharedPointer<const ConnectivityInterface> master, QVector<QSharedPointer<SingleCpuRoutesContainer> > cpuList);

    /*!
     *  Get the CPU route container for the selected CPU.
     *
     *      @param [in] comparisonCpu   The selected CPU.
     *      @param [in] existingRoutes  List of available CPU route containers.
     *
     *      @return The CPU route container containing the selected CPU.
     */
    QSharedPointer<SingleCpuRoutesContainer> getCpuDetailRoute(QSharedPointer<Cpu> comparisonCpu, QVector<QSharedPointer<SingleCpuRoutesContainer> > existingRoutes);
};

#endif //CONNECTIVITYGRAPHUTILITIES_H