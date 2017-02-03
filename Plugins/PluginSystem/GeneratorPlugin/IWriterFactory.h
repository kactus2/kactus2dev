//-----------------------------------------------------------------------------
// File: IWriterFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 30.01.2017
//
// Description:
// Common interface for writer factories of generators.
//-----------------------------------------------------------------------------

#ifndef IWRITERFACTORY_H
#define IWRITERFACTORY_H

#include <Plugins/common/HDLParser/MetaDesign.h>

//-----------------------------------------------------------------------------
// Verilog file generator.
//-----------------------------------------------------------------------------
class IWriterFactory
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IWriterFactory() {}

    /*!
     *  Creates writers for the given formatted HDL component.
     *
     *      @param [in] outputPath			The path to the output file.
     *      @param [in] component           The component which is needs writing.
     *
     *      @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QSharedPointer<GenerationFile> prepareComponent(QString const& outputPath,
        QSharedPointer<MetaComponent> component) = 0;

    /*!
     *  Creates writers for the given parsed meta design.
     *
     *      @param [in] design             The design which is needs writing.
     *
     *      @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QSharedPointer<GenerationFile> prepareDesign(QSharedPointer<MetaDesign> design) = 0;
    
    /*!
     *  Returns the language of the factory.
     */
    virtual QString getLanguage() const = 0;
};

#endif // IWRITERFACTORY_H
