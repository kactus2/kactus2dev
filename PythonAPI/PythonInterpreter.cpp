//-----------------------------------------------------------------------------
// File: PythonInterpreter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "PythonInterpreter.h"

#include <QApplication>

#include <string.h>

//#include <PythonAPI/OutputRedirector.cpp>

namespace
{
    const std::string ps1 = ">>> ";
    const std::string ps2 = "... ";
};

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::PythonInterpreter()
//-----------------------------------------------------------------------------
PythonInterpreter::PythonInterpreter(WriteChannel* outputChannel, WriteChannel* errorChannel, QObject* parent) :
    QObject(parent), 
    WriteChannel(),
    globalContext_(nullptr), 
    localContext_(nullptr),
    inputBuffer_(),
    outputChannel_(outputChannel),
    errorChannel_(errorChannel),
    runMultiline_(false)
{
   
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::~PythonInterpreter()
//-----------------------------------------------------------------------------
PythonInterpreter::~PythonInterpreter()
{
    Py_XDECREF(globalContext_);
    Py_XDECREF(localContext_);

    Py_FinalizeEx();
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::initialize()
//-----------------------------------------------------------------------------
bool PythonInterpreter::initialize()
{
    std::string appName = QApplication::applicationName().toStdString();
    wchar_t *program = Py_DecodeLocale(appName.data(), NULL);
    if (program == NULL) {
        //fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        return false;

    }

    Py_SetProgramName(program); 
    //PyImport_AppendInittab("emb", &PyInit_emb);
    //PyImport_AppendInittab("StandardOutputCapture", &PyInit_StandardOutputCapture);
    Py_Initialize();
    

#ifdef Q_OS_WIN
    //m_notifier = new QWinEventNotifier(GetStdHandle(STD_INPUT_HANDLE), this);
    //connect(m_notifier, &QWinEventNotifier::activated, this, &PythonInterpreter::readCommand);

#else
    m_notifier = new QSocketNotifier(_fileno(stdin), QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, SLOT(readCommand()));
#endif

    //m_notifier->setEnabled(true);

    //connect(process_, SIGNAL(readyReadStandardOutput()), this, SLOT(writeCommand()));

    PyMem_RawFree(program);

    bool success = Py_IsInitialized();
    if (success)
    {        
        outputChannel_->write(QString("Python ") + QString(Py_GetVersion()) + QString("\n"));
        printPrompt();
    }

    localContext_ = PyDict_New();
    globalContext_ = PyDict_New();
    PyDict_SetItemString(globalContext_, "__builtins__", PyEval_GetBuiltins());


   /* PyObject* customName = PyUnicode_FromString("StandardOutputCapture");
    if (customName == NULL)
    {
        return false;
    }

    PyObject* customModule = PyImport_Import(customName);

    PyObject* dict = PyModule_GetDict(customModule);
    if (dict == nullptr) {
        PyErr_Print();
        std::cerr << "Fails to get the dictionary.\n";
        return 1;
    }
    Py_DECREF(customModule);



    PyObject* python_class = PyDict_GetItemString(dict, "StandardOutputCapture");
    if (python_class == nullptr) {
        PyErr_Print();
        std::cerr << "Fails to get the Python class.\n";
        return 1;
    }
    Py_DECREF(dict);

    PyObject* catcher = nullptr;

    // Creates an instance of the class
    if (PyCallable_Check(python_class)) {
         catcher = PyObject_CallObject(python_class, nullptr);
        Py_DECREF(python_class);
    }
    else {
        std::cout << "Cannot instantiate the Python class" << std::endl;
        Py_DECREF(python_class);
        return 1;
    }


    if (PySys_SetObject("stdout", catcher) < 0)
    {
        return false;
    }*/

    return success;
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::write()
//-----------------------------------------------------------------------------
void PythonInterpreter::write(QString const& text)
{
    execute(text.toStdString());
}

//-----------------------------------------------------------------------------
// Function: PythonInterpreter::execute()
//-----------------------------------------------------------------------------
void PythonInterpreter::execute(std::string const& line)
{
    if (line.empty() && inputBuffer_.empty())
    {
        printPrompt();
        return;
    }    

    inputBuffer_.append(line);
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
// Function: PythonInterpreter::printPrompt()
//-----------------------------------------------------------------------------
void PythonInterpreter::printPrompt() const
{
    if (runMultiline_)
    {
        outputChannel_->write(QString::fromStdString(ps2));
    }
    else
    {
        outputChannel_->write(QString::fromStdString(ps1));
    }
}
