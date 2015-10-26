//-----------------------------------------------------------------------------
// File: PortMap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.08.2014
//
// Description:
// Implementation for ipxact:portMap.
//-----------------------------------------------------------------------------

#ifndef PORTMAP_H
#define PORTMAP_H

#include "ipxactmodels_global.h"

#include <IPXACTmodels/common/Range.h>
#include <IPXACTmodels/common/PartSelect.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:portMap.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortMap
{
public:

	//! The logical port of the port map.
	struct LogicalPort
	{
		//! The name of the logical port.
		QString name_;

		//! The range of the logical port.
		QSharedPointer<Range> range_;

		//! The default constructor.
		IPXACTMODELS_EXPORT  LogicalPort() {}
	};

	//! The physical port of the port map.
	struct PhysicalPort
	{
		//! The name of the physical port.
		QString name_;

		//! The selected physical port mapping.
		QSharedPointer<PartSelect> partSelect_;

		//! The default constructor.
		IPXACTMODELS_EXPORT  PhysicalPort() {}
	};

    //! Default constructor
	PortMap();

	//! The copy constructor
	PortMap(PortMap const& other);

    //! The destructor.
    ~PortMap();

	/*! The assignment operator
	 *
	 *      @param [in] other   The PortMap to assign.
	 *
	 *      @return Reference to this PortMap.
	*/
	PortMap& operator=(PortMap const& other);
    
    /*!
     *  Gets the logical port in the port map.
     *
     *      @return The name of the logical port.
     */
    QSharedPointer<LogicalPort> getLogicalPort() const;

    /*!
     *  Sets the logical port in the port map.
     *
     *      @param [in] logicalPort   The logical port to set.
     */
    void setLogicalPort(QSharedPointer<LogicalPort> logicalPort);

    /*!
     *  Gets the physical port in the port map.
     *
     *      @return The name of the physical port.
     */
    QSharedPointer<PhysicalPort> getPhysicalPort() const;

    /*!
     *  Sets the physical port in the port map.
     *
     *      @param [in] physical   The physical port to set.
     */
    void setPhysicalPort(QSharedPointer<PhysicalPort> physicalPort);
	
    /*!
     *  Gets the logical tie off.
     *
     *     @return    The logical tie off value.
     */
	QString getLogicalTieOff() const;
	
    /*!
     *  Sets the logical tie off.
     *
     *      @param [in] logicalTieOff   The new logical tie off.
     */
	void setLogicalTieOff(QString const& logicalTieOff);

private:
	//! The logical port in the port map.
	QSharedPointer<LogicalPort> logicalPort_;

    //! The physical port in the port map.
    QSharedPointer<PhysicalPort> physicalPort_;
         
	//! The logical tie off.
	QString logicalTieOff_;
};

#endif // PORTMAP_H