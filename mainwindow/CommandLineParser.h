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
#define COMMANDLINEPARSER

#include <QCommandLineParser>
#include <QStringList>

class LibraryInterface;
class IPluginUtility;

//-----------------------------------------------------------------------------
//! Utilities for parsing and executing command line arguments.
//-----------------------------------------------------------------------------
class CommandLineParser
{
public:

    // The constructor.
    CommandLineParser();

    // The destructor.
    ~CommandLineParser() = default;

    /*!
     *  Reads the given arguments. 
     *  The arguments can then be checked before processing with argumentsValid() and helpOrVersionOptionSet().
     *
     *      @param [in] arguments   The arguments to read.
     */
    void readArguments(QStringList const& arguments);

    /*!
     *  Checks if help or version option is set.
     *
     *      @param [in] arguments   The list of command line arguments to check.
     *
     *      @return True, if help or verion option is set, otherwise false.
     */
    bool helpOrVersionOptionSet() const;

    /*!
     *  Processes the given command line arguments and executes accordingly.
     *
     *      @param [in] arguments   The command line arguments to process.
     *      @param [in] utility     Utilities for library access and user notifications.     
     *
     *      @return 0 for a successful run, any other value for errors.
     */
    int process(QStringList const& arguments, IPluginUtility* utility);
   
private:

    /*!
     *  Get the usage help text.
     *
     *      @return The usage help text.
     */
    QString helpText();

    // Parser for command-line options.
    QCommandLineParser optionParser_;   

    // Flag to indicate if arguments are read before process();
    bool preReadDone_;
};

#endif 
