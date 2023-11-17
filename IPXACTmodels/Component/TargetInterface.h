//-----------------------------------------------------------------------------
// File: SlaveInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.062023
//
// Description:
// Implementation of ipxact:target in bus interface.
//-----------------------------------------------------------------------------

#ifndef TARGETINTERFACE_H
#define TARGETINTERFACE_H

#include "TransparentBridge.h"
#include "FileSetRef.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/utilities/Copy.h>

#include <QString>
#include <QList>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:target in bus interface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT TargetInterface
{
public:

    //! Implementation of ipxact:fileSetRefGroup element.
	struct FileSetRefGroup
	{
		//! The group name.
		QString group_;

        //! The referenced filesets.
		QSharedPointer<QList<QSharedPointer<FileSetRef> > > fileSetRefs_ =
			QSharedPointer<QList<QSharedPointer<FileSetRef> > >(new QList<QSharedPointer<FileSetRef> > ());

		//! The constructor.	
		FileSetRefGroup(): group_() {}

		FileSetRefGroup(FileSetRefGroup const& other) : group_(other.group_)
		{
			Copy::copyList(other.fileSetRefs_, fileSetRefs_);
		}

		FileSetRefGroup& operator=(FileSetRefGroup const& other)
		{
			if (this != &other)
			{
				group_ = other.group_;

				fileSetRefs_->clear();
				Copy::copyList(other.fileSetRefs_, fileSetRefs_);
			}

			return *this;
		}
	};

	//! The default constructor.
	TargetInterface();

	//! Copy constructor
	TargetInterface(const TargetInterface& other);

	//! Assignment operator
	TargetInterface& operator=(const TargetInterface& other);
	
	//! The destructor
	~TargetInterface();
    
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


    QStringList getModeRefs() const;
    void setModeRefs(QStringList const& modeRefs);

	/*! Get the bridges for this slave interface.
	 *
	 *      @return The bridges for this slave interface.
	 */
	QSharedPointer<QList<QSharedPointer<TransparentBridge> > > getBridges() const;
    
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

    //! Mode references.
    QStringList modeRefs_;

	//! The bridges to master interfaces.
	QSharedPointer<QList<QSharedPointer<TransparentBridge> > > bridges_ =
		QSharedPointer<QList<QSharedPointer<TransparentBridge> > >(new QList<QSharedPointer<TransparentBridge> >);

    //! The file set references for the slave interface.
	QSharedPointer<QList<QSharedPointer<FileSetRefGroup> > > fileSetRefGroup_ =
		QSharedPointer<QList<QSharedPointer<FileSetRefGroup> > >(
			new QList<QSharedPointer<TargetInterface::FileSetRefGroup> >);
};


#endif // TARGETINTERFACE_H
