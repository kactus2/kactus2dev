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
class MessageMediator;

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
     *    @param [in] arguments   The arguments to read.
     */
    void readArguments(QStringList const& arguments);

    /*!
     *  Checks if the command-line option is set.
     *
     *    @return True, if command-line option is set, otherwise false.
     */
    bool commandlineMode() const;

    /*!
     *	Checks if the run a script mode is set.
     *	    
     *    @return True, if the mode is set, otherwise false.
     */
    bool runScriptMode() const;

    /*!
     *  Processes the given command line arguments and executes accordingly.
     *
     *    @param [in] arguments       The command line arguments to process.
     *    @param [in] messageChannel  Interface for output and error messages.     
     *
     *    @return 0 for a successful run, any other value for errors.
     */
    int process(QStringList const& arguments, MessageMediator* messageChannel);

    /*!
     *	Get the user provided value of given command line option.
     *  
     *    @param [in] option     The option whose value to get.
     *	    
     *    @return The user provide value for given option.
     */
    QString getOptionValue(QString const& option) const;
   
private:

    /*!
     *  Get the usage help text.
     *
     *    @return The usage help text.
     */
    QString helpText();

    // Parser for command-line options.
    QCommandLineParser optionParser_;   

    // Flag to indicate if arguments are read before process();
    bool preReadDone_;
};

#endif 
