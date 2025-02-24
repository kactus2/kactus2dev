//-----------------------------------------------------------------------------
// File: LinuxDeviceTreePlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Copyright: Tampere University of Technology
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Creates a Linux Device Tree from the selected document.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREEGENERATOR_H
#define LINUXDEVICETREEGENERATOR_H

#include "LinuxDeviceTreeGenerator_global.h"

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/MemoryConnectionAddressCalculator.h>

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUDetails.h>

#include <QSharedPointer>
#include <QTextStream>

class LibraryInterface;
class Component;
class Cpu;
class AddressSpace;
class Design;
class DesignConfiguration;
class LinuxDeviceTreeCpuRoutesContainer;

//-----------------------------------------------------------------------------
//! Creates a Linux Device Tree from the selected document.
//-----------------------------------------------------------------------------
class LINUXDEVICETREEGENERATOR_EXPORT LinuxDeviceTreeGenerator
{

public:
    
    /*!
     *  The Constructor.
     *
     *    @param [in] library     Interface to the library.
     */
    LinuxDeviceTreeGenerator(LibraryInterface* library);

    /*!
     *  The destructor.
     */
    ~LinuxDeviceTreeGenerator() = default;

    /*!
     *  Generates the linux device tree.
     *
     *    @param [in] topComponent        The top component in the hierarchy to generate listing for.
     *    @param [in] activeView          The active view for the generation.
     *    @param [in] writeAddressBlocks  Flag for writing address block data.
     *    @param [in] cpuContainers       List of CPU containers.
     *    @param [in] outputFolder        Path to the output folder.
     */
    bool generate(QSharedPointer<Component> topComponent,
        QString const& activeView,
        bool writeAddressBlocks,
        QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > cpuContainers,
        QString const& outputFolder);

private:

    // Disable copying.
    LinuxDeviceTreeGenerator(LinuxDeviceTreeGenerator const& rhs);
    LinuxDeviceTreeGenerator& operator=(LinuxDeviceTreeGenerator const& rhs);

    /*!
     *  Write the device tree file.
     *
     *    @param [in] designVendor        Vendor of the selected design.
     *    @param [in] designName          Name of the selected design.
     *    @param [in] cpuContainer        Container for CPUs.
     *    @param [in] writeAddressBlocks  Flag for writing address block data.
     *    @param [in] outputFolder        Path for the folder.
     */
    void writeFile(QString const& designVendor,
        QString const& designName,
        QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> cpuContainer,
        bool writeAddressBlocks,
        QString const& outputFolder);

    /*!
     *	Write the start of the device tree.
     *
     *		@param [in]	outputStream    The stream to write into.
     *    @param [in] designVendor    Vendor of the selected design.
     *    @param [in] designName      Name of the selected design.
     *    @param [in] masterRoots     Roots of the connectivity graph.
     *    @param [in] prefix          Current tab prefix of the items.
     */
    void writeTreeStart(QTextStream& outputStream, QString const& designVendor, QString const& designName, QString const& prefix, int addressSize, int rangeSize) const;

	/*!
	 *  Write the found memory paths of the selected CPU container.
	 *
	 *    @param [in]	outputStream        The stream to write into.
     *    @param [in] cpuContainer        The selected CPU container.
     *    @param [in] writeAddressBlocks  Flag for writing address block data.
     *    @param [in] prefix              Current tab prefix of the items.
     */
    void writeContainer(QTextStream& outputStream,
        QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> cpuContainer, bool writeAddressBlocks,
        QString& prefix, int addressSize, int rangeSize);

    /*!
     *  Write the selected interface path.
     *
     *    @param [in] outputStream        The stream to write into.
     *    @param [in] pathRootNode        Root interface of the path.
     *    @param [in] cpuContainer        CPU container containing the path.
     *    @param [in] writeAddressBlocks  Flag for writing address block data.
     *    @param [in] prefix              Current tab prefix of the items.
     */
    void startPathWriting(QTextStream& outputStream,
        QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> nodeContainer,
        bool writeAddressBlocks,
        QString& prefix,
        int addressSize,
        int rangeSize);

