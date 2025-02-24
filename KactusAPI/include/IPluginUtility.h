//-----------------------------------------------------------------------------
// File: IPluginUtility.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 28.6.2012
//
// Description:
// Interface which offers core utilities for plugins, namely access to the console, the parent widget, and most
// importantly the library interface. It is provided for a plugin via the implemented plugin interface.
//-----------------------------------------------------------------------------

#ifndef IPLUGINUTILITY_H
#define IPLUGINUTILITY_H

#include <QWidget>
#include <QString>

class LibraryInterface;

class IPluginUtility
{
public:

    /*!
     *  Destructor.
     */
    virtual ~IPluginUtility() {}

    /*!
     *  Returns the library interface.
     */
    virtual LibraryInterface* getLibraryInterface() = 0;

    /*!
     *  Returns the parent widget to be used for e.g. dialogs.
     */
	virtual QWidget* getParentWidget() = 0;

	//! Returns the current version of the Kactus2 build.
	virtual QString getKactusVersion() const = 0;
  
    /*!
     *  Prints an error message to the console window.
     *
     *    @param [in] message The error message to print.
     */
    virtual void printError(QString const& message) = 0;

    /*!
     *  Prints an info message to the console window.
     *
     *    @param [in] message The info message to print.
     */
    virtual void printInfo(QString const& message) = 0;
};

//-----------------------------------------------------------------------------

#endif // IPLUGINUTILITY_H
