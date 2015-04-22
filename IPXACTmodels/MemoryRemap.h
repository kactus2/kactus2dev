//-----------------------------------------------------------------------------
// File: MemoryRemap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.04.2015
//
// Description:
// Memory Remap describes additional memory map items of a slave bus interface in a specific remap state.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAP_H_
#define MEMORYREMAP_H_

#include "ipxactmodels_global.h"

#include <IPXACTmodels/AbstractMemoryMap.h>
#include <IPXACTmodels/remapstate.h>

#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! Memory Remap describes additional memory map items of a slave bus interface in a specific remap state.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryRemap : public AbstractMemoryMap
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] memoryRemapNode     A reference to the QDomNode to parse the information from.
     */
    MemoryRemap(QDomNode& memoryRemapNode);

    /*!
     *  The default constructor.
     */
    MemoryRemap();

    /*!
     *  Copy constructor.
     *
     *      @param [in] other   The memory remap being copied.
     */
    MemoryRemap(const MemoryRemap& other);

    /*!
     *  Assignment operator.
     *
     *      @param [in] other   The memory remap being assigned.
     */
    MemoryRemap& operator=(const MemoryRemap& other);

    /*!
     *  The destructor.
     */
    ~MemoryRemap();

	/*!
	 *  Writes the memory remap information with the writer.
	 *
	 *      @param [in] writer   A reference to a QXmlStreamWriter instance used to write the information.
	 */
	void write(QXmlStreamWriter& writer);

    /*!
	 *  Check if the memory remap is in a valid state.
	 *
	 *      @param [in] componentChoices    Choices in the containing component.
	 *      @param [in] remapStateNames     A list of remap state names of the containing component.
	 *      @param [in] errorList           The list to add the possible error messages to.
	 *      @param [in] parentIdentifier    Parent information to help identifying the location of the errors.
     *
     *      @return True, if the memory remap is in a valid state, false otherwise
	 */
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList remapStateNames,
        QStringList& errorList, const QString& parentIdentifier) const;

	/*!
	 *  Check if the memory remap is in a valid state.
	 *
	 *      @param [in] componentChoices            Choices in the containing component.
	 *      @param [in] componentRemapStateNames    A list of remap state names in the containing component.
     *
     *      @return True, if the state is valid and writing is possible, false otherwise.
	 */
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QStringList componentRemapStateNames) const;

    /*!
     *  Set the remap state of the memory remap.
     *
     *      @param [in] newRemapState   The name of the new remap state of the memory remap.
     */
    void setRemapState(QString newRemapState);

    /*!
     *  Get the remap state of the memory remap.
     *
     *      @return The name of the remap state of the memory remap.
     */
    QString getRemapState() const;

private:

    //! The remap state of this memory remap.
    QString remapState_;

};

#endif // MEMORYREMAP_H_