    /*!
     *	Write an introduction to the CPUs.
     *
     *		@param [in]	outputStream    The stream to write into.
     *    @param [in] cpus            The selected CPUs.
     *    @param [in] componentVLNV   VLNV of the component containing the CPUs.
     *    @param [in] prefix          The prefix for CPUs.
     */
    void writeIntroductionToCPUs(QTextStream& outputStream, QVector<QSharedPointer<const Cpu> > cpus,
        QString const& componentVLNV, QString& prefix);

    /*!
     *  Write a single CPU.
     *
     *		@param [in]	outputStream    The stream to write into.
     *    @param [in] CPUName         Name of the selected CPU.
     *    @param [in] componentVLNV   VLNV of the component containing the selected CPU.
     *		@param [in]	cpuNumber       Number of the CPU.
     *    @param [in] prefix          The prefix for a single CPU.
     */
    void writeCPU(QTextStream& outputStream, QString const& CPUName, QString const& componentVLNV, int cpuNumber,
        QString& prefix);

    /*!
     *  Write the selected interface node.
     *
     *    @param [in] outputStream        The stream to write into.
     *    @param [in] previousInterface   The previously written interface.
     *    @param [in] nodeContainer       Container for the interface node.
     *    @param [in] baseAddress         Currently effective base address.
     *    @param [in] memoryItemRange     Currently effective range of the memory item.
     *    @param [in] prefix              Current tab prefix of the items.
     *    @param [in] writeAddressBlocks  Flag for writing address block data.
     *    @param [in] addressSize         Current address size.
     *    @param [in] rangeSize           Current range size.
     */
    void writePathNode(QTextStream& outputStream,
        QSharedPointer<const ConnectivityInterface> previousInterface,
        QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> nodeContainer,
        quint64 const& baseAddress,
        quint64 const& memoryItemRange,
        QString& prefix,
        bool writeAddressBlocks,
        int addressSize,
        int rangeSize);

    /*!
     *  Check if an interface node has access to memory interface nodes.
     *
     *    @param [in] interfaceNode   The selected interface node.
     *    @param [in] previousNode    Previous interface node on the tree.
     *
     *    @return True, if an interface has access to memory interface nodes, false otherwise.
     */
    bool canWriteNode(QSharedPointer<const ConnectivityInterface> interfaceNode,
        QSharedPointer<const ConnectivityInterface> previousNode) const;

    /*!
     *	Write a line ending.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	prefix          The prefix for the line ending.
     */
    void writeLineEnding(QTextStream& outputStream, QString const& prefix) const;

    /*!
     *  Write a single bridge.
     *
     *    @param [in] outputStream    The stream to write into.
     *    @param [in] interfaceNode   Interface containing the bridge.
     *    @param [in] bridgeType      The bridge type.
     *    @param [in] baseAddress     The current base address of the connection chain.
     *    @param [in] memoryRange     Range given for memory items.
     *    @param [in] prefix          The prefix for the bridge.
     */
    void writeBridge(QTextStream& outputStream, QSharedPointer<ConnectivityInterface const> interfaceNode,
        QString const& bridgeType, int addressSize, int rangeSize, QString& prefix);

    /*!
     *  Get address and size requirements of the selected interface node.
     *
     *    @param [in] rootContainer   Container containing the selected interface node.
     *    @param [in] previousNode    Previous node in the connectivity graph.
     *    @param [in] baseAddress     The current base address of the connection chain.
     *    @param [in] memoryRange     Current range for the memory items.
     *    @param [in] stopAtBridges   Flag for stopping the requirement check at bridge components.
     *
     *    @return Addres / size requirement pair.
     */
    QPair<quint64, quint64> getAddressAndSizeRequirements(QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> rootContainer,
        QSharedPointer<ConnectivityInterface const> previousNode,
        quint64 baseAddress,
        quint64 memoryRange,
        bool stopAtBridges);

    /*!
     *  Calculate the amount of 32 bit sizes of the selected value.
     *
     *    @param [in] requirementValue    The selected value.
     *
     *    @return The amount of 32 bit sizes of the selected value.
     */
    unsigned int calculateWordSize(quint64 const& requirementValue) const;

