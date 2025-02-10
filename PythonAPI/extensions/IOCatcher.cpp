//-----------------------------------------------------------------------------
// File: IOCatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 12.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "IOCatcher.h"

#include <structmember.h>

static void OutputCatcher_dealloc(OutputCatcherObject* self)
{
    ((OutputCatcherObject *)self)->channel = NULL;

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* OutputCatcher_new(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwds*/)
{
    OutputCatcherObject *self = (OutputCatcherObject *)type->tp_alloc(type, 0);
    if (self == NULL)
    {
        return NULL;
    }

    self->channel = NULL;

    return (PyObject *)self;
}

static int OutputCatcher_init(OutputCatcherObject* self, PyObject* /*args*/, PyObject* /*kwds*/)
{
    self->channel = NULL;

    return 0;
}

static PyMemberDef OutputCatcher_members[] = 
{
    {"channel", T_OBJECT, offsetof(OutputCatcherObject, channel), 0,
     "OutputCatcher output channel"},
    {NULL}  /* Sentinel */
};

static PyObject* OutputCatcher_write(OutputCatcherObject* self, PyObject* args)
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

static PyObject* OutputCatcher_flush(OutputCatcherObject* /*self*/, PyObject* /*args*/)
{
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* OutputCatcher_isatty(OutputCatcherObject* /*self*/, PyObject* /*args*/)
{
    Py_INCREF(Py_False);
    return Py_False;
}

static PyMethodDef OutputCatcher_methods[] = 
{
    {"write", (PyCFunction)OutputCatcher_write, METH_VARARGS,
    PyDoc_STR("Write")
    },
    {"flush", (PyCFunction)OutputCatcher_flush, METH_NOARGS,
    PyDoc_STR("Flush write buffer")
    },
    { "isatty", (PyCFunction)OutputCatcher_isatty, METH_NOARGS,
    PyDoc_STR("Check if object is a tty.")
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject OutputCatcherType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "OutputCatcher.OutputCatcher",           /*tp_name*/
    sizeof(OutputCatcherObject),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)OutputCatcher_dealloc, /*tp_dealloc*/
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
    OutputCatcher_methods,             /* tp_methods */
    OutputCatcher_members,             /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)OutputCatcher_init,      /* tp_init */
    0,                          /* tp_alloc */
    OutputCatcher_new,                 /* tp_new */
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


static void InputCatcher_dealloc(InputCatcherObject* self)
{
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* InputCatcher_new(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwds*/)
{
    InputCatcherObject* self = (InputCatcherObject *)type->tp_alloc(type, 0);
    if (self == NULL)
    {
        return NULL;
    }

    return (PyObject *)self;
}

static int InputCatcher_init(InputCatcherObject* /*self*/, PyObject* /*args*/, PyObject* /*kwds*/)
{
    return 0;
}

static PyMemberDef InputCatcher_members[] =
{
    {NULL}  /* Sentinel */
};

static PyObject* InputCatcher_readline(InputCatcherObject* /*self*/, PyObject* /*args*/)
{
    // Return empty string to indicate EOF.
    return Py_BuildValue("s", "");
}
static PyObject* InputCatcher_isatty(InputCatcherObject* /*self*/, PyObject* /*args*/)
{
    // Return false to indicate non-interactive input.
    return Py_False;
}

static PyMethodDef InputCatcher_methods[] =
{
    {"readline", (PyCFunction)InputCatcher_readline, METH_VARARGS,
    PyDoc_STR("Read a line from the stream.")
    },
    { "isatty", (PyCFunction)InputCatcher_isatty, METH_NOARGS,
    PyDoc_STR("Check if object is a tty.")
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject InputCatcherType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "InputCatcher.InputCatcher",           /*tp_name*/
    sizeof(InputCatcherObject),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)InputCatcher_dealloc, /*tp_dealloc*/
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
    InputCatcher_methods,             /* tp_methods */
    InputCatcher_members,             /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)InputCatcher_init,      /* tp_init */
    0,                          /* tp_alloc */
    InputCatcher_new,                 /* tp_new */
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

static PyModuleDef IOCatchermodule =
{
    PyModuleDef_HEAD_INIT,
   "IOCatcher",
    PyDoc_STR("Module containing IO catchers for redirecting input and output streams."),
    -1,
};

PyMODINIT_FUNC PyInit_IOCatcher(void)
{    
    if (PyType_Ready(&OutputCatcherType) < 0)
    {
        return NULL;
    }

    if (PyType_Ready(&InputCatcherType) < 0)
    {
        return NULL;
    }

    PyObject * module = PyModule_Create(&IOCatchermodule);
    if (module == NULL)
    {
        return NULL;
    }
        
    Py_INCREF(&OutputCatcherType);
    if (PyModule_AddObject(module, "OutputCatcher", (PyObject *)&OutputCatcherType) < 0)
    {
        Py_DECREF(&OutputCatcherType);
        Py_DECREF(module);

        return NULL;
    }

    Py_INCREF(&InputCatcherType);
    if (PyModule_AddObject(module, "InputCatcher", (PyObject *)&InputCatcherType) < 0)
    {
        Py_DECREF(&OutputCatcherType);
        Py_DECREF(&InputCatcherType);
        Py_DECREF(module);

        return NULL;
    }

    return module;
}
