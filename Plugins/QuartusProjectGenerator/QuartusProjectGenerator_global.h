/* 
 *	Created on:	18.4.2013
 *	Author:		Antti Kamppi
 *	File name:	QuartusProjectGenerator_global.h
 *	Project:		Kactus 2
*/

#ifndef QUARTUSPROJECTGENERATOR_GLOBAL_H
#define QUARTUSPROJECTGENERATOR_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QUARTUSGENERATOR_LIB
# define QUARTUSPROJECTGENERATOR_EXPORT Q_DECL_EXPORT
#else
# define QUARTUSPROJECTGENERATOR_EXPORT Q_DECL_IMPORT
#endif

#endif // QUARTUSPROJECTGENERATOR_GLOBAL_H
