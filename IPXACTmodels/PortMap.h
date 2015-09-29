//-----------------------------------------------------------------------------
// File: PortMap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.08.2014
//
// Description:
// Describes the mapping between the logical and physical ports.
//-----------------------------------------------------------------------------

#ifndef PORTMAP_H
#define PORTMAP_H

#include "ipxactmodels_global.h"

#include "generaldeclarations.h"

#include <QSharedPointer>
#include <IPXACTmodels/common/Range.h>
#include <IPXACTmodels/common/PartSelect.h>

class Port;
class Vector;

//-----------------------------------------------------------------------------
//! class PortMap.
//
// PortMap is used to store the spirit:portMap element info. It can be used
// to map logical ports, defined in abstraction definition to physical ports,
// defined in component description.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortMap {

public:
	//! The logical port of the port map.
	struct LogicalPort
	{
		//! The name associated with the port.
		QString name_;
		//! The range of the port.
		QSharedPointer<Range> range_;

		/*! \brief The default constructor.
		 *
		*/
		IPXACTMODELS_EXPORT  LogicalPort(){}
	};

	//! The physical port of the port map.
	struct PhysicalPort
	{
		//! The name associated with the port.
		QString name_;
		//! The selected physical port mapping.
		QSharedPointer<PartSelect> partSelect_;

		/*! \brief The default constructor.
		 *
		*/
		IPXACTMODELS_EXPORT  PhysicalPort(){}
	};

    /*! \brief Default constructor
	 *
	 */
	PortMap();

	//! \brief The copy constructor
	PortMap(const PortMap& other);

    //! The destructor.
    ~PortMap();

	/*! \brief The assignment operator
	 *
	 * \param other Reference to the PortMap to assign.
	 *
	 * \return Reference to this PortMap
	*/
	PortMap& operator=(const PortMap& other);
    
    /*!
     *  Gets the name of the logical port in the port map.
     *
     *      @return The name of the logical port.
     */
    QSharedPointer<LogicalPort> getLogicalPort() const;

    /*!
     *  Sets the name of the logical port in the port map.
     *
     *      @param [in] logicalPort   The name to set.
     */
    void setLogicalPort(QSharedPointer<LogicalPort> logicalPort);

    /*!
     *  Gets the name of the physical port in the port map.
     *
     *      @return The name of the physical port.
     */
    QSharedPointer<PhysicalPort> getPhysicalPort() const;

    /*!
     *  Sets the name of the physical port in the port map.
     *
     *      @param [in] physical   The name to set.
     */
    void setPhysicalPort(QSharedPointer<PhysicalPort> physicalPort);
	
    /*!
     *  Gets the logical tie off.
     *
     *     @return    The logical tie off.
     */
	QString getLogicalTieOff() const;
	
    /*!
     *  Sets the logical tie off.
     *
     *      @param [in] logicalTieOff   The new logical tie off.
     */
	void setLogicalTieOff(QString logicalTieOff);

private:
	//! MANDATORY Name of the logical port
	QSharedPointer<LogicalPort> logicalPort_;
   
    //!MANDATORY Name of the physical port
    QSharedPointer<PhysicalPort> physicalPort_;
         
	//! The logical tie off.
	QString logicalTieOff_;
};

#endif // PORTMAP_H