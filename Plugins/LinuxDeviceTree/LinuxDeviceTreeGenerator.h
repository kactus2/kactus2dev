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

#include <QSharedPointer>
#include <QTextStream>

class LibraryInterface;
class Component;
class Cpu;
class AddressSpace;
class Design;
class DesignConfiguration;

//-----------------------------------------------------------------------------
//! Creates a Linux Device Tree from the selected document.
//-----------------------------------------------------------------------------
class LINUXDEVICETREEGENERATOR_EXPORT LinuxDeviceTreeGenerator
{

public:
    
    /*!
     *  The Constructor.
     *
     *      @param [in] library     Interface to the library.
     */
    LinuxDeviceTreeGenerator(LibraryInterface* library);

    /*!
     *  The destructor.
     */
    ~LinuxDeviceTreeGenerator();

    /*!
     *  Generates the linux device tree.
     *
     *      @param [in] topComponent        The top component in the hierarchy to generate listing for.
     *      @param [in] activeView          The active view for the generation.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     *      @param [in] outputPath          Path to the output file.
     */
    void generate(QSharedPointer<Component> topComponent, QString const& activeView, bool writeAddressBlocks,
        QString const& outputPath);
  
private:

    // Disable copying.
    LinuxDeviceTreeGenerator(LinuxDeviceTreeGenerator const& rhs);
    LinuxDeviceTreeGenerator& operator=(LinuxDeviceTreeGenerator const& rhs);

    //! Container for memory data.
    struct CpuMemory
    {
        //! Interface containing the memory item.
        QSharedPointer<ConnectivityInterface const> memoryInterface_;

        //! Base address of the memory location.
        quint64 itemBaseAddress_;

        //! Base address of the memory.
        quint64 mapBaseAddress_;

        //! Range of the memory.
        quint64 range_;
    };

    //! Container for a component and its CPUs connected to an interface.
    struct ComponentCPUContainer
    {
        //! List of the interfaced CPUs.
        QVector<QSharedPointer<const Cpu> > interfacedCPUs_;

        //! List of memory data connected to the CPUs.
        QVector<LinuxDeviceTreeGenerator::CpuMemory> memory_;

        //! Component containing the CPUs.
        QSharedPointer<Component const> containingComponent_;
    };

    /*!
     *  Combine master roots with same CPUs into single master roots.
     *
     *      @param [in] roots   List of master interface nodes.
     *
     *      @return List of combined master roots.
     */
    QVector<QSharedPointer<ConnectivityInterface> > getMasterRoots(
        QVector<QSharedPointer<ConnectivityInterface> > roots);

    /*!
     *  Check if two CPU containers are same.
     *
     *      @param [in] firstContainer      The first compared CPU container.
     *      @param [in] secondContainer     The second compared CPU container.
     *
     *      @return True, if the CPUs are same, false otherwise.
     */
    bool cpuContainersMatch(ComponentCPUContainer const& firstContainer,
        ComponentCPUContainer const& secondContainer) const;

    /*!
     *  Write the device tree file.
     *
     *      @param [in] outputPath          Path for the file.
     *      @param [in] topComponent        The top component in the hierarchy to generate listing for.
     *      @param [in] activeView          The currently active view of the top component.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     *      @param [in] masterRoots         List of the master interfaces.
     */
    void writeFile(QString const& outputPath, QSharedPointer<Component> topComponent, QString const& activeView,
        bool writeAddressBlocks, QVector<QSharedPointer<ConnectivityInterface> >  masterRoots);

	/*!
	 *  Write the found memory paths.
	 *
	 *      @param [in]	outputStream        The stream to write into.
	 *		@param [in]	topComponent        The starting component for the search.
	 *		@param [in]	activeView          The currently active view of the starting component.
	 *		@param [in]	interfaceNodes      List of interface nodes.
     *      @param [in] pathNumber          Number of the current CPU container.
     *      @param [in] connectedCPUs       List of handled CPUs.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void writePaths(QTextStream& outputStream, QSharedPointer<Component> topComponent, QString const& activeView,
        QVector<QSharedPointer<ConnectivityInterface> > interfaceNodes, int pathNumber,
        QVector<QSharedPointer<const Cpu> >& connectedCPUs, bool writeAddressBlocks);

    /*!
     *  Write the selected interface path.
     *
     *      @param [in] outputStream        The stream to write into.
     *      @param [in] path                The selected interface path.
     *      @param [in] cpuContainer        Container for component and its CPUs connected to the starting
     *                                          interface.
     *		@param [in]	pathNumber          The current running number for CPU listings.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void startPathWriting(QTextStream& outputStream, QSharedPointer<const ConnectivityInterface> pathRootNode,
        ComponentCPUContainer cpuContainer, int pathNumber, bool writeAddressBlocks);

    /*!
     *  Write the selected interface node.
     *
     *      @param [in] outputStream        The stream to write into.
     *      @param [in] previousInterface   The previously written interface.
     *      @param [in] interfaceNode       The current interface.
     *      @param [in] baseAddress         Currently effective base address.
     *      @param [in] memoryItemRange     Currently effective range of the memory item.
     *      @param [in] prefix              Current tab prefix of the items.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void writePathNode(QTextStream& outputStream, QSharedPointer<const ConnectivityInterface> previousInterface,
        QSharedPointer<const ConnectivityInterface> interfaceNode, quint64 const& baseAddress,
        quint64 const& memoryItemRange, QString& prefix, bool writeAddressBlocks);

    /*!
     *  Check if an interface node has access to memory interface nodes.
     *
     *      @param [in] interfaceNode   The selected interface node.
     *      @param [in] previousNode    Previous interface node on the tree.
     *
     *      @return True, if an interface has access to memory interface nodes, false otherwise.
     */
    bool canWriteNode(QSharedPointer<const ConnectivityInterface> interfaceNode,
        QSharedPointer<const ConnectivityInterface> previousNode) const;

