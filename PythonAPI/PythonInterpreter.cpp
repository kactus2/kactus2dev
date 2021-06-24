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

#include <PythonAPI/extensions/IOCatcher.h>
#include <PythonAPI/PythonAPI.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::PythonInterpreter()
//-----------------------------------------------------------------------------
PythonInterpreter::PythonInterpreter(ReadChannel* inputChannel, WriteChannel* outputChannel, WriteChannel* errorChannel,
    bool interactive, QObject* parent) :
    QObject(parent), 
    WriteChannel(),
    inputBuffer_(),
    interactive_(interactive),
    runMultiline_(false),
    inputChannel_(inputChannel),
    outputChannel_(outputChannel),
    errorChannel_(errorChannel),
    globalContext_(nullptr),
    localContext_(nullptr),
    threadState_(nullptr)
{
   
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::~PythonInterpreter()
//-----------------------------------------------------------------------------
PythonInterpreter::~PythonInterpreter()
{
    PyEval_AcquireThread(threadState_);
    Py_FinalizeEx();
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::initialize()
//-----------------------------------------------------------------------------
bool PythonInterpreter::initialize()
{
    PyImport_AppendInittab("IOCatcher", &PyInit_IOCatcher);

    Py_InitializeEx(0); //<! Disable signals.
    
    if (Py_IsInitialized() == false)
    {        
        errorChannel_->write(QStringLiteral("Could not initialize Python interpreter."));
        return false;
    }

    threadState_ = Py_NewInterpreter();

    outputChannel_->write(QString("Python ") + QString(Py_GetVersion()) + QString("\n"));    

    PyObject *module = PyImport_ImportModule("__main__");
    localContext_ = PyModule_GetDict(module);
    globalContext_ = localContext_;

    if (setOutputChannels() == false)
    {
        return false;
    }

    setAPI();

    PyEval_ReleaseThread(threadState_);

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
// Function: PythonInterpreter::runFile()
//-----------------------------------------------------------------------------
void PythonInterpreter::runFile(QString const& filePath)
{
    Q_ASSERT_X(Py_IsInitialized(), "Python interpreter", "Trying to execute file without initializing.");


    QFile scriptFile(filePath);
    if (scriptFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        int fd = scriptFile.handle();
        FILE* f = fdopen(dup(fd), "rb");

        PyEval_AcquireThread(threadState_);
        PyRun_SimpleFile(f, scriptFile.fileName().toLocal8Bit());
        PyEval_ReleaseThread(threadState_);

        fclose(f);

        scriptFile.close();

        printPrompt();
    }

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


    PyEval_AcquireThread(threadState_);

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

    PyEval_ReleaseThread(threadState_);

    printPrompt();
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::setOutputChannels()
//-----------------------------------------------------------------------------
bool PythonInterpreter::setOutputChannels()
{
    PyObject* IOCatcherName = PyUnicode_FromString("IOCatcher");
    if (IOCatcherName == NULL)
    {
        return false;
    }

    PyObject* IOCatcherModule = PyImport_Import(IOCatcherName);
    Py_DECREF(IOCatcherName);
    if (IOCatcherModule == NULL)
    {
        return false;
    }

    PyObject* dict = PyModule_GetDict(IOCatcherModule);
    Py_DECREF(IOCatcherModule);

    if (dict == NULL) 
    {
        PyErr_Print();
        errorChannel_->write(QStringLiteral("Fails to get the output dictionary.\n"));
        return false;
    }

    PyObject* python_class = PyDict_GetItemString(dict, "OutputCatcher");
    if (python_class == NULL) 
    {
        PyErr_Print();
        errorChannel_->write(QStringLiteral("Fails to get the output Python class.\n"));
        return false;
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
        outputChannel_->write(QStringLiteral("Cannot instantiate the Python class for output"));
        return false;
    }

    ((OutputCatcherObject*)outCatcher)->channel = outputChannel_;
    if (PySys_SetObject("stdout", outCatcher) < 0)
    {
        return false;
    }

    ((OutputCatcherObject*)errCatcher)->channel = errorChannel_;
    if (PySys_SetObject("stderr", errCatcher) < 0)
    {
        return false;
    }


    PyObject* input_python_class = PyDict_GetItemString(dict, "InputCatcher");

    if (input_python_class == NULL) 
    {
        PyErr_Print();
        errorChannel_->write(QStringLiteral("Fails to get the input Python class.\n"));
        return false;
    }

    PyObject* inputBuffer;

    // Creates an instance of the class
    if (PyCallable_Check(input_python_class))
    {
        inputBuffer = PyObject_CallObject(input_python_class, nullptr);
    }

    if (PySys_SetObject("stdin", inputBuffer) < 0)
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
        errorChannel_->write(QStringLiteral("Could not import Kactus2 PythonAPI.\n"));
        PyErr_Print();
    }
    else
    {
        PyObject* dict = PyModule_GetDict(apiModule);

        if (dict == NULL) {
            PyErr_Print();
            errorChannel_->write(QStringLiteral("Could not import Kactus2 PythonAPI.\n"));
            return false;
        }

        PyObject* python_class = PyDict_GetItemString(dict, "PythonAPI");

        if (python_class == NULL) {
            PyErr_Print();
            errorChannel_->write(QStringLiteral("Could not import Kactus2 PythonAPI.\n"));
            return false;
        }

        // Creates an instance of the class
        if (PyCallable_Check(python_class))
        {
            PyObject* apiObject = PyObject_CallObject(python_class, nullptr);
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
    if (interactive_ == false)
    {
        return;
    }

    if (runMultiline_)
    {
        outputChannel_->write("... ");
    }
    else
    {
        outputChannel_->write(">>> ");
    }
}
