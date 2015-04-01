//-----------------------------------------------------------------------------
// File: verilogincludeimport_global.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.04.2015
//
// Description:
// DLL export definitions for Verilog Include Import.
//-----------------------------------------------------------------------------

#ifndef VERILOGINCLUDEIMPORT_GLOBAL_H
#define VERILOGINCLUDEIMPORT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef VERILOGINCLUDEIMPORT_LIB
# define VERILOGINCLUDEIMPORT_EXPORT Q_DECL_EXPORT
#else
# define VERILOGINCLUDEIMPORT_EXPORT Q_DECL_IMPORT
#endif

#endif // VERILOGINCLUDEIMPORT_GLOBAL_H
