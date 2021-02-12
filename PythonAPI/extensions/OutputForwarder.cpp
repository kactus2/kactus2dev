//-----------------------------------------------------------------------------
// File: OutputForwarder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 12.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "OutputForwarder.h"

#include <structmember.h>

static void OutputForwarder_dealloc(OutputForwarderObject *self)
{
    ((OutputForwarderObject *)self)->channel = NULL;

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* OutputForwarder_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    OutputForwarderObject *self = (OutputForwarderObject *)type->tp_alloc(type, 0);
    if (self == NULL)
    {
        return NULL;
    }

    self->channel = NULL;

    return (PyObject *)self;
}

static int OutputForwarder_init(OutputForwarderObject *self, PyObject *args, PyObject *kwds)
{
    self->channel = NULL;

    return 0;
}

static PyMemberDef OutputForwarder_members[] = 
{
    {"channel", T_OBJECT, offsetof(OutputForwarderObject, channel), 0,
     "OutputForwarder output channel"},
    {NULL}  /* Sentinel */
};

static PyObject* OutputForwarder_write(OutputForwarderObject *self, PyObject * args)
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

static PyObject* OutputForwarder_flush(OutputForwarderObject *self, PyObject * args)
{
    return Py_None;
}

static PyObject* OutputForwarder_isatty(OutputForwarderObject *self, PyObject * args)
{
    return Py_True;
}

static PyMethodDef OutputForwarder_methods[] = 
{
    {"write", (PyCFunction)OutputForwarder_write, METH_VARARGS,
    PyDoc_STR("Write")
    },
    {"flush", (PyCFunction)OutputForwarder_flush, METH_NOARGS,
    PyDoc_STR("Flush write buffer")
    },
    { "isatty", (PyCFunction)OutputForwarder_isatty, METH_NOARGS,
    PyDoc_STR("Check if object is a tty.")
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject OutputForwarderType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "OutputForwarder.OutputForwarder",           /*tp_name*/
    sizeof(OutputForwarderObject),       /*tp_basicsize*/
    0,                          /*tp_itemsize*/
    (destructor)OutputForwarder_dealloc, /*tp_dealloc*/
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
    OutputForwarder_methods,             /* tp_methods */
    OutputForwarder_members,             /* tp_members */
    0,                          /* tp_getset */
    0,                          /* tp_base */
    0,                          /* tp_dict */
    0,                          /* tp_descr_get */
    0,                          /* tp_descr_set */
    0,                          /* tp_dictoffset */
    (initproc)OutputForwarder_init,      /* tp_init */
    0,                          /* tp_alloc */
    OutputForwarder_new,                 /* tp_new */
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

static PyModuleDef OutputForwardermodule =
{
    PyModuleDef_HEAD_INIT,
   "OutputForwarder",
    PyDoc_STR("Example module that creates an extension type."),
    -1,
};

PyMODINIT_FUNC PyInit_OutputForwarder(void)
{    
    if (PyType_Ready(&OutputForwarderType) < 0)
    {
        return NULL;
    }

    PyObject * module = PyModule_Create(&OutputForwardermodule);
    if (module == NULL)
    {
        return NULL;
    }
        
    Py_INCREF(&OutputForwarderType);
    if (PyModule_AddObject(module, "OutputForwarder", (PyObject *)&OutputForwarderType) < 0)
    {
        Py_DECREF(&OutputForwarderType);
        Py_DECREF(module);

        return NULL;
    }

    return module;
}
