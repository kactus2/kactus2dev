//-----------------------------------------------------------------------------
// File: CLIConsole.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "CLIConsole.h"

#include <QApplication>

#include <string.h>

namespace
{
    const std::string ps1 = ">>> ";
    const std::string ps2 = "... ";
};

//-----------------------------------------------------------------------------
// Function: CLIConsole::CLIConsole()
//-----------------------------------------------------------------------------
CLIConsole::CLIConsole(QObject* parent) : 
    QObject(parent), 
    globalContext_(nullptr), 
    localContext_(nullptr),
    inputBuffer_(),
    runMultiline_(false)
{
   
}

//-----------------------------------------------------------------------------
// Function: CLIConsole::~CLIConsole()
//-----------------------------------------------------------------------------
CLIConsole::~CLIConsole()
{
    Py_XDECREF(globalContext_);
    Py_XDECREF(localContext_);

    Py_FinalizeEx();
}

//-----------------------------------------------------------------------------
// Function: CLIConsole::initialize()
//-----------------------------------------------------------------------------
bool CLIConsole::initialize()
{
    std::string appName = QApplication::applicationName().toStdString();
    wchar_t *program = Py_DecodeLocale(appName.data(), NULL);
    if (program == NULL) {
        //fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        return false;

    }

    Py_SetProgramName(program); 
    Py_Initialize();

#ifdef Q_OS_WIN
    m_notifier = new QWinEventNotifier(GetStdHandle(STD_INPUT_HANDLE), this);
    connect(m_notifier, &QWinEventNotifier::activated, this, &CLIConsole::readCommand);

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
        std::cout << "Starting Python interpreter.\n" <<
            "Python " << Py_GetVersion() << "\n";        
        std::cout << ">>> ";
    }

    localContext_ = PyDict_New();
    globalContext_ = PyDict_New();
    PyDict_SetItemString(globalContext_, "__builtins__", PyEval_GetBuiltins());

    return success;
}

//-----------------------------------------------------------------------------
// Function: CLIConsole::execute()
//-----------------------------------------------------------------------------
void CLIConsole::execute(std::string const& line)
{
    if (std::cin.eof())                          /* Ctrl-D pressed */
    {
        emit quit();
        return;
    }

    if (line.empty() && inputBuffer_.empty())
    {
        std::cout << ps1;
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

    if (runMultiline_)
    {
        std::cout << ps2;
    }
    else
    {
        std::cout << ps1;
    }    
}

//-----------------------------------------------------------------------------
// Function: CLIConsole::readCommand()
//-----------------------------------------------------------------------------
void CLIConsole::readCommand()
{
   std::string line;

    std::getline(std::cin, line);

    if (std::cin.eof())
    {        
        emit quit();
    }
    else
    {
        execute(line);
    }
    
    return;
  
}

//-----------------------------------------------------------------------------
// Function: CLIConsole::writeCommand()
//-----------------------------------------------------------------------------
void CLIConsole::writeCommand()
{
   // std::cout << process_->readAllStandardOutput().toStdString();
}
