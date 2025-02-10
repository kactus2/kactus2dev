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
class MessageMediator;

//-----------------------------------------------------------------------------
// class PluginUtilityAdapter.
//-----------------------------------------------------------------------------
class PluginUtilityAdapter : public IPluginUtility
{
public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] libraryInterface    The available library of IP-XACT objects.
     *      @param [in] messageChannel      The channel for displaying messages to the user.
     *      @param [in] versionString       The current version of the Kactus2 build.
     *      @param [in] parentWidget        The parent widget.
     *      @param [in] parent              The parent object.
	 */
	PluginUtilityAdapter(LibraryInterface* libraryInterface, MessageMediator* messageChannel,
        QString const& versionString, QWidget* parentWidget = 0);

	/*!
	 *  The destructor.
	*/
	virtual ~PluginUtilityAdapter() = default;

    // Disable copying.
    PluginUtilityAdapter(PluginUtilityAdapter const& rhs) =delete;

    // Disable assignment.
    PluginUtilityAdapter& operator=(PluginUtilityAdapter const& rhs) = delete;

    //! Returns the library interface.
    virtual LibraryInterface* getLibraryInterface() override final;

    //! Returns the parent widget to be used for e.g. dialogs.
    virtual QWidget* getParentWidget() override final;

	//! Returns the current version of the Kactus2 build.
	virtual QString getKactusVersion() const override final;
 
    /*!
     *  Prints an error message to the console window.
     *
     *      @param [in] message The error message to print.
     *
     *      @remark  Requires signal errorMessage to be connected by the user.
     */
    virtual void printError(QString const& message) override final;

    /*!
     *  Prints an info message to the console window.
     *
     *      @param [in] message The info message to print.
     *
     *      @remark  Requires signal infoMessage to be connected by the user.
     */
    virtual void printInfo(QString const& message) override final;

private:

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The library interface to use.
	LibraryInterface* libraryInterface_;

    //! The channel for displaying messages to the user.
    MessageMediator* messageChannel_;

	//! The parent widget to use.
	QWidget* parentWidget_;

	//! The string denoting the current Kactus version.
	QString versionString_;
};
#endif // PLUGINUTILITYADAPTER_H
