//-----------------------------------------------------------------------------
// File: RemapState.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Describes the ipxact:remapState element.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATE_H
#define REMAPSTATE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/NameGroup.h>

#include <QString>
#include <QList>
#include <QSharedPointer>

class RemapPort;

//-----------------------------------------------------------------------------
//! Describes the ipxact:remapState element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RemapState : public NameGroup
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name    The name of the remap state.
     */
    RemapState(QString const& name = QString());

	/*!
	 *  Copy constructor.
	 *
	 *      @param [in] other   The remap state to be copied.
	 */
	RemapState(const RemapState &other);

    /*!
	 *  Assignment operator.
	 *
	 *      @param [in] other   The remap state being assigned.
	 */
	RemapState &operator=(const RemapState &other);

	/*!
	 *  The destructor.
	 */
	~RemapState();

    /*!
	 *  Get the list of the remap ports for this remap state.
	 *
     *      @return List containing pointer to the remap port instances.
	 */
    QSharedPointer<QList<QSharedPointer<RemapPort> > > getRemapPorts() const;

    /*!
     *  Set the remap ports.
     *
     *      @param [in] newRemapPorts   Pointer to a list containing the new remap ports.
     */
    void setRemapPorts(QSharedPointer<QList<QSharedPointer<RemapPort> > > newRemapPorts);

private:

    /*!
     *  Copy the remap ports.
     *
     *      @param [in] other   The copied remap state.
     */
    void copyRemapPorts(const RemapState& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of remap ports definining the remap state.
    QSharedPointer<QList<QSharedPointer<RemapPort> > > remapPorts_;
};

#endif // REMAPSTATE_H
