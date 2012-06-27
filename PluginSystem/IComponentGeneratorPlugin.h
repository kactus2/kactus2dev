//-----------------------------------------------------------------------------
// File: IComponentGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// Interface for plugins that are used to generate content for components.
//-----------------------------------------------------------------------------

#ifndef ICOMPONENTGENERATORPLUGIN_H
#define ICOMPONENTGENERATORPLUGIN_H

#include <QSharedPointer>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Interface for plugins that are used to generate content for components.
//-----------------------------------------------------------------------------
class IComponentGeneratorPlugin
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IComponentGeneratorPlugin() {}

    /*!
     *  Runs the generator.
     *
     *      @param [in,out] component     The component for which the generator is run.
     *      @param [in]     libInterface  The library interface to get access to the library.
     */
    virtual void runComponentGenerator(QSharedPointer<Component> component,
                                       LibraryInterface const* libInterface) = 0;
};

Q_DECLARE_INTERFACE(IComponentGeneratorPlugin, "com.tut.Kactus2.IComponentGeneratorPlugin/1.0")

//-----------------------------------------------------------------------------

#endif // ICOMPONENTGENERATORPLUGIN_H