    /*!
     *  Write memory item.
     *
     *    @param [in] outputStream        Stream to write into.
     *    @param [in] itemName            Name of the memory item.
     *    @param [in] instanceName        Name of the instance containing the memory item.
     *    @param [in] componentVLNV       VLNV of the component instance.
     *    @param [in] baseAddress         Base address of the memory item.
     *    @param [in] range               Range of the memory item.
     *    @param [in] isMemory            Item memory usage.
     *    @param [in] prefix              Prefix of the memory item.
     *    @param [in] memoryNode          Interface containing the memory item.
     *    @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void writeMemoryData(QTextStream& outputStream, quint64 const& itemBaseAddress, quint64 const& mapBaseAddress,
        quint64 const& range,
        int addressSize, int rangeSize, bool isMemory, QString& prefix,
        QSharedPointer<ConnectivityInterface const> memoryNode, bool writeAddressBlocks) const;

    /*!
     *  Write address blocks.
     *
     *    @param [in] outputStream        Stream to write into.
     *    @param [in] memoryNode          Interface containing the memory item.
     *    @param [in] itemBaseAddress     Base address of the memory location.
     *    @param [in] prefix              Prefix of the address blocks.
     */
    void writeAddressBlocksData(QTextStream& outputStream, QSharedPointer<ConnectivityInterface const> memoryNode,
        quint64 const& itemBaseAddress, int addressSize, int rangeSize, QString& prefix) const;

    /*!
     *  Write the selected address block.
     *
     *    @param [in] outputStream        Stream to write into.
     *    @param [in] memoryNode          Interface containing the memory item.
     *    @param [in] blockItem           The selected address block memory item.
     *    @param [in] itemBaseAddress     Base address of the memory location.
     *    @param [in] prefix              Prefix of the address blocks.
     */
    void writeSingleAddressBlock(QTextStream& outputStream,
        QString const& vendorName, QSharedPointer<MemoryItem> blockItem,
        quint64 const& itemBaseAddress, int addressSize, int rangeSize, QString& prefix) const;

    QString getStatus(QSharedPointer<MemoryItem> blockItem) const;

    void writeRegister(QTextStream& output, quint64 const& address, quint64 const& range, 
        int addressSize, int rangeSize,
        QString const& prefix) const;

    QString formatValue(quint64 const& address, int addressSize) const;

    /*!
     *  Write the memory items with usage memory.
     *
     *    @param [in] outputStream        Stream to write into.
     *    @param [in] cpuContainer        CPU containing the memories.
     *    @param [in] prefix              Prefix of the memories.
     *    @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void writeMemories(QTextStream& outputStream,
        QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> cpuContainer, QString& prefix,
        bool writeAddressBlocks,int addressSize, int rangeSize) const;

    /*!
     *	Get the active view matching the selected view name.
     *
     *		@param [in]	containingComponent     Component containing the views.
     *		@param [in]	viewName                Name of the currently active view.
     *
     *    @return The currently active view.
     */
    QSharedPointer<View> getView(QSharedPointer<Component const> containingComponent, QString const& viewName)
        const;

    /*!
     *	Get the design configuration referenced by the selected view.
     *
     *		@param [in]	containingComponent     Component containing the required items.
     *		@param [in]	referencingView         The selected view.
     *
     *    @return Design configuration referenced by the selected view.
     */
    QSharedPointer<const DesignConfiguration> getDesignConfiguration(
        QSharedPointer<Component const> containingComponent, QSharedPointer<View> referencingView) const;

    /*!
     *	Get the design referenced by the selected view.
     *
     *		@param [in]	containingComponent     Component containing the required items.
     *		@param [in]	referencingView         The selected view.
     *		@param [in]	designConfiguration     Design configuration referenced by the selected view.
     *
     *		@return	Design referenced by the selected view.
     */
    QSharedPointer<const Design> getHierarchicalDesign(QSharedPointer<Component const> containingComponent,
        QSharedPointer<View> referencingView, QSharedPointer<const DesignConfiguration> designConfiguration) const;

    /*!
     *	Get the VLNV of the hierarchical design referenced by the selected view.
     *
     *		@param [in]	containingComponent     Component containing the selected view.
     *		@param [in]	referencingView         The selected view.
     *
     *		@return	VLNV of the hierarchical design referenced by the selected view.
     */
    VLNV getHierarchicalDesignVLNV(QSharedPointer<Component const> containingComponent,
        QSharedPointer<View> referencingView) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    //! The connectivity graph factory.
    ConnectivityGraphFactory graphFactory_;
};

#endif // LINUXDEVICETREEGENERATOR_H
