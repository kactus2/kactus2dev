/* 
 *	Created on:	18.4.2013
 *	Author:		Antti Kamppi
 *	File name:	alterabspgenerator_global.h
 *	Project:		Kactus 2
*/

#ifndef ALTERABSPGENERATOR_GLOBAL_H
#define ALTERABSPGENERATOR_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MEMORYMAPHEADERGENERATOR_LIB
# define ALTERABSPGENERATOR_EXPORT Q_DECL_EXPORT
#else
# define ALTERABSPGENERATOR_EXPORT Q_DECL_IMPORT
#endif

#endif // ALTERABSPGENERATOR_GLOBAL_H
