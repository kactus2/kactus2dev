//-----------------------------------------------------------------------------
// File: PluginUtilityAdapter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.7.2014
//
// Description:
// Generic utility for passing parent window and library interface for generator plugins.
//-----------------------------------------------------------------------------

#ifndef PLUGINUTILITYADAPTER_H
#define PLUGINUTILITYADAPTER_H

#include "IPluginUtility.h"

#include <QObject>
#include <QWidget>

class LibraryInterface;

//-----------------------------------------------------------------------------
// class PluginUtilityAdapter.
//-----------------------------------------------------------------------------
class PluginUtilityAdapter : public QObject, public IPluginUtility
{
	Q_OBJECT

public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent object.
	 */
	PluginUtilityAdapter(LibraryInterface* libraryInterface, QWidget* parentWidget, QObject* parent = 0);

	/*!
	 *  The destructor.
	*/
	virtual ~PluginUtilityAdapter();

    /*!
     *  Prints an error message to the console window.
     *
     *      @param [in] message The error message to print.
     *
     *      @remark  Requires signal errorMessage to be connected by the user.
     */
    virtual void printError(QString const& message);

    /*!
     *  Prints an info message to the console window.
     *
     *      @param [in] message The info message to print.
     *
     *      @remark  Requires signal infoMessage to be connected by the user.
     */
    virtual void printInfo(QString const& message);

    //! Returns the library interface.
    virtual LibraryInterface* getLibraryInterface();

    //! Returns the parent widget to be used for e.g. dialogs.
    virtual QWidget* getParentWidget();

signals:

    //! Emitted when an error message should be printed.
    void errorMessage(QString const& message);

    //! Emitted when an info message should be printed.
    void infoMessage(QString const& message);

private:
    // Disable copying.
    PluginUtilityAdapter(PluginUtilityAdapter const& rhs);

    // Disable assignment.
    PluginUtilityAdapter& operator=(PluginUtilityAdapter const& rhs);

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The library interface to use.
    LibraryInterface* libraryInterface_;

    //! The parent widget to use.
    QWidget* parentWidget_;
};
#endif // PLUGINUTILITYADAPTER_H
