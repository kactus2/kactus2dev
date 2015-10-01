//-----------------------------------------------------------------------------
// File: MemoryRemap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAP_H
#define MEMORYREMAP_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>

#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryRemap : public MemoryMapBase
{

public:

    /*!
     *  The default constructor.
     */
    /*!
     *  The constructor.
     *
     *      @param [in] name        Name of the memory remap.
     *      @param [in] remapState  The remap state refered by the memory remap.
     */
    MemoryRemap(QString const& name = QString(), QString const& remapState = QString());

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
	 *  Check if the memory remap is in a valid state.
	 *
	 *      @param [in] componentChoices    Choices in the containing component.
	 *      @param [in] remapStateNames     A list of remap state names of the containing component.
	 *      @param [in] errorList           The list to add the possible error messages to.
	 *      @param [in] parentIdentifier    Parent information to help identifying the location of the errors.
     *
     *      @return True, if the memory remap is in a valid state, false otherwise
	 */
	//bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList remapStateNames,
    //    QStringList& errorList, const QString& parentIdentifier) const;

	/*!
	 *  Check if the memory remap is in a valid state.
	 *
	 *      @param [in] componentChoices            Choices in the containing component.
	 *      @param [in] componentRemapStateNames    A list of remap state names in the containing component.
     *
     *      @return True, if the state is valid and writing is possible, false otherwise.
	 */
	//bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    //    QStringList componentRemapStateNames) const;

    /*!
     *  Get the remap state of the memory remap.
     *
     *      @return The name of the remap state of the memory remap.
     */
    QString getRemapState() const;

    /*!
     *  Set the remap state of the memory remap.
     *
     *      @param [in] newRemapState   The name of the new remap state of the memory remap.
     */
    void setRemapState(QString newRemapState);

private:

    //! The remap state of this memory remap.
    QString remapState_;

};

#endif // MEMORYREMAP_H_
