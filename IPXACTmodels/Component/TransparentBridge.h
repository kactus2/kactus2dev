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

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:transparentBridge element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT TransparentBridge 
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] masterInterfaceRef   Name reference to the bridged master interface.
	 */
	TransparentBridge(QString const& masterInterfaceRef = QString());
    	
    //! The copy constructor.
    TransparentBridge(TransparentBridge const& other);

    //! The move constructor.
    TransparentBridge(TransparentBridge&& other) = default;

	//! The destructor.
	~TransparentBridge() = default;

    // Disable assignment.
    TransparentBridge& operator=(TransparentBridge const& rhs) = delete;

    /*!
     *  Set the master interface reference.
     *
     *      @param [in] masterInterfaceName   The master interface name to reference.
     */
    void setMasterRef(QString const& masterInterfaceName);

    /*!
     *  Get the referenced master interface name.
     *
     *      @return The name of the referenced master interface.
     */
    QString getMasterRef() const;

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
  
    //! Reference to a master interface.
    QString masterRef_;

    //! Presence of the transparent bridge.
    QString isPresent_;

};

#endif // TRANSPARENTBRIDGE_H
