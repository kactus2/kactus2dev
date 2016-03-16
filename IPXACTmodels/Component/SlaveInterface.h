//-----------------------------------------------------------------------------
// File: SlaveInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.10.2015
//
// Description:
// Implementation of ipxact:slave in bus interface.
//-----------------------------------------------------------------------------

#ifndef SLAVEINTERFACE_H
#define SLAVEINTERFACE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QList>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:slave in bus interface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SlaveInterface
{
public:

    //! Implementation of ipxact:bridge element.
	struct Bridge
	{
		//! Reference to a master interface.
		QString masterRef_;

		//! Presence of the Bridge.
		QString isPresent_;

		//! The default constructor.		 
		IPXACTMODELS_EXPORT Bridge();
	};

    //! Implementation of ipxact:fileSetRefGroup element.
	struct FileSetRefGroup
	{
		//! The group name.
		QString group_;

        //! The referenced fileset names.
		QStringList fileSetRefs_;

		//! The constructor.	
		FileSetRefGroup(): group_(), fileSetRefs_() {}
	};

	//! The default constructor.
	SlaveInterface();

	//! Copy constructor
	SlaveInterface(const SlaveInterface& other);

	//! Assignment operator
	SlaveInterface& operator=(const SlaveInterface& other);
	
	//! The destructor
	~SlaveInterface();
    
    /*!  Get the name of the referenced memory map if any.
	 *
	 *      @return The name of the memory map referenced in the slave interface.
	 */
	QString getMemoryMapRef() const;
    
	/*! Set the memory map for this interface.
	 *
	 *      @param [in] memoryMapRef    The name of the memory map to reference.
	 */
	void setMemoryMapRef(QString const& memoryMapRef);

	/*! Get the bridges for this slave interface.
	 *
	 *      @return The bridges for this slave interface.
	 */
	QSharedPointer<QList<QSharedPointer<Bridge> > > getBridges() const;
    
	/*! Check if the slave is connected to a master interface through bridge.
	 *
	 *      @return True if at least one bridge is found, otherwise false.
	 */
	bool hasBridge() const;

	/*! Get list of the master interfaces referred to through the contained bridge-elements.
	 *
	 *      @return The names of the referred master interfaces.
	 */
	QStringList getMasterReferences() const;

	/*! Get the fileSetRefGroups for this slave interface.
	 *
	 *      @return The fileSetRefGroups for this slave interface.
	 */
	QSharedPointer<QList<QSharedPointer<FileSetRefGroup> > > getFileSetRefGroup() const;

private:

	//! The referenced memory map name.	 
	QString memoryMapRef_;

	//! The bridges to master interfaces.
	QSharedPointer<QList<QSharedPointer<Bridge> > > bridges_;

    //! The file set references for the slave interface.
	QSharedPointer<QList<QSharedPointer<FileSetRefGroup> > > fileSetRefGroup_;
};

#endif // SLAVEINTERFACE_H
