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

#include "../IPlugin.h"

#include <QSharedPointer>
#include <QWidget>
#include <QIcon>

class LibraryInterface;
class Document;
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
     *      @param [in] libComp		The component for which to check support. If libDes is not null, libComp
	 *                              must refer to libDes or libDesConf.
     *      @param [in] libDesConf	The design configuration for design libDes, if it is not null.
     *      @param [in] libDes	    The design, if the generator is ran for a design.
     *
     *      @return True, if the generator may run the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Document const> libComp,
		QSharedPointer<Document const> libDesConf = QSharedPointer<Document const>(),
		QSharedPointer<Document const> libDes = QSharedPointer<Document const>()) const = 0;

    /*!
     *  Runs the generation, creating new files and/or modifying the IP-XACT metadata. The function has
	 *  also access to the parent window widget, so that it can show dialogs for the user to aid the generation.
     *
     *      @param [in]			utility			The plugin utility interface.
     *      @param [in,out]     libComp			The component for which the generator is run. If libDes is not null,
	 *                                          libComp must refer to libDes or libDesConf.
     *      @param [in, out]	libDesConf		The design configuration for design libDes, if it is not null.
     *      @param [in, out]	libDes			The design, if the generator is ran for a design.
     */
    virtual void runGenerator(IPluginUtility* utility,
                              QSharedPointer<Document> libComp,
							  QSharedPointer<Document> libDesConf = QSharedPointer<Document>(),
							  QSharedPointer<Document> libDes = QSharedPointer<Document>()) = 0;
};

Q_DECLARE_INTERFACE(IGeneratorPlugin, "com.tut.Kactus2.IGeneratorPlugin/1.0")

//-----------------------------------------------------------------------------

#endif // IGENERATORPLUGIN_H
