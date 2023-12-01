//-----------------------------------------------------------------------------
// File: TransparentBridge.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2017
//
// Description:
// Implementation of ipxact:transparentBridge element.
//-----------------------------------------------------------------------------

#ifndef TRANSPARENTBRIDGE_H
#define TRANSPARENTBRIDGE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Extendable.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:transparentBridge element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT TransparentBridge : public Extendable
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] masterInterfaceRef   Name reference to the bridged master interface.
	 */
	explicit TransparentBridge(QString const& masterInterfaceRef = QString());
    	
	//! The destructor.
	~TransparentBridge() = default;

    /*!
     *  Set the master interface reference.
     *
     *      @param [in] interfaceName   The master interface name to reference.
     */
    void setMasterRef(QString const& interfaceName);

    /*!
     *  Get the referenced master interface name.
     *
     *      @return The name of the referenced master interface.
     */
    QString getMasterRef() const;

    /*!
     *  Set the initiator interface reference.
     *
     *      @param [in] interfaceName   The initiator interface name to reference.
     */
    void setInitiatorRef(QString const& interfaceName);

    /*!
     *  Get the referenced initiator interface name.
     *
     *      @return The name of the referenced initiator interface.
     */
    QString getInitiatorRef() const;

    /*!
     *  Sets the presence for the transparent bridge.
     *
     *      @param [in] presence   The presence to set.
     */
    void setIsPresent(QString const& presence);

    /*!
     *  Get the presence of the transparent bridge.
     *
     *      @return The presence of the transparent bridge.
     */
    QString getIsPresent() const;
  
private:
   
    //! Reference to an initiator interface.
    QString initiatorRef_;

    //! Presence of the transparent bridge.
    QString isPresent_;

};

#endif // TRANSPARENTBRIDGE_H
