//-----------------------------------------------------------------------------
// File: InitiatorInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.6.2023
//
// Description:
// Implementation of ipxact:initiator in bus interface.
//-----------------------------------------------------------------------------

#ifndef INITIATORINTERFACE_H
#define INITIATORINTERFACE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Extendable.h>

#include <QString>
#include <QMap>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:initiator in bus interface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InitiatorInterface: public Extendable
{
public:

	//! Default constructor.
	InitiatorInterface();

	//! Copy constructor.
	InitiatorInterface(const InitiatorInterface& other);

	//! Assignment operator.
	InitiatorInterface& operator=(const InitiatorInterface& other);

	//! The destructor.
	virtual ~InitiatorInterface();
    
	/*! Set the address space reference.
	 *
	 *      @param [in] addressSpaceRef     The name of the address space to reference.
	 */
	void setAddressSpaceRef(QString const& addressSpaceRef);

	/*! Get the address space reference.
	 *
	 *      @return QString containing the address space name
	 */
	QString getAddressSpaceRef() const;
    
    /*!
     *  Set the presence value.
     *
     *      @param [in] newIsPresent    The new isPresent value.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the presence.
     *
     *      @return The isPresent value.
     */
    QString getIsPresent() const;

	/*! Set the base address.
	 *
	 *      @param [in]  baseAddress    The base address to set.
	 */
	void setBaseAddress(QString const& baseAddress);

	/*! Get the base address.
	 *
	 *      @return The base address for the initiator interface.
	 */
	QString getBaseAddress() const;


	void setModeRefs(QStringList const& modeRefs);

	QStringList getModeRefs() const;

private:

    //! The name of an address space that defines the range and width for transaction for the initiator interface.
    QString addressSpaceRef_;

    //! The presence of the master interface.
    QString isPresent_;

	// The starting address of the address space.
	QString baseAddress_;


	//! Mode references.
	QStringList modeRefs_;
};

#endif // INITIATORINTERFACE_H
