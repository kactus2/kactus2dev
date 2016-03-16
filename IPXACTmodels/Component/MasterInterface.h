//-----------------------------------------------------------------------------
// File: MasterInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.8.2010
//
// Description:
// Implementation of ipxact:master in bus interface.
//-----------------------------------------------------------------------------

#ifndef MASTERINTERFACE_H
#define MASTERINTERFACE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:master in bus interface.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MasterInterface
{

public:

	//! Default constructor.
	MasterInterface();

	//! Copy constructor.
	MasterInterface(const MasterInterface& other);

	//! Assignment operator.
	MasterInterface& operator=(const MasterInterface& other);

	//! The destructor.
	~MasterInterface();
    
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
	 *      @return The base address for the master interface.
	 */
	QString getBaseAddress() const;

	/*! Get the base address attributes.
	 *
	 *      @return The attributes for the base address.
	 */
	QMap<QString,QString> getBaseAttributes() const;

	/*! Get the prompt attribute
	 *
	 *      @return The value of the prompt attribute for base address.
	 */
	QString getPrompt() const;

	/*! Set the base address attributes.
	 *
	 *      @param [in] baseAttributes  The attributes for the base address element to set.
	 */
	void setBaseAttributes(const QMap<QString,QString>& baseAttributes);

private:

	//! The name of an address space that defines the range and width for transaction for the master interface.
	QString addressSpaceRef_;

    //! The presence of the master interface.
    QString isPresent_;

	// The starting address of the address space.
	QString baseAddress_;

	//! Any other attributes for the base address.
	QMap<QString, QString> baseAttributes_;
};

#endif // MASTERINTERFACE_H
