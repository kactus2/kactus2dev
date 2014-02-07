//-----------------------------------------------------------------------------
// File: quartuspinimportplugin_global.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.11.2013
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef QUARTUSPINIMPORTPLUGIN_GLOBAL_H
#define QUARTUSPINIMPORTPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QUARTUSPINIMPORTPLUGIN_LIB
# define QUARTUSPINIMPORTPLUGIN_EXPORT Q_DECL_EXPORT
#else
# define QUARTUSPINIMPORTPLUGIN_EXPORT Q_DECL_IMPORT
#endif

#endif // QUARTUSPINIMPORTPLUGIN_GLOBAL_H
