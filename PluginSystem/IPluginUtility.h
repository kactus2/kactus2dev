//-----------------------------------------------------------------------------
// File: IPluginUtility.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.6.2012
//
// Description:
// Interface which offers core utilities for plugins.
//-----------------------------------------------------------------------------

#ifndef IPLUGINUTILITY_H
#define IPLUGINUTILITY_H

#include <QWidget>
#include <QString>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Interface which offers core utilities for plugins.
//-----------------------------------------------------------------------------
class IPluginUtility
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IPluginUtility() {}

    /*!
     *  Prints an error message to the console window.
     *
     *      @param [in] message The error message to print.
     */
    virtual void printError(QString const& message) = 0;

    /*!
     *  Prints an info message to the console window.
     *
     *      @param [in] message The info message to print.
     */
    virtual void printInfo(QString const& message) = 0;

    /*!
     *  Returns the library interface.
     */
    virtual LibraryInterface* getLibraryInterface() = 0;

    /*!
     *  Returns the parent widget to be used for e.g. dialogs.
     */
    virtual QWidget* getParentWidget() = 0;
};

//-----------------------------------------------------------------------------

#endif // IPLUGINUTILITY_H
