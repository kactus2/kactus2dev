// Copyright Tampere University of Technology

#ifndef MODELSIMGENERATOR_GLOBAL_H
#define MODELSIMGENERATOR_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MODELSIMGENERATOR_LIB
# define MODELSIMGENERATOR_EXPORT Q_DECL_EXPORT
#else
# define MODELSIMGENERATOR_EXPORT Q_DECL_IMPORT
#endif

#endif // MODELSIMGENERATOR_GLOBAL_H
