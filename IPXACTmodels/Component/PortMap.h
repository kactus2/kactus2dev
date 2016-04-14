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

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Range.h>
#include <IPXACTmodels/common/PartSelect.h>

#include <IPXACTmodels/common/BooleanValue.h>

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
		IPXACTMODELS_EXPORT LogicalPort(QString const& name = QString()): name_(name) {}
	};

	//! The physical port of the port map.
	struct PhysicalPort
	{
		//! The name of the physical port.
		QString name_;

		//! The selected physical port mapping.
		QSharedPointer<PartSelect> partSelect_;

		//! The default constructor.
		IPXACTMODELS_EXPORT PhysicalPort(QString const& name = QString()): name_(name) {}
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
     *  Get the invert value.
     *
     *      @return The invert value as either true, false or empty.
     */
    BooleanValue getInvert() const;

    /*!
     *  Set the invert value.
     *
     *      @param [in] newInvert   The new invert value.
     */
    void setInvert(bool newInvert);

    /*!
     *  Remove the invert value.
     */
    void clearInvert();

    /*!
     *  Get the presence.
     *
     *      @return The value defining the presence.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence value.
     *
     *      @param [in] newIsPresent    The new value for presence.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the is informative value.
     *
     *      @return The isInformative value.
     */
    BooleanValue getIsInformative() const;

    /*!
     *  Set the is informative value.
     *
     *      @param [in] newInformative  The new is informative value.
     */
    void setIsInformative(bool newInformative);

    /*!
     *  Remove the is informative value..
     */
    void clearIsInformative();

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

    //! Specifies connections to the indicated physical port shall be logically inverted.
    BooleanValue invert_;

    //! The presence of this port map.
    QString isPresent_;

	//! The logical port in the port map.
	QSharedPointer<LogicalPort> logicalPort_;

    //! The physical port in the port map.
    QSharedPointer<PhysicalPort> physicalPort_;
         
	//! The logical tie off.
	QString logicalTieOff_;

    //! When true, specifies the port map to be used only for informative purposes.
    BooleanValue isInformative_;
};

#endif // PORTMAP_H