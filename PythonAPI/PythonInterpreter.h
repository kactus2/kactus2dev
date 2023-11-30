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
     *     @param [in] printPromt       Flag for enabling prompt printing.
     *     @param [in] parent           The parent object.
     */
     explicit PythonInterpreter(WriteChannel* outputChannel, 
         WriteChannel* errorChannel, bool printPromt = true,
         QObject* parent = nullptr);

     // ! The destructor.
    ~PythonInterpreter() final = default;

    /*!
     * Initializes the interpreter. This function must be called before writing any commands with write().
     *
     *     @param [in] interactive  Flag for enabling interactive std input. Set to true on command-line.
     *
     *     @return True, if initialization was successful, otherwise false.
     */
     bool initialize(bool interactive = true);     
     
     /*!
      * Clean up the interpreter state at end of life time.
      */
    void finalize();

    /*!
     * Execute a single line in the interpreter.
     *
     *     @param [in] line  The line to execute.
     */
     void execute(std::string const& line);

signals:
    
    void executeDone();

public slots:

    /*!
     * Write a command for interpreter to execute.
     *
     *     @param [in] command  The command to execute.
     */
    virtual void write(QString const& command) override final;

    /*!
     * Execute the given arbitrary long string in the interpreter.
     *
     *     @param [in] string  The string to execute.
     */
    void executeString(QString const& string);

    /*!
     * Run a script from a given file.
     *
     *     @param [in] filePath  Path to the file to run.
     */
    void runFile(QString const& filePath);

private:

    /*!
     * Redirect the interpreter output and error output to WriteChannels.
     *
     *     @param [in] interactive  Flag for enabling interactive std input. Set to true on command-line.
     *
     *     @return True, if the input and outputs could be redirected, otherwise false.
     */
     bool redirectIO(bool interactive);

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
    std::string inputBuffer_{ };

    //! Flag for enabling prompt printing.
    bool printPrompt_{ true };

    //! True, if the current command requires multiple lines (e.g. loop).
    bool runMultiline_{ false };

    //! Channel for interpreter output.
    WriteChannel* outputChannel_{ nullptr };

    //! Channel for interpreter errors.
    WriteChannel* errorChannel_{ nullptr };

    //! The global context for the interpreter.
    PyObject* globalContext_{ nullptr };

    //! The local context for the interpreter when running a command.
    PyObject* localContext_{ nullptr };

    //! Interpreter thread state value holder.
    PyThreadState* threadState_{ nullptr };
};

#endif // PYTHON_INTERPRETER_H