//-----------------------------------------------------------------------------
// File: ModelSimWriterFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.04.2017
//
// Description:
// Creates writers for generating do files.
//-----------------------------------------------------------------------------

#ifndef MODELSIMFACTORY_H
#define MODELSIMFACTORY_H

#include "ModelSimDocument.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IWriterFactory.h>

struct GenerationSettings;

//-----------------------------------------------------------------------------
// Creates writers for generating do files.
//-----------------------------------------------------------------------------
class ModelSimWriterFactory : public IWriterFactory
{
public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] library                 The component library.
     *    @param [in] messages                For message output.
     *    @param [in] settings                Points to the settings to be used within the generation.
	 *    @param [in] kactusVersion		    The version of the current Kactus build.
	 *    @param [in] generatorVersion	    The current version of the generator.
	 */
    ModelSimWriterFactory(LibraryInterface* library, 
        MessageMediator* messages, GenerationSettings* settings,
        QString const& kactusVersion, QString const& generatorVersion);

    // Disable copying.
    ModelSimWriterFactory(ModelSimWriterFactory const& rhs) = delete;
    ModelSimWriterFactory& operator=(ModelSimWriterFactory const& rhs) = delete;

    //! The destructor.
    virtual ~ModelSimWriterFactory() = default;

    /*!
     *  Creates writers for the given formatted HDL component.
     *
     *    @param [in] outputPath			The path to the output file.
     *    @param [in] component           The component which is needs writing.
     *
     *    @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QSharedPointer<GenerationOutput> prepareComponent(QString const& outputPath,
        QSharedPointer<MetaComponent> component);

    /*!
     *  Creates writers for the given parsed meta design.
     *
     *    @param [in] design             The design which is needs writing.
     *
     *    @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QList<QSharedPointer<GenerationOutput> > prepareDesign(QList<QSharedPointer<MetaDesign> >& designs);
    
    /*!
     *  Returns the language of the factory.
     */
    virtual QString getLanguage() const;
    
    /*!
     *  Returns true, if it is desirable to save the output to file set by default.
     */
    virtual bool getSaveToFileset() const;
    
    /*!
     *  Returns the group identifier suitable for the produced files.
     */
    virtual QString getGroupIdentifier() const;

private:

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
};

#endif // MODELSIMFACTORY_H
