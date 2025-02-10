#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PYTHONAPI_LIB)
#  define PYTHONAPI_EXPORT Q_DECL_EXPORT
# else
#  define PYTHONAPI_EXPORT Q_DECL_IMPORT
# endif
#else
# define PYTHONAPI_EXPORT
#endif
