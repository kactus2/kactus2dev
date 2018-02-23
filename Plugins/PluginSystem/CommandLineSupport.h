//-----------------------------------------------------------------------------
// File: CommandLineSupport.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.02.2018
//
// Description:
// Interface for enabling plugin run using the command line arguments.
//-----------------------------------------------------------------------------

#ifndef COMMANDLINESUPPORT_H
#define COMMANDLINESUPPORT_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QCommandLineOption>

class IPluginUtility;

//-----------------------------------------------------------------------------
//! Interface for enabling plugin run using the command line arguments.
//-----------------------------------------------------------------------------
class CommandLineSupport 
{
public:

	//! The destructor.
    virtual ~CommandLineSupport() {};

    /*!
     *  Gets the command required to run the plugin.
     *
     *      @return The command to run the plugin.
     */
    virtual QString getCommand() const = 0;

    /*!
     *  Executes the plugin with the given arguments.
     *
     *      @param [in] arguments   The arguments for the execution.
     *      @param [in] utility     Utilities for enabling plugin execution.
     */
    virtual void process(QStringList const& arguments, IPluginUtility* utility) = 0;
};

#endif // COMMANDLINESUPPORT_H
