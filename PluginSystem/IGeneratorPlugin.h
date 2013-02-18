//-----------------------------------------------------------------------------
// File: IGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// Interface for plugins that are used to generate content for different
// IP-XACT documents.
//-----------------------------------------------------------------------------

#ifndef IGENERATORPLUGIN_H
#define IGENERATORPLUGIN_H

#include "IPlugin.h"

#include <QSharedPointer>
#include <QWidget>
#include <QIcon>

class LibraryInterface;
class LibraryComponent;
class IPluginUtility;

//-----------------------------------------------------------------------------
//! Interface for plugins that are used to generate content for different
//! IP-XACT documents.
//-----------------------------------------------------------------------------
class IGeneratorPlugin : public IPlugin
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IGeneratorPlugin() {}

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const = 0;

    /*!
     *  Checks whether the generator supports generation for the given library component.
     *
     *      @param [in] libComp The library component for which to check support.
     *
     *      @return True, if the generator supports the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp) const = 0;

    /*!
     *  Runs the generator.
     *
     *      @param [in]     utility       The plugin utility interface.
     *      @param [in,out] libComp       The library component for which the generator is run.
     */
    virtual void runGenerator(IPluginUtility* utility,
                              QSharedPointer<LibraryComponent> libComp) = 0;
};

Q_DECLARE_INTERFACE(IGeneratorPlugin, "com.tut.Kactus2.IGeneratorPlugin/1.0")

//-----------------------------------------------------------------------------

#endif // IGENERATORPLUGIN_H
