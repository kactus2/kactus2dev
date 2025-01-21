//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCpuRoutesContainer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.05.2023
//
// Description:
// Container for routes connected to CPUs in Linux Device Tree.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREECPUROUTESCONTAINER_H
#define LINUXDEVICETREECPUROUTESCONTAINER_H

#include <Plugins/common/CpuRoutesContainer.h>

//! Utilities for Linux Device Tree CPU route container
namespace DeviceTreeUtilities
{
    //! Container for interface and the containers connected to it.
    struct CpuDeviceTree
    {
        //! Interface of the container.
        QSharedPointer<const ConnectivityInterface> interface_;

        //! Containers connected to this interface.
        QVector<QSharedPointer<CpuDeviceTree> > childContainers_;
    };
};

//-----------------------------------------------------------------------------
//! Container for routes connected to a single CPU.
//-----------------------------------------------------------------------------
class LinuxDeviceTreeCpuRoutesContainer : public CpuRoutesContainer
{

public:

    /*!
     *  The constructor.
     * 
     *    @param [in] CPU     The CPU.
     */
    LinuxDeviceTreeCpuRoutesContainer();

    /*!
     *  The destructor.
     */
    virtual ~LinuxDeviceTreeCpuRoutesContainer() = default;

    /*!
     *  Copy constructor.
     * 
     *    @param [in] other   The original to be copied.
     */
    LinuxDeviceTreeCpuRoutesContainer(const CpuRoutesContainer& other);

    /*!
     *  Get the contained CPUs.
     *
     *    @return The contained CPUs.
     */
    QVector<QSharedPointer<const Cpu> > getCpus() const;

    /*!
     *  Set the contained CPUs.
     *
     *    @param [in] newCpu  The new CPUs.
     */
    void setCpus(QVector<QSharedPointer<Cpu> > newCpus);

    /*!
     *  Add a new connected CPU.
     *
     *    @param [in] newCpu  The new CPU.
     */
    void addCpu(QSharedPointer<Cpu> newCpu);

    /*!
     *  Get the file path.
     *
     *    @return The file path.
     */
    QString getFilePath() const;

    /*!
     *  Set the file path.
     *
     *    @param [in] newFilePath     The new file path.
     */
    void setFilePath(QString const& newFilePath);

    /*!
     *  Get a list of the interface trees connected to this container.
     *
     *    @return List of interface trees connected to this container.
     */
    QVector<QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> > getDeviceTreeRoutes() const;

    /*!
     *  Set the connected interface trees for this container.
     *
     *    @param [in] newContainers   The new connected interface trees.
     */
    virtual void setRoutes(QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > newContainers) override final;

    /*!
     *  Add a new connected interface tree for this container.
     *
     *    @param [in] newContainer    The new connected interface tree.
     */
    virtual void addRoute(QSharedPointer<CpuRouteStructs::CpuRoute> newContainer) override final;

    //! No copying. No assignment.
    LinuxDeviceTreeCpuRoutesContainer(const LinuxDeviceTreeCpuRoutesContainer& other) = delete;
    LinuxDeviceTreeCpuRoutesContainer& operator=(const LinuxDeviceTreeCpuRoutesContainer& other) = delete;

private:

    /*!
     *  Set up the device tree route containers.
     *
     *    @param [in] newRouteContainers  List of the CPU route containers.
     */
    void setupDeviceTreeRoutes(QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > newRouteContainers);

    /*!
     *  Set up a single device tree route container.
     *
     *    @param [in] routeContainers     The CPU route container.
     */
    void setupSingleDeviceTreeRoute(QSharedPointer<CpuRouteStructs::CpuRoute> routeContainer);

    /*!
     *  Get the device tree route container for the selected interface route.
     *
     *    @param [in] route       The selected route.
     *    @param [in] treeRoutes  List of the available device tree route containers.
     *
     *    @return The device tree route container containing the selected interface route.
     */
    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> getDeviceTreeForRoute(
        QVector<QSharedPointer<const ConnectivityInterface> > route,
        QVector<QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> > treeRoutes);

    /*!
     *  Get the device tree route container for the selected interface.
     *
     *    @param [in] interfaceNode   The selected interface.
     *    @param [in] treeContainers  List of available device tree route containers.
     *
     *    @return The device tree route container containing the selected interface.
     */
    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> getDeviceTreeContainerForInterface(
        QSharedPointer<const ConnectivityInterface> interfaceNode,
        QVector<QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> > treeContainers) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of connected CPUs.
    QVector<QSharedPointer<const Cpu> > cpus_;

    //! List of root interface containers.
    QVector<QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> > deviceTreeRoutes_;

    //! The file path.
    QString filePath_;
};

#endif // LINUXDEVICETREECPUROUTESCONTAINER_H