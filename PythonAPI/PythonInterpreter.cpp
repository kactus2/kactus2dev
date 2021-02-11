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

#include "structmember.h"

#include <QApplication>

#include <string.h>


typedef struct
{
    PyObject_HEAD
    WriteChannel* channel;
} CustomObject;

static void Custom_dealloc(CustomObject *self)
{
    ((CustomObject *)self)->channel = NULL;

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* Custom_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CustomObject *self = (CustomObject *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->channel = NULL;
    }

    return (PyObject *)self;
}

static int Custom_init(CustomObject *self, PyObject *args, PyObject *kwds)
{
    return 0;
}

static PyMemberDef Custom_members[] = 
{
    {"channel", T_OBJECT, offsetof(CustomObject, channel), 0,
     "custom output channel"},
    {NULL}  /* Sentinel */
};

static PyObject* Custom_write(CustomObject *self, PyObject * args)
{
    char* s;
    int res = PyArg_ParseTuple(args, "s", &s);
    if (!res)
    {
        PyErr_Print();
    }

    QString text(s);
    if (self->channel != NULL)
    {
        self->channel->write(s);
    }

    return Py_BuildValue("i", text.length());
}

static PyObject* Custom_flush(CustomObject *self, PyObject * args)
{

    if (self->channel != NULL)
    {
        self->channel->write("\n");
    }

    return Py_None;
}

static PyMethodDef Custom_methods[] = 
{
    {"write", (PyCFunction)Custom_write, METH_VARARGS,
    PyDoc_STR("Write")
    },
    {"flush", (PyCFunction)Custom_flush, METH_NOARGS,
    PyDoc_STR("Flush write buffer")
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject CustomType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "custom2.Custom",           /*tp_name*/
    sizeof(CustomObject),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)Custom_dealloc, /*tp_dealloc*/
    0,                          /*tp_vectorcall_offset*/
    0,                          /*tp_getattr*/
    0,                          /*tp_setattr*/
    0,                          /*tp_as_async*/
    0,                          /*tp_repr*/
    0,                          /*tp_as_number*/
    0,                          /*tp_as_sequence*/
    0,                          /*tp_as_mapping*/
    0,                          /*tp_hash */
    0,                          /*tp_call*/
    0,                          /*tp_str*/
    0,                          /*tp_getattro*/
    0,                          /*tp_setattro*/
    0,                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  /*tp_flags*/
    PyDoc_STR(""),              /* tp_doc */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_richcompare */
    0,                          /* tp_weaklistoffset */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    Custom_methods,             /* tp_methods */
    Custom_members,             /* tp_members */
    0,                          /* tp_getset */
 0,//    &PyTextIOBase_Type ,        /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)Custom_init,      /* tp_init */
    0,                          /* tp_alloc */
    Custom_new,                          /* tp_new */
    0,                          /* tp_free */
    0,                          /* tp_is_gc */
    0,                          /* tp_bases */
    0,                          /* tp_mro */
    0,                          /* tp_cache */
    0,                          /* tp_subclasses */
    0,                          /* tp_weaklist */
    0,                          /* tp_del */
    0,                          /* tp_version_tag */
    0,                          /* tp_finalize */
};

static PyModuleDef custommodule =
{
    PyModuleDef_HEAD_INIT,
   "custom2",
    PyDoc_STR("Example module that creates an extension type."),
    -1,
};

PyMODINIT_FUNC PyInit_custom2(void)
{
    PyObject *m;
    if (PyType_Ready(&CustomType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&CustomType);
    if (PyModule_AddObject(m, "Custom", (PyObject *)&CustomType) < 0) {
        Py_DECREF(&CustomType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

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

    PyImport_AppendInittab("custom2", &PyInit_custom2);

    Py_Initialize();

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


    PyObject* customName = PyUnicode_FromString("custom2");
    if (customName == NULL)
    {
        return false;
    }

    PyObject* customModule = PyImport_Import(customName);

    if (customModule == NULL)
    {
        return false;
    }

    PyObject* dict = PyModule_GetDict(customModule);
    if (dict == nullptr) {
        PyErr_Print();
        errorChannel_->write(QStringLiteral("Fails to get the dictionary.\n"));
        return 1;
    }
    Py_DECREF(customModule);



    PyObject* python_class = PyDict_GetItemString(dict, "Custom");
    if (python_class == nullptr) {
        PyErr_Print();
        errorChannel_->write(QStringLiteral("Fails to get the Python class.\n"));
        return 1;
    }
    Py_DECREF(dict);

    PyObject* outCatcher = nullptr;
    PyObject* errCatcher = nullptr;

    // Creates an instance of the class
    if (PyCallable_Check(python_class)) {
        outCatcher = PyObject_CallObject(python_class, nullptr);
    }
    else {
        outputChannel_->write(QStringLiteral("Cannot instantiate the Python class"));
        Py_DECREF(python_class);
        return false;
    }

    if (PyCallable_Check(python_class)) {
        errCatcher = PyObject_CallObject(python_class, nullptr);
    }
    else {
        outputChannel_->write(QStringLiteral("Cannot instantiate the Python class"));
        Py_DECREF(python_class);
        return false;
    }

    Py_DECREF(python_class);

    ((CustomObject *)outCatcher)->channel = outputChannel_;
    if (PySys_SetObject("stdout", outCatcher) < 0)
    {
        return false;
    }


    ((CustomObject *)errCatcher)->channel = errorChannel_;
    
    if (PySys_SetObject("stderr", errCatcher) < 0)
    {
        return false;
    }

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
        outputChannel_->write("... ");
    }
    else
    {
        outputChannel_->write(">>> ");
    }
}
