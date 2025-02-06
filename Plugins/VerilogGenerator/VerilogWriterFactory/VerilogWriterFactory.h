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

struct GenerationSettings;
class MessageMediator;

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
        MessageMediator* messages, GenerationSettings* settings,
        QString const& kactusVersion, QString const& generatorVersion);

    //! The destructor.
    virtual ~VerilogWriterFactory() = default;

    /*!
     *  Creates writers for the given formatted HDL component.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] component           The component which is needs writing.
     *
     *      @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QSharedPointer<GenerationOutput> prepareComponent(QString const& outputPath,
        QSharedPointer<MetaComponent> component) override final;

    /*!
     *  Creates writers for the given meta designs.
     *
     *      @param [in] designs             The designs that will yield documents.
     *
     *      @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QList<QSharedPointer<GenerationOutput> > prepareDesign(QList<QSharedPointer<MetaDesign> >& designs)
        override final;
    
    /*!
     *  Returns the language of the factory.
     */
    virtual QString getLanguage() const override final;
    
    /*!
     *  Returns true, if it is desirable to save the output to file set by default.
     */
    virtual bool getSaveToFileset() const override final;
    
    /*!
     *  Returns the group identifier suitable for the produced files.
     */
    virtual QString getGroupIdentifier() const override final;

private:

    // Disable copying.
    VerilogWriterFactory(VerilogWriterFactory const& rhs);
    VerilogWriterFactory& operator=(VerilogWriterFactory const& rhs);

    /*!
     *  Checks if given file exists, and tries to parse its existing implementation.
     *
     *      @param [in] document        	    The document, which will get writers for writing design.
     *      @param [in] outputPath        	    The path of the potentially the existing file.
	 *
	 *      @return False, if the file exists, but could not be opened or parsed properly. Else true.
     */
    bool readImplementation(QSharedPointer<VerilogDocument> document, QString const& outputPath);

    /*!
     *  Initializes writers for component.
     *
     *      @param [in] document        	    The document, which will get writers for writing component.
     *      @param [in] component		        The component, which data will be written.
     */
    void initializeComponentWriters(QSharedPointer<VerilogDocument> document, QSharedPointer<MetaComponent> component);
    
    /*!
     *  Initializes writers for the given design.
     *
     *      @param [in] document        	The document, which will get writers for writing design.
	 *      @param [in] design		        The design, which data will be written.
     */
    void initializeDesignWriters(QSharedPointer<VerilogDocument> document, QSharedPointer<MetaDesign> design);
   
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
    MessageMediator* messages_;

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
