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

#include <QSharedPointer>

class LibraryInterface;
class Component;
class Cpu;
class AddressSpace;

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
     *      @param [in] masterRoutes    List of the master slave routes that should be examined.
     */
    void writeFile(QString const& outputPath, QSharedPointer<Component> topComponent,
        QVector<QVector<QSharedPointer<const ConnectivityInterface> > >  masterRoutes);

    /*!
     *  Write the CPUs of the selected component.
     *
     *      @param [in] containingComponent     The selected component.
     *      @param [in] outputStream            The output stream.
     */
    void writeCPUs(QSharedPointer<Component> containingComponent, QTextStream& outputStream);

    /*!
     *  Get the address space referenced in the CPU.
     *
     *      @param [in] containingComponent     Component containing the CPU.
     *      @param [in] spaceReference          Name of the address space referenced by the CPU.
     */
    QSharedPointer<AddressSpace> getCPUAddressSpace(QSharedPointer<Component> containingComponent,
        QString const& spaceReference) const;

	void writePaths(QVector<QVector<QSharedPointer<const ConnectivityInterface> > > masterPaths);

    /*!
     *  Write the selected interface path.
     *
     *      @param [in] path    The selected interface path.
     */
    void writePath(QVector<QSharedPointer<const ConnectivityInterface> > path);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connectivity graph factory.
    ConnectivityGraphFactory graphFactory_;
};

#endif // LINUXDEVICETREEGENERATOR_H
