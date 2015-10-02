/* 
 *
 *  Created on: 28.7.2010
 *      Author: Antti Kamppi
 */

#ifndef ADDRESSSPACE_H_
#define ADDRESSSPACE_H_

#include "MemoryMapBase.h"
#include "../segment.h"

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Range.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QSharedPointer>
#include <QDomNode>
#include <QString>
#include <QList>
#include <QMap>
#include <QXmlStreamWriter>

class Choice;
class Parameter;
class VendorExtension;

/*! \brief Equals the spirit:addressSpace element in the IP-Xact specification
 *
 * Each AddressSpace defines a logical address space seen by a master bus
 * interface.
 */
class IPXACTMODELS_EXPORT AddressSpace : public NameGroup, public Extendable{

public:

	/*! \brief The default constructor
	 *
	*/
	AddressSpace();

	//! \brief The copy constructor
	AddressSpace(const AddressSpace &other);

	//! \brief The assignment operator
	AddressSpace &operator=(const AddressSpace &other);

	//! \brief The destructor
	~AddressSpace();

    /*!
     *  Gets the presence.
     *
     *      @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
	 *  Check if the address space is in a valid state.
	 *
	 *      @param [in] componentChoices    Choices in the containing component.
	 *      @param [in] remapStateNames     List containing the remap state names of the component.
	 *      @param [in] errorList           The list to add the possible error messages to.
	 *      @param [in] parentIdentifier    String from parent to help identify the location of the error.
     *
     *      @return True if the state isvalid and writing is possible.
	 */
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList remapStateNames,
        QStringList& errorList, const QString& parentIdentifier) const;

	/*!
	 *  Check if the address space is in a valid state.
	 *
	 *      @param [in] componentChoices    Choices in the containing component.
	 *      @param [in] remapStateNames     A list of remap state names of the component.
     *
     *      @return True, if the state is valid and writing is possible, false otherwise.
	 */
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList remapStateNames)
        const;

	/*! \brief Get the address unit bits
	 *
	 * \return the amount of bits.
	 */
	unsigned int getAddressUnitBits() const;

	/*! \brief Get the range of the address space
	 *
	 * \return QString containing the range of the address space
	 */
	QSharedPointer<Range> getRange() const;

	/*! \brief Get the width of a row in an address space
	 *
	 * \return The width
	 */
	int getWidth() const;

	/*! \brief Set the address unit bits
	 *
	 * \param addressUnitBits The wanted setting
	 */
	void setAddressUnitBits(unsigned int addressUnitBits);

	/*! \brief Set the range for the address space
	 *
	 * \param range The wanted range
	 */
	void setRange(const QSharedPointer<Range> range);

	/*! \brief Set the width of the address space
	 *
	 * \param width The wanted width
	 */
	void setWidth(int width);

	/*! \brief Get the pointer to the local memory map
	 *
	 * \return A pointer to the local memory map
	 */
	QSharedPointer<MemoryMapBase> getLocalMemoryMap();

	/*! \brief Check if the address space contains local memory map.
	 *
	 * Method: 		hasLocalMemoryMapBase
	 * Full name:	AddressSpace::hasLocalMemoryMapBase
	 * Access:		public 
	 *
	 *
	 * \return True if a local memory map is found.
	*/
	bool hasLocalMemoryMap() const;

	/*! \brief Set the local memory map
	 *
	 * \param localMemoryMap a pointer to the new local memory map
	 */
	void setLocalMemoryMap(QSharedPointer<MemoryMapBase> localMemoryMap);

	/*! \brief Get the parameters of the address space.
	 *
	 * \return QList containing pointers to the parameters.
	*/
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get the parameters of the address space.
	 *
	 * \return QList containing pointers to the parameters.
	*/
	const QList<QSharedPointer<Parameter> >& getParameters() const;

	/*! \brief Get the segments contained in this address space.
	*
	* \return QList containing pointers to the segments.
	*/
	QList<QSharedPointer<Segment> >& getSegments();

	/*! \brief Get the segments contained in this address space.
	*
	* \return QList containing pointers to the segments.
	*/
	const QList<QSharedPointer<Segment> >& getSegments() const;

	/*! \brief Set the segments for this address space.
	*
	* \param segments QList containing pointers to set for the address space.
	*/
	void setSegments(const QList<QSharedPointer<Segment> >& segments);

	/*! \brief Get the last address contained in the address space.
	 * 
	 * The last address is defined by the range specified for address space.
	 * 
	 * \return The last address of the address space.
	*/
	quint64 getLastAddress() const;

private:

	//! Presence of the CPU.
	QString isPresent_;

	//! \brief MANDATORY, address range of an address space.
	QSharedPointer<Range> range_;

	//! \brief MANDATORY The bit width of a row in the address space.
	int width_;

	//! \brief OPTIONAL Contains the segments of the address space
	QList<QSharedPointer<Segment> > segments_;

	/*! \brief The number of data bits in each address increment.
	 * OPTIONAL
	 * (default value 8)
	 */
	unsigned int addressUnitBits_;

	/*! \brief Block of memory that can only be accessed by the master interfaces
	 * OPTIONAL spirit:localMemoryMapBase
	 */
	QSharedPointer<MemoryMapBase> localMemoryMap_;

	//! \brief Contains the parameters for the addressSpace
	//! OPTIONAL spirit:parameters
	QList<QSharedPointer<Parameter> > parameters_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Address space vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif /* ADDRESSSPACE_H_ */
