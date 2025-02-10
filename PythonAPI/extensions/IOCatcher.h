//-----------------------------------------------------------------------------
// File: IOCatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 12.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef OUTPUTFORWARDER_H
#define OUTPUTFORWARDER_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <PythonAPI/WriteChannel.h>

typedef struct
{
    PyObject_HEAD
    WriteChannel* channel;
} OutputCatcherObject;

typedef struct
{
    PyObject_HEAD
} InputCatcherObject;

 PyMODINIT_FUNC PyInit_IOCatcher(void);

#endif // OUTPUTFORWARDER_H