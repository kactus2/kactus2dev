//-----------------------------------------------------------------------------
// File: CommandLineParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.02.2018
//
// Description:
// Utilities for parsing and executing command line arguments.
//-----------------------------------------------------------------------------

#ifndef COMMANDLINEPARSER
#define  COMMANDLINEPARSER

#include <QStringList>

class LibraryInterface;
class IPluginUtility;

//-----------------------------------------------------------------------------
//! Utilities for parsing and executing command line arguments.
//-----------------------------------------------------------------------------
namespace CommandLineParser
{
    /*!
     *  Checks if help or version option is set.
     *
     *      @param [in] arguments   The list of command line arguments to check.
     *
     *      @return True, if help or verion option is set, otherwise false.
     */
    bool helpOrVersionOptionSet(QStringList const& arguments);

    /*!
     *  Processes the given command line arguments and executes accordingly.
     *
     *      @param [in] arguments   The command line arguments to process.
     *      @param [in] utility     Utilities for library access and user notifications.     
     *
     *      @return 0 for a successful run, any other value for errors.
     */
    int process(QStringList const& arguments, IPluginUtility* utility);
}

#endif 

