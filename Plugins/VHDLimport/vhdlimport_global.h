#ifndef VHDLIMPORT_GLOBAL_H
#define VHDLIMPORT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef VHDLIMPORT_LIB
# define VHDLIMPORT_EXPORT Q_DECL_EXPORT
#else
# define VHDLIMPORT_EXPORT Q_DECL_IMPORT
#endif

#endif // VHDLIMPORT_GLOBAL_H
