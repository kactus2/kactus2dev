//-----------------------------------------------------------------------------
// File: IGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 27.6.2012
//
// Description:
// Interface for plugins that are used to generate content for different IP-XACT documents.
//-----------------------------------------------------------------------------

#ifndef IGENERATORPLUGIN_H
#define IGENERATORPLUGIN_H

#include "IPlugin.h"

#include <QSharedPointer>
#include <QIcon>

class Component;
class Design;
class DesignConfiguration;
class IPluginUtility;

//-----------------------------------------------------------------------------
//! Generator plugins can be used in the component editor and design editors to generate content for the currently
//! active document. The content can //! be, e.g., new source files or direct modifications to the IP-XACT
//! metadata of the document. Plugin generators will automatically appear in the Kactus2 ribbon menu under
//! the Generation group.
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
     *  Checks whether the generator may run for the given component or design. 
     *
     *      @param [in] component	        The component for which to check support. If design is not null, component
	 *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     *
     *      @return True, if the generator may run the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Component const> component,
        QSharedPointer<Design const> design,
        QSharedPointer<DesignConfiguration const> designConfiguration) const = 0;

    /*!
     *  Runs the generation, creating new files and/or modifying the IP-XACT metadata. The function has
	 *  also access to the parent window widget, so that it can show dialogs for the user to aid the generation.
     *
     *      @param [in] utility			    The plugin utility interface.
     *      @param [in] component	        The component for which to check support. If design is not null, component
     *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     */
    virtual void runGenerator(IPluginUtility* utility, 
        QSharedPointer<Component> component,
        QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConfiguration) = 0;
};

Q_DECLARE_INTERFACE(IGeneratorPlugin, "com.tut.Kactus2.IGeneratorPlugin/1.0")

//-----------------------------------------------------------------------------

#endif // IGENERATORPLUGIN_H
