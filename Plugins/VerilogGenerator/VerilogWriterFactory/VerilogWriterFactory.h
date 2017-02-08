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

#include "VerilogDocument.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IWriterFactory.h>

class GenerationSettings;

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class VerilogWriterFactory : public IWriterFactory
{
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] library                 The component library.
     *      @param [in] messages                For message output.
     *      @param [in] settings                Points to the settings to be used within the generation.
	 *      @param [in] kactusVersion		    The version of the current Kactus build.
	 *      @param [in] generatorVersion	    The current version of the generator.
	 */
    VerilogWriterFactory(LibraryInterface* library, 
        MessagePasser* messages, GenerationSettings* settings,
        QString const& kactusVersion, QString const& generatorVersion);

    //! The destructor.
    ~VerilogWriterFactory();

    /*!
     *  Creates writers for the given formatted HDL component.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] component           The component which is needs writing.
     *
     *      @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QSharedPointer<GenerationFile> prepareComponent(QString const& outputPath,
        QSharedPointer<MetaComponent> component);

    /*!
     *  Creates writers for the given parsed meta design.
     *
     *      @param [in] design             The design which is needs writing.
     *
     *      @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QSharedPointer<GenerationFile> prepareDesign(QSharedPointer<MetaDesign> design);
    
    /*!
     *  Returns the language of the factory.
     */
    virtual QString getLanguage() const;

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
    QSharedPointer<VerilogDocument> initializeComponentWriters(QSharedPointer<MetaComponent> topComponent);
    
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

    //! The messages.
    MessagePasser* messages_;

    //! Points to the settings to be used within the generation.
    GenerationSettings* settings_;

    //! Version of the generator.
    QString generatorVersion_;

    //! Version of Kactus2.
    QString kactusVersion_;

    //! Sorter for component ports.
    QSharedPointer<PortSorter> sorter_;
};

#endif // VERILOGWRITERFACTORY_H
