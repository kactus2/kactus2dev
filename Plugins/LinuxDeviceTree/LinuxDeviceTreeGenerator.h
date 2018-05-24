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
     *      @param [in] topComponent    The top component in the hierarchy to generate listing for.
     *      @param [in] activeView      The active view for the generation.
     *      @param [in] outputPath      Path to the output file.
     */
    void generate(QSharedPointer<Component> topComponent, QString const& activeView, QString const& outputPath);
  
private:

    // Disable copying.
    LinuxDeviceTreeGenerator(LinuxDeviceTreeGenerator const& rhs);
    LinuxDeviceTreeGenerator& operator=(LinuxDeviceTreeGenerator const& rhs);

    /*!
     *  Write the device tree file.
     *
     *      @param [in] outputPath      Path for the file.
     *      @param [in] topComponent    The top component in the hierarchy to generate listing for.
     *      @param [in] activeView      The currently active view of the top component.
     *      @param [in] masterRoutes    List of the master slave routes that should be examined.
     */
    void writeFile(QString const& outputPath, QSharedPointer<Component> topComponent, QString const& activeView,
        QVector<QVector<QSharedPointer<const ConnectivityInterface> > >  masterRoutes);

	/*!
	 *  Write the found memory paths.
	 *
	 *      @param [in]	outputStream    The stream to write into.
	 *		@param [in]	topComponent    The starting component for the search.
	 *		@param [in]	activeView      The currently active view of the starting component.
	 *		@param [in]	masterPaths     List of the master slave paths.
	 */
	void writePaths(QTextStream& outputStream, QSharedPointer<Component> topComponent, QString const& activeView,
        QVector<QVector<QSharedPointer<const ConnectivityInterface> > > masterPaths);

    /*!
     *  Write the selected interface path.
     *
     *      @param [in] outputStream    The stream to write into.
     *      @param [in] path            The selected interface path.
     *      @param [in] interfacedCPUs  List of CPUs connected to starting point of the selected path.
     *		@param [in]	pathNumber      The current running number for CPU listings.
     */
    void writePath(QTextStream& outputStream, QVector<QSharedPointer<const ConnectivityInterface> > path,
        QVector<QSharedPointer<Cpu> >& interfacedCPUs, int pathNumber);

    /*!
     *	Get all the CPUs connected to the selected interface.
     *
     *		@param [in]	startInterface  The selected interface.
     *
     *		@return A list of the CPUs connected to the selected interface.
     */
    QVector<QSharedPointer<Cpu > > getCPUsForInterface(QSharedPointer<const ConnectivityInterface> startInterface)
        const;

    /*!
     *	Get all the CPUs connected to the selected interface from the selected componetn.
     *
     *		@param [in]	connectionInterface     The selected interface.
     *		@param [in]	containingComponent     The component containing the CPUs.
     *
     *		@return	List of all the CPUs within the selected component connected to the selected interface.
     */
    QVector<QSharedPointer<Cpu> > getComponentCPUsForInterface(
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
    void writeLineEnding(QTextStream& outputStream, QString const& prefix);

    /*!
     *	Write an introduction to the CPUs.
     *
     *		@param [in]	outputStream    The stream to write into.
     */
    void writeIntroductionToCPUs(QTextStream& outputStream);

    /*!
     *  Write a single CPU.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	cpuNumber       Number of the CPU.
     */
    void writeCPU(QTextStream& outputStream, int cpuNumber);

    /*!
     *	Write the items connected to the selected starting interface.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	startInterface  The selected interface.
     *		@param [in]	endInterface    The end interface of the path.
     *		@param [in]	pathVariables   Variables calculated from the interface path.
     */
    void writeMultipleItems(QTextStream& outputStream, QSharedPointer<const ConnectivityInterface> startInterface,
        QSharedPointer<const ConnectivityInterface> endInterface,
        MemoryConnectionAddressCalculator::ConnectionPathVariables pathVariables);

    /*!
     *	Write a single item.
     *
     *		@param [in]	outputStream    The stream to write into.
     *		@param [in]	itemName        Name of the selected item.
     *		@param [in]	baseAddress     Base address of the selected item.
     *		@param [in]	lastAddress     Last address of the selected item.
     */
    void writeItem(QTextStream& outputStream, QString const& itemName, quint64 baseAddress, quint64 lastAddress);

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
        QVector<QSharedPointer<Cpu> >& connectedCPUs, QSharedPointer<Component> topComponent,
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
        QSharedPointer<Component> component, QVector<QSharedPointer<Cpu> >& connectedCPUs);

    /*!
     *	Get the active view matching the selected view name.
     *
     *		@param [in]	containingComponent     Component containing the views.
     *		@param [in]	viewName                Name of the currently active view.
     *
     *      @return The currently active view.
     */
    QSharedPointer<View> getView(QSharedPointer<Component> containingComponent, QString const& viewName) const;

    /*!
     *	Get the design configuration referenced by the selected view.
     *
     *		@param [in]	containingComponent     Component containing the required items.
     *		@param [in]	referencingView         The selected view.
     *
     *      @return Design configuration referenced by the selected view.
     */
    QSharedPointer<const DesignConfiguration> getDesignConfiguration(QSharedPointer<Component> containingComponent,
        QSharedPointer<View> referencingView) const;

    /*!
     *	Get the design referenced by the selected view.
     *
     *		@param [in]	containingComponent     Component containing the required items.
     *		@param [in]	referencingView         The selected view.
     *		@param [in]	designConfiguration     Design configuration referenced by the selected view.
     *
     *		@return	Design referenced by the selected view.
     */
    QSharedPointer<const Design> getHierarchicalDesign(QSharedPointer<Component> containingComponent,
        QSharedPointer<View> referencingView, QSharedPointer<const DesignConfiguration> designConfiguration) const;

    /*!
     *	Get the VLNV of the hierarchical design referenced by the selected view.
     *
     *		@param [in]	containingComponent     Component containing the selected view.
     *		@param [in]	referencingView         The selected view.
     *
     *		@return	VLNV of the hierarchical design referenced by the selected view.
     */
    VLNV getHierarchicalDesignVLNV(QSharedPointer<Component> containingComponent,
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
        QVector<QSharedPointer<Cpu> >& connectedCPUs, QSharedPointer<const Design> design,
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
