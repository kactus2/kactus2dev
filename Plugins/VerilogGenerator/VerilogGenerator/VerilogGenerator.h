//-----------------------------------------------------------------------------
// File: VerilogGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#ifndef VERILOGGENERATOR_H
#define VERILOGGENERATOR_H

#include "../veriloggeneratorplugin_global.h"

#include <QTextStream>
#include <Plugins/common/HDLParser/MetaDesign.h>

class ComponentVerilogWriter;
class ComponentInstanceVerilogWriter;
class VerilogTiedValueWriter;
class LibraryInterface;
class PortSorter;
class VerilogHeaderWriter;
class VerilogWireWriter;
class CommentWriter;
class Writer;
class WriterGroup;

struct VerilogDocument
{
    //! The name of the file for the document
    QString filePath_;

    //! Writer for generating file header.
    QSharedPointer<VerilogHeaderWriter> headerWriter_;

    //! Writer for the component, is to contain everyting that comes to document, except header writer.
    QSharedPointer<ComponentVerilogWriter> topWriter_;

    //! Writers for Verilog interconnections.
    QSharedPointer<WriterGroup> interconnectionWriters_;

    //! Writers for Verilog wires.
    QSharedPointer<WriterGroup> wireWriters_;

    //! Writer for ad hoc tied values.
    QSharedPointer<VerilogTiedValueWriter> tiedValueWriter_;

    //! Writers for Verilog instances.
    QList<QSharedPointer<ComponentInstanceVerilogWriter> > instanceWriters_;

    //! Header writers for Verilog instances.
    QMap<QSharedPointer<ComponentInstanceVerilogWriter>, QSharedPointer<Writer> > instanceHeaderWriters_;
};

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VerilogGenerator : public QObject
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
    VerilogGenerator(LibraryInterface* library, bool useInterfaces, bool generateMemory, QString const& kactusVersion, QString const& generatorVersion);

    //! The destructor.
    ~VerilogGenerator();

    /*!
     *  Prepares a given HDL component for generation, creating writers for the task.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] component           The component to which is prepared for generation.
     *
     *      @return False, if something went wrong.
     */
    bool prepareComponent(QString const& outputPath, QSharedPointer<MetaInstance> component);

    /*!
     *  Prepares the given set of HDL designs for generation, creating writers for the task.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] designs             The designs to which are prepared for generation.
     *
     *      @remark If parsing is not called before generation, nothing is generated.
     */
    void prepareDesign(QString const& outputPath, QSharedPointer<MetaDesign> design);

    /*!
     *  Parses the module implementation out of verilog file given as output, if it already exists.
     *
	 *      @param [in] outputPath		The path to the output file.
	 *      @param [in] implementation	The module implementation.
	 *      @param [in] postModule		Anything that exists after the module declaration.
	 *
	 *      @return False, if the file exists, but could not be opened or parsed properly.
     */
	bool selectImplementation(QString const& outputPath, QString& implementation,
		QString& postModule) const;
    
    /*!
     *  Writes the Verilog to files.
     *
     *      @remark If prepares are not called before generate(), nothing is generated.
     */
	void write() const;
    
    /*!
     *  Generates Verilog to the given stream.
     */
    void generate(QTextStream& outputStream, QSharedPointer<VerilogDocument> document) const;
    
    /*!
     *  Returns pointer to the list of the documents that are to be written.
     */
    QSharedPointer<QList<QSharedPointer<VerilogDocument> > > getDocuments();

    //! Documents that are ready for writing
    QSharedPointer<QList<QSharedPointer<VerilogDocument> > > documents_;

signals:
	
    /*!
     *  Emitted when reportable error occurs.
     */
	void reportError(const QString& errorMessage) const;

private:

    // Disable copying.
    VerilogGenerator(VerilogGenerator const& rhs);
    VerilogGenerator& operator=(VerilogGenerator const& rhs);

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

#endif // VERILOGGENERATOR_H
