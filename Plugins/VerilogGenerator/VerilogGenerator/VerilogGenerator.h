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
#include <Plugins/common/HDLParser/HDLParserCommon.h>

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
	 */
    VerilogGenerator(LibraryInterface* library, bool useInterfaces, bool generateMemory);

    //! The destructor.
    ~VerilogGenerator();

    /*!
     *  Parses a given component for generation.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] component           The component to parse for generation.
     *      @param [in] topComponentView    The component view to parse for generation.
     *
     *      @remark If parsing is not called before generation, nothing is generated.
     */
    void parseComponent(QString const& outputPath, QSharedPointer<GenerationComponent> gc);

    /*!
     *  Parses a given design for generation.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] component           The component of the design.
     *      @param [in] topComponentView    The component view to parse for generation.
	 *      @param [in] design              The design to parse for generation.
	 *      @param [in] designConf          The design configuration to parse for generation.
     *
     *      @remark If parsing is not called before generation, nothing is generated.
     */
    void parseDesign(QString const& outputPath, QList<QSharedPointer<GenerationDesign> >& designs);

    void createDesignWriters(QSharedPointer<GenerationDesign> design, QSharedPointer<VerilogDocument> document);

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
     *  Generates the component Verilog to a given file.
     *
	 *      @param [in] kactusVersion		The version of the current Kactus build.
	 *      @param [in] generatorVersion	The current version of the generator.
     *
     *      @remark If parse() is not called before generate(), nothing is generated.
     */
	void generate(QString const& generatorVersion,
        QString const& kactusVersion) const;

    QSharedPointer<QList<QSharedPointer<VerilogDocument> > > getDocuments(){return documents_;}

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
     *  Initializes writers for parsing.
     */
	QSharedPointer<VerilogDocument> initializeWriters(QSharedPointer<GenerationComponent> topComponent);

    /*!
    *  Checks if the generator should write nothing.
    *
    *      @return True, if the generator has nothing to write, otherwise false.
    */
    bool nothingToWrite() const;
   
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
    QSharedPointer<Writer> createHeaderWriterForInstance(QSharedPointer<GenerationInstance> instance) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component library.
    LibraryInterface* library_;

    //! True, if interfaces are utilized separately from physical ports, else false.
    bool useInterfaces_;

    //! If true, definitions for registers are generated as well, else it is false.
    bool generateMemory_;

    //! Sorter for component ports.
    QSharedPointer<PortSorter> sorter_;

    //! Documents that are ready for writing
    QSharedPointer<QList<QSharedPointer<VerilogDocument> > > documents_;
};

#endif // VERILOGGENERATOR_H
