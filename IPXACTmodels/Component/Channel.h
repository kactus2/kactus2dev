//-----------------------------------------------------------------------------
// File: Channel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.09.2015
//
// Description:
// Implementation for ipxact:channel element.
//-----------------------------------------------------------------------------

#ifndef CHANNEL_H
#define CHANNEL_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QString>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:channel element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Channel : public NameGroup, public Extendable
{
public:

	//! Struct for BusInterfaceRefs.
	struct BusInterfaceRef: public Extendable
	{
		QString localName_;

		explicit BusInterfaceRef(QString const& localName) : localName_(localName) {};

		~BusInterfaceRef() = default;

		BusInterfaceRef& operator=(BusInterfaceRef const& other) = delete;

		BusInterfaceRef(BusInterfaceRef const& other):
			Extendable(other),
			localName_(other.localName_)
		{
			copyVendorExtensions(other);
		}
	};

	/*! The default constructor
	 *
	 * Constructs an empty channel which is in invalid state.
	*/
	Channel();

	//! Copy constructor
	Channel(const Channel& other);

	//! Assignment operator
	Channel& operator=(const Channel& other);

	//! The destructor
	~Channel();

    /*!
     *  Gets the presence.
     *
     *    @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *    @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*! Get the references to bus interfaces that belong to this channel.
	 *
	 *    @return The interface name references.
	 */
	QStringList getInterfaceNames() const;

	QSharedPointer<QList<QSharedPointer<BusInterfaceRef> > > getInterfaces() const;

	/*! Set the interfaces that belong to this channel.
	 *
	 *    @param [in] interfaceNames The name references to interfaces.
	*/
	void setInterfaces(QStringList const& interfaceNames);

private:

    void copyBusInterfaceRefs(Channel const& other);

	//! Presence of the channel.
	QString isPresent_;

	//! List of references to mirrored bus interfaces.
	QSharedPointer<QList<QSharedPointer<BusInterfaceRef> > > busInterfaces_ =
		QSharedPointer<QList<QSharedPointer<BusInterfaceRef> > >(new QList<QSharedPointer<BusInterfaceRef> >);

};

#endif // CHANNEL_H