    /*!
     *	Get all the CPUs connected to the selected interface.
     *
     *		@param [in]	startInterface  The selected interface.
     *
     *		@return Container for component and its CPUs connected to the selected interface.
     */
    ComponentCPUContainer getCPUsForInterface(QSharedPointer<ConnectivityInterface> startInterface) const;

    /*!
     *  Get the component containing the selected interface.
     *
     *      @param [in] interfaceNode   The selected interface.
     *
     *      @return Component containing the selected interface.
     */
    QSharedPointer<Component const> getComponentContainingInterface(
        QSharedPointer<const ConnectivityInterface> interfaceNode) const;

    /*!
     *  Get the memory data of the selected interface.
     *
     *      @param [in] previousInterface   The previously checked interface.
     *      @param [in] interfaceNode       The current interface.
     *      @param [in] baseAddress         Currently active base address.
     *      @param [in] memoryItemRange     Currently active range.
     *
     *      @return List of memory data connected to the selected interface.
     */
    QVector<CpuMemory> getMemories(QSharedPointer<const ConnectivityInterface> previousInterface,
        QSharedPointer<ConnectivityInterface> interfaceNode, quint64 baseAddress,
        quint64 const& memoryItemRange) const;

    /*!
     *  Check if the selected interface contains memory data.
     *
     *      @param [in] memoryInterface     The selected interface.
     *
     *      @return True, if the selected interface contains memory data, false otherwise.
     */
    bool interfacedItemIsMemory(QSharedPointer<const ConnectivityInterface> memoryInterface) const;

    /*!
     *	Get all the CPUs connected to the selected interface from the selected component.
     *
     *		@param [in]	connectionInterface     The selected interface.
     *		@param [in]	containingComponent     The component containing the CPUs.
     *
     *		@return	Container for component and its CPUs connected to the selected interface.
     */
    ComponentCPUContainer getComponentCPUsForInterface(
        QSharedPointer<const ConnectivityInterface> connectionInterface,
        QSharedPointer<Component const> containingComponent) const;

    /*!
     *	Write the start of the device tree.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	pathNumber      The current running number for CPU listings.
     */
    void writeTreeStart(QTextStream& outputStream, int pathNumber);

    /*!
     *	Write a line ending.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	prefix          The prefix for the line ending.
     */
    void writeLineEnding(QTextStream& outputStream, QString const& prefix) const;

    /*!
     *	Write an introduction to the CPUs.
     *
     *		@param [in]	outputStream    The stream to write into.
     *      @param [in] prefix          The prefix for CPUs.
     */
    void writeIntroductionToCPUs(QTextStream& outputStream, QString& prefix);

    /*!
     *  Write a single CPU.
     *
     *		@param [in]	outputStream    The stream to write into.
     *      @param [in] CPUName         Name of the selected CPU.
     *      @param [in] componentVLNV   VLNV of the component containing the selected CPU.
     *		@param [in]	cpuNumber       Number of the CPU.
     *      @param [in] prefix          The prefix for a single CPU.
     */
    void writeCPU(QTextStream& outputStream, QString const& CPUName, VLNV const& componentVLNV, int cpuNumber,
        QString& prefix);

    /*!
     *  Write a single bridge.
     *
     *      @param [in] outputStream    The stream to write into.
     *      @param [in] interfaceNode   Interface containing the bridge.
     *      @param [in] prefix          The prefix for the bridge.
     */
    void writeBridge(QTextStream& outputStream, QSharedPointer<ConnectivityInterface const> interfaceNode,
        QString& prefix);

