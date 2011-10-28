/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator2.h
 *		Project: Kactus 2
 */

#ifndef VHDLGENERATOR2_H
#define VHDLGENERATOR2_H

#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>

#include <LibraryManager/vlnv.h>

#include "vhdlgeneric.h"
#include "vhdlport.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

class LibraryInterface;
class VhdlConnectionEndPoint;

/*! \brief Vhdl Generator generates top-level vhdl for hierarchical component.
 *
 * This class and it's interface provide services to generate vhdl code from
 * a hierarchical view of a component. 
 */
class VhdlGenerator2 : public QObject {
	Q_OBJECT

public:

	VhdlGenerator2(LibraryInterface* handler, QObject *parent);
	
	//! \brief The destructor
	virtual ~VhdlGenerator2();

private:
	//! \brief No copying
	VhdlGenerator2(const VhdlGenerator2& other);

	//! \brief No assignment
	VhdlGenerator2& operator=(const VhdlGenerator2& other);
	
	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	QSharedPointer<Component> component_;

	QSharedPointer<Design> design_;

	QSharedPointer<DesignConfiguration> desConf_;

	QString viewName_;

	QString architectureName_;

	QString topLevelEntity_;

	QStringList libraries_;

	QMap<QString, QSharedPointer<VhdlGeneric> > topGenerics_;

	QMap<QString, QSharedPointer<VhdlPort> > topPorts_;
};

#endif // VHDLGENERATOR2_H
