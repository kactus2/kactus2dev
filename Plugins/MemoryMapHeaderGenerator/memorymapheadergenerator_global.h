/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator_global.h
 * 	Project:	Kactus 2
*/

#ifndef MEMORYMAPHEADERGENERATOR_GLOBAL_H
#define MEMORYMAPHEADERGENERATOR_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MEMORYMAPHEADERGENERATOR_LIB
# define MEMORYMAPHEADERGENERATOR_EXPORT Q_DECL_EXPORT
#else
# define MEMORYMAPHEADERGENERATOR_EXPORT Q_DECL_IMPORT
#endif

#endif // MEMORYMAPHEADERGENERATOR_GLOBAL_H
