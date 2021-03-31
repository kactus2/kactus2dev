//-----------------------------------------------------------------------------
// File: PythonInterpreter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2021
//
// Description:
// Convenience class for accessing Python interpreter.
//-----------------------------------------------------------------------------

#ifndef PYTHON_INTERPRETER_H
#define PYTHON_INTERPRETER_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <QObject>
#include <QFile>

#include <string>

#include <PythonAPI/WriteChannel.h>

//-----------------------------------------------------------------------------
//! Convenience class for accessing Python interpreter.
//-----------------------------------------------------------------------------
class PythonInterpreter : public QObject, public WriteChannel
{
    Q_OBJECT

public:

    /*!
     * The constructor.
     *
     *     @param [in] outputChannel    Channel for standard messages and interpreter output.
     *     @param [in] errorChannel     Channel for error messages.
     *     @param [in] parent           The parent object.
     */
     explicit PythonInterpreter(WriteChannel* outputChannel, WriteChannel* errorChannel, QObject* parent = nullptr);

     // ! The destructor.
    virtual ~PythonInterpreter();


    /*!
     * Initializes the interpterter. This function must be called before writing any commands with write().
     *
     *     @return True, if initialization was successful, otherwise false.
     */
     bool initialize();

    /*!
     * Write a command for interpreter to execute.
     *
     *     @param [in] command  The command to execute.
     */
     virtual void write(QString const& command) override final;

     /*!
      * Run a script from a given file.
      *
      *     @param [in] filePath  Path to the file to run.
      */
      void runFile(QString const& filePath);

    /*!
     * Execute the given line in the interpreter.
     *
     *     @param [in] line  The line to execute.
     */
     void execute(std::string const& line);
   
private:

    /*!
     * Redirect the interpreter output and error output to WriteChannels.
     *
     *     @return True, if the outputs could be redirected, otherwise false.
     */
     bool setOutputChannels();

     /*!
      * Import and set the core api available in python context.
      *
      *     @return True, if the api was set, otherwise false.
      */
      bool setAPI();

    /*!
     * Write prompt to output.
     */
     void printPrompt() const;
  
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Buffer to store multiple lines for command to execute.
    std::string inputBuffer_;

    //! True, if the current command requires multiple lines (e.g. loop).
    bool runMultiline_;

    //! Channel for interpreter output.
    WriteChannel* outputChannel_;

    //! Channel for interpreter errors.
    WriteChannel* errorChannel_; 

    //! The global context for the interpreter.
    PyObject* globalContext_;

    //! The local context for the interpreter when running a command.
    PyObject* localContext_;
};

#endif // PYTHON_INTERPRETER_H