//-----------------------------------------------------------------------------
// File: PythonInterpreter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2021
//
// Description:
// Convenience class for accessing Python interpreter.
//-----------------------------------------------------------------------------
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "PythonInterpreter.h"

#include <PythonAPI/extensions/OutputForwarder.h>
#include <PythonAPI/PythonAPI.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::PythonInterpreter()
//-----------------------------------------------------------------------------
PythonInterpreter::PythonInterpreter(WriteChannel* outputChannel, WriteChannel* errorChannel, QObject* parent) :
    QObject(parent), 
    WriteChannel(),
    inputBuffer_(),
    runMultiline_(false),
    outputChannel_(outputChannel),
    errorChannel_(errorChannel),
    globalContext_(nullptr),
    localContext_(nullptr)

{
   
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::~PythonInterpreter()
//-----------------------------------------------------------------------------
PythonInterpreter::~PythonInterpreter()
{
    Py_DECREF(globalContext_);
    Py_DECREF(localContext_);
    Py_FinalizeEx();
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::initialize()
//-----------------------------------------------------------------------------
bool PythonInterpreter::initialize()
{
    PyImport_AppendInittab("OutputForwarder", &PyInit_OutputForwarder);

    Py_InitializeEx(0); //<! Disable signals.

    
    if (Py_IsInitialized() == false)
    {        
        errorChannel_->write(QStringLiteral("Could not initialize Python interpreter."));
        return false;
    }

    outputChannel_->write(QString("Python ") + QString(Py_GetVersion()) + QString("\n"));    

    localContext_ = PyDict_New();
    globalContext_ = PyDict_New();
    PyDict_SetItemString(globalContext_, "__builtins__", PyEval_GetBuiltins());

    if (setOutputChannels() == false)
    {
        return false;
    }

    setAPI();


    printPrompt();
    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::write()
//-----------------------------------------------------------------------------
void PythonInterpreter::write(QString const& command)
{
    execute(command.toStdString());
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::execute()
//-----------------------------------------------------------------------------
void PythonInterpreter::execute(std::string const& command)
{
    Q_ASSERT_X(Py_IsInitialized(), "Python interpreter", "Trying to execute without initializing.");

    if (command.empty() && inputBuffer_.empty())
    {
        printPrompt();
        return;
    }    

    inputBuffer_.append(command);
    inputBuffer_.push_back('\n');

    auto inputSize = inputBuffer_.length();
    bool endMultiline = runMultiline_ && inputSize >= 2 && inputBuffer_.at(inputSize - 2) == '\n';

    PyObject* src = Py_CompileString(inputBuffer_.c_str(), "<stdin>", Py_single_input);

    /* compiled just fine - */
    if (src != nullptr)
    {
        /* ">>> " or "... " and double '\n' */
        if (runMultiline_ == false || endMultiline)
        {                                               /* so execute it */
            PyObject* dum = PyEval_EvalCode(src, globalContext_, localContext_);
            Py_XDECREF(dum);
            Py_XDECREF(src);

            if (PyErr_Occurred())
            {
                PyErr_Print();
            }                

            runMultiline_ = false;
            inputBuffer_.clear();
        }
        else
        {
            runMultiline_ = true;
        }
    }     
    /* syntax error or E_EOF? */
    else if (PyErr_ExceptionMatches(PyExc_SyntaxError))
    {
        PyObject* exc = nullptr;
        PyObject* val = nullptr;
        PyObject* trb = nullptr;
        PyObject* obj = nullptr;

        char *msg = nullptr;

        /* clears exception! */
        PyErr_Fetch(&exc, &val, &trb);  

        if (PyArg_ParseTuple(val, "sO", &msg, &obj) && !strcmp(msg, "unexpected EOF while parsing") && 
            endMultiline == false) /* E_EOF */
        {            
            Py_XDECREF(exc);
            Py_XDECREF(val);
            Py_XDECREF(trb);

            runMultiline_ = true;
        }
        else /* some other syntax error */
        {
            PyErr_Restore(exc, val, trb);
            PyErr_Print();

            runMultiline_ = false;
            inputBuffer_.clear();
        }
    }
    else /* some non-syntax error */
    {
        PyErr_Print();
        runMultiline_ = false;
        inputBuffer_.clear();
    }

    printPrompt();
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::setOutputChannels()
//-----------------------------------------------------------------------------
bool PythonInterpreter::setOutputChannels()
{
    PyObject* OutputForwarderName = PyUnicode_FromString("OutputForwarder");
    if (OutputForwarderName == NULL)
    {
        return false;
    }

    PyObject* OutputForwarderModule = PyImport_Import(OutputForwarderName);
    Py_DECREF(OutputForwarderName);
    if (OutputForwarderModule == NULL)
    {
        return false;
    }

    PyObject* dict = PyModule_GetDict(OutputForwarderModule);
    Py_DECREF(OutputForwarderModule);

    if (dict == NULL) {
        PyErr_Print();
        errorChannel_->write(QStringLiteral("Fails to get the dictionary.\n"));
        return 1;
    }

    PyObject* python_class = PyDict_GetItemString(dict, "OutputForwarder");

    if (python_class == NULL) {
        PyErr_Print();
        errorChannel_->write(QStringLiteral("Fails to get the Python class.\n"));
        return 1;
    }

    PyObject* outCatcher = nullptr;
    PyObject* errCatcher = nullptr;

    // Creates an instance of the class
    if (PyCallable_Check(python_class))
    {
        outCatcher = PyObject_CallObject(python_class, nullptr);
        errCatcher = PyObject_CallObject(python_class, nullptr);
    }
    else
    {
        outputChannel_->write(QStringLiteral("Cannot instantiate the Python class"));

        return false;
    }

    ((OutputForwarderObject *)outCatcher)->channel = outputChannel_;
    if (PySys_SetObject("stdout", outCatcher) < 0)
    {
        return false;
    }

    ((OutputForwarderObject *)errCatcher)->channel = errorChannel_;
    if (PySys_SetObject("stderr", errCatcher) < 0)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::setAPI()
//-----------------------------------------------------------------------------
bool PythonInterpreter::setAPI()
{
 
    PyObject* emptyList = PyList_New(0);
    PyObject* apiModule = PyImport_ImportModuleEx("pythonAPI", globalContext_, localContext_, emptyList);
    Py_XDECREF(emptyList);

    if (apiModule == NULL)
    {
        errorChannel_->write(QStringLiteral("Could not import Kactus2 pythonAPI."));
        PyErr_Print();
    }
    else
    {
        PyObject* dict = PyModule_GetDict(apiModule);

        if (dict == NULL) {
            PyErr_Print();
            errorChannel_->write(QStringLiteral("Fails to get the dictionary for pythonAPI.\n"));
            return false;
        }

        PyObject* python_class = PyDict_GetItemString(dict, "PythonAPI");

        if (python_class == NULL) {
            PyErr_Print();
            errorChannel_->write(QStringLiteral("Fails to get the Python class PythonAPI.\n"));
            return false;
        }

        PyObject* apiObject;
        // Creates an instance of the class
        if (PyCallable_Check(python_class))
        {
            apiObject = PyObject_CallObject(python_class, nullptr);
            PyDict_SetItemString(localContext_, "kactus2", apiObject);
        }

    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::printPrompt()
//-----------------------------------------------------------------------------
void PythonInterpreter::printPrompt() const
{
    if (runMultiline_)
    {
        outputChannel_->write("... ");
    }
    else
    {
        outputChannel_->write(">>> ");
    }
}

