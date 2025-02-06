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
// Common interface for writer factories of generators.
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
    virtual QSharedPointer<GenerationOutput> prepareComponent(QString const& outputPath,
        QSharedPointer<MetaComponent> component) = 0;

    /*!
     *  Creates writers for the given meta designs.
     *
     *      @param [in] designs             The designs that will yield documents.
     *
     *      @return The objects that bundles the writers. Will be null, if could not be created.
     */
    virtual QList<QSharedPointer<GenerationOutput> > prepareDesign(QList<QSharedPointer<MetaDesign> >& designs) = 0;
    
    /*!
     *  Returns the language of the factory.
     */
    virtual QString getLanguage() const = 0;
    
    /*!
     *  Returns true, if it is desirable to save the output to file set by default.
     */
    virtual bool getSaveToFileset() const = 0;
    
    /*!
     *  Returns the group identifier suitable for the produced files.
     */
    virtual QString getGroupIdentifier() const = 0;
};

#endif // IWRITERFACTORY_H