    /*!
     *  Write a memory map item.
     *
     *      @param [in] outputStream        Stream to write into.
     *      @param [in] interfaceNode       Interface containing the memory item.
     *      @param [in] itemBaseAddress     Base address of the memory location.
     *      @param [in] baseAddress         Base address of the memory item.
     *      @param [in] range               Range of the memory item.
     *      @param [in] prefix              Prefix of the memory map item.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void writeMemoryMapItem(QTextStream& outputStream, QSharedPointer<ConnectivityInterface const> interfaceNode,
        quint64 itemBaseAddress, quint64 mapBaseAddress, quint64 range, QString& prefix, bool writeAddressBlocks);

    /*!
     *  Write memory item.
     *
     *      @param [in] outputStream        Stream to write into.
     *      @param [in] itemName            Name of the memory item.
     *      @param [in] instanceName        Name of the instance containing the memory item.
     *      @param [in] componentVLNV       VLNV of the component instance.
     *      @param [in] baseAddress         Base address of the memory item.
     *      @param [in] range               Range of the memory item.
     *      @param [in] isMemory            Item memory usage.
     *      @param [in] prefix              Prefix of the memory item.
     *      @param [in] memoryNode          Interface containing the memory item.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void writeMemoryData(QTextStream& outputStream, QString const& itemName, QString const& instanceName,
        QString const& componentVLNV, quint64 const& itemBaseAddress, quint64 const& mapBaseAddress,
        quint64 const& range, bool isMemory, QString& prefix,
        QSharedPointer<ConnectivityInterface const> memoryNode, bool writeAddressBlocks) const;

    /*!
     *  Write address blocks.
     *
     *      @param [in] outputStream        Stream to write into.
     *      @param [in] memoryNode          Interface containing the memory item.
     *      @param [in] itemBaseAddress     Base address of the memory location.
     *      @param [in] prefix              Prefix of the address blocks.
     */
    void writeAddressBlocksData(QTextStream& outputStream, QSharedPointer<ConnectivityInterface const> memoryNode,
        quint64 const& itemBaseAddress, QString& prefix) const;

    /*!
     *  Write the selected address block.
     *
     *      @param [in] outputStream        Stream to write into.
     *      @param [in] memoryNode          Interface containing the memory item.
     *      @param [in] blockItem           The selected address block memory item.
     *      @param [in] itemBaseAddress     Base address of the memory location.
     *      @param [in] prefix              Prefix of the address blocks.
     */
    void writeSingleAddressBlock(QTextStream& outputStream,
        QSharedPointer<ConnectivityInterface const> containingMemoryNode, QSharedPointer<MemoryItem> blockItem,
        quint64 const& itemBaseAddress, QString& prefix) const;

    /*!
     *  Write the memory items with usage memory.
     *
     *      @param [in] outputStream        Stream to write into.
     *      @param [in] cpuContainer        CPU containing the memories.
     *      @param [in] prefix              Prefix of the memories.
     *      @param [in] writeAddressBlocks  Flag for writing address block data.
     */
    void writeMemories(QTextStream& outputStream, ComponentCPUContainer const& cpuContainer, QString& prefix,
        bool writeAddressBlocks) const;

    /*!
     *	Write the unconnected CPUs of the design.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	pathNumber      The current running number for CPU listings.
     *		@param [in]	connectedCPUs   List of the connected CPUs.
     *		@param [in]	topComponent    The top component of the design.
     *		@param [in]	activeView      Currently active view of the top component.
     */
    void writeUnconnectedCPUs(QTextStream& outputStream, int& pathNumber,
        QVector<QSharedPointer<Cpu const> >& connectedCPUs, QSharedPointer<Component const> topComponent,
        QString const& activeView);

    /*!
     *	Write the unconnected CPUS of the selected component.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	pathNumber      The current running number for CPU listings.
     *		@param [in]	component       The selected component.
     *		@param [in]	connectedCPUs   List of the connected CPUs.
     */
    void writeUnconnectedCPUsOfComponent(QTextStream& outputStream, int& pathNumber,
        QSharedPointer<Component const> component, QVector<QSharedPointer<Cpu const> >& connectedCPUs);

    /*!
     *	Get the active view matching the selected view name.
     *
     *		@param [in]	containingComponent     Component containing the views.
     *		@param [in]	viewName                Name of the currently active view.
     *
     *      @return The currently active view.
     */
    QSharedPointer<View> getView(QSharedPointer<Component const> containingComponent, QString const& viewName)
        const;

    /*!
     *	Get the design configuration referenced by the selected view.
     *
     *		@param [in]	containingComponent     Component containing the required items.
     *		@param [in]	referencingView         The selected view.
     *
     *      @return Design configuration referenced by the selected view.
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

    /*!
     *	Search the selected design for any unconnected CPUs.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	pathNumber      The current running number for CPU listings.
     *		@param [in]	connectedCPUs   List of the connected CPUs.
     *		@param [in]	design          The selected design.
     *		@param [in]	configuration   Design configuration of the selected design.
     */
    void analyzeDesignForUnconnectedCPUs(QTextStream& outputStream, int& pathNumber,
        QVector<QSharedPointer<Cpu const> >& connectedCPUs, QSharedPointer<const Design> design,
        QSharedPointer<const DesignConfiguration> configuration);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    //! The connectivity graph factory.
    ConnectivityGraphFactory graphFactory_;
};

#endif // LINUXDEVICETREEGENERATOR_H
