//-----------------------------------------------------------------------------
// File: VerilogWriterFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Creates writes for generating Verilog.
//-----------------------------------------------------------------------------

#ifndef VERILOGWRITERFACTORY_H
#define VERILOGWRITERFACTORY_H

#include "../veriloggeneratorplugin_global.h"

#include "VerilogDocument.h"

#include <QTextStream>
#include <Plugins/common/HDLParser/MetaDesign.h>
#include <Plugins/common/HDLParser/HDLComponentParser.h>

class ComponentVerilogWriter;
class ComponentInstanceVerilogWriter;
class VerilogAssignmentWriter;
class LibraryInterface;
class PortSorter;
class VerilogHeaderWriter;
class VerilogWireWriter;
class CommentWriter;
class Writer;
class WriterGroup;

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VerilogWriterFactory : public QObject
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] library                 The component library.
     *      @param [in] useInterfaces           True, if interfaces are utilized separately from physical ports, else false.
     *      @param [in] generateMemory          If true, definitions for registers are generated as well, else it is false.
	 *      @param [in] kactusVersion		    The version of the current Kactus build.
	 *      @param [in] generatorVersion	    The current version of the generator.
	 */
    VerilogWriterFactory(LibraryInterface* library, bool useInterfaces, bool generateMemory,
        QString const& kactusVersion, QString const& generatorVersion);

    //! The destructor.
    ~VerilogWriterFactory();

    /*!
     *  Prepares a given HDL component for generation, creating writers for the task.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] component           The component to which is prepared for generation.
     *
     *      @return False, if something went wrong.
     */
    QSharedPointer<VerilogDocument> prepareComponent(QString const& outputPath, QSharedPointer<HDLComponentParser> component);

    /*!
     *  Prepares the given set of HDL designs for generation, creating writers for the task.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] designs             The designs to which are prepared for generation.
     *
     *      @remark If parsing is not called before generation, nothing is generated.
     */
    QSharedPointer<VerilogDocument> prepareDesign(QSharedPointer<MetaDesign> design);

signals:
	
    /*!
     *  Emitted when reportable error occurs.
     */
	void reportError(const QString& errorMessage) const;

private:

    // Disable copying.
    VerilogWriterFactory(VerilogWriterFactory const& rhs);
    VerilogWriterFactory& operator=(VerilogWriterFactory const& rhs);

    /*!
    *  Checks if the generator should write nothing.
    *
    *      @return True, if the generator has nothing to write, otherwise false.
    */
    bool nothingToWrite() const;

    /*!
     *  Initializes writers for the given top component.

     *      @return The document, which has writers associated with the component writing.
     */
    QSharedPointer<VerilogDocument> initializeComponentWriters(QSharedPointer<MetaInstance> topComponent);
    
    /*!
    *  Initializes writers for the given design.
     *
	 *      @param [in] design		        The design, which data will be written.
	 *      @param [in] document        	The document, which will get writers for design writing.
     */
    void initializeDesignWriters(QSharedPointer<MetaDesign> design, QSharedPointer<VerilogDocument> document);
   
     /*!
      *  Adds the generated writers to the top writer in correct order.            
      */
    void addWritersToTopInDesiredOrder(QSharedPointer<VerilogDocument> document) const;

    /*!
     *  Creates a writer for writing a preceding header for a component instance.
     *
     *      @param [in] instanceName   The name of the component instance to create the writer for.
     *
     *      @return A writer for the header.
     */
    QSharedPointer<Writer> createHeaderWriterForInstance(QSharedPointer<MetaInstance> instance) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component library.
    LibraryInterface* library_;

    //! True, if interfaces are utilized separately from physical ports, else false.
    bool useInterfaces_;

    //! If true, definitions for registers are generated as well, else it is false.
    bool generateMemory_;

    //! Version of the generator.
    QString generatorVersion_;

    //! Version of Kactus2.
    QString kactusVersion_;

    //! Sorter for component ports.
    QSharedPointer<PortSorter> sorter_;
};

#endif // VERILOGWRITERFACTORY_H
