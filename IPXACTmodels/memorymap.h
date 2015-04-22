/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#ifndef MEMORYMAP_H_
#define MEMORYMAP_H_

#include "ipxactmodels_global.h"

#include <IPXACTmodels/AbstractMemoryMap.h>
#include <IPXACTmodels/MemoryRemap.h>
#include <IPXACTmodels/remapstate.h>

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QTextStream>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Equals to the spirit:memoryMap element in IP-Xact specification.
//  Contains an unbounded list of memory map elements
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMap : public AbstractMemoryMap
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] memoryMapNode   A reference to the QDomNode to parse the information from.
	 */
	MemoryMap(QDomNode &memoryMapNode);

	/*!
	 *  The default constructor.
	 */
	MemoryMap();

	/*!
	 *  Copy constructor.
	 *
	 *      @param [in] other   The memory map to be copied.
	 */
	MemoryMap(const MemoryMap &other);

	/*!
	 *  Assignment operator.
	 *
	 *      @param [in] other   The memory map being assigned.
	 */
	MemoryMap &operator=(const MemoryMap &other);

	/*!
	 *  The destructor.
	 */
	~MemoryMap();

    /*!
	 *  Writes the memory map information with the writer.
	 *
	 *      @param [in] writer  A reference to a QXMLStreamWriter instance used to write the information.
	 */
	void write(QXmlStreamWriter& writer);

	/*!
	 *  Check if the memory map is in a valid state.
	 *
	 *      @param [in] componentChoices    Choices in the containing component.
	 *      @param [in] remapStateNames     A list containing remap state names of the component.
	 *      @param [in] errorList           The list to add the possible error messages to.
	 *      @param [in] parentIdentifier    Parent information to help identify the location of the errors.
     *
     *      @return True, if the state is valid and writing is possible.
	 */
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList remapStateNames,
        QStringList& errorList, const QString& parentIdentifier) const;

	/*!
	 *  Check if the memory map is in a valid state.
	 *
	 *      @param [in] componentChoices    Choices in the containing component.
	 *      @param [in] remapStateNames     A list containing remap state names of the component.
     *
     *      @return True, if the state is valid and writing is possible.
	 */
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList remapStateNames)
        const;

    /*!
	 *  Get the address unit bits value.
	 *
     *      @return The address unit bit value.
	 */
	unsigned int getAddressUnitBits() const;


	/*!
	 *  Set the address unit bits for this memory map.
	 *
	 *      @param [in] addressUnitBits     Value to be set.
	 */
	void setAddressUnitBits(unsigned int addressUnitBits);

    /*!
     *  Get a pointer to the list containing the memory remaps of this memory map.
     *
     *      @return Pointer to the list containing the memory remaps of this memory map.
     */
    QSharedPointer<QList<QSharedPointer<MemoryRemap> > > getMemoryRemaps();

private:

	/*!
	 * OPTIONAL (spirit:addressUnitBits)
	 * Defines the number of data bits in each address increment of the memory
	 * map.
	 */
	unsigned int addressUnitBits_;

    /*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Memory map vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

    /*!
     * OPTIONAL (spirit: memoryRemaps)
     * Memory remaps of the memory map.
     */
    QSharedPointer<QList<QSharedPointer<MemoryRemap> > > memoryRemaps_;

};

#endif /* MEMORYMAP_H_ */
