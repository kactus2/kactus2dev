//-----------------------------------------------------------------------------
// File: WirePort.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Implementation for ipxact:wirePort element within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef WIREPORT_H
#define WIREPORT_H

#include <QString>
#include <QSharedPointer>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/DirectionTypes.h>

class TimingConstraint;
class CellSpecification;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:wirePort element within abstraction definition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT WirePort 
{
public:

	//! The constructor.
	WirePort();

    //! Copy constructor.
    WirePort(WirePort const& other);

	//! The destructor.
	~WirePort();
    
    /*!
     *  Sets the group name for system mode.
     *
     *      @param [in] groupName   The system group to set.
     */
    void setSystemGroup(QString const& groupName);
    
    /*!
     *  Gets the group name for system mode.
     *
     *      @return The system group the wire belongs to.
     */
    QString getSystemGroup() const;

    /*!
     *  Sets the presence restriction for the port.
     *
     *      @param [in] presence   The restriction to set.
     */
    void setPresence(PresenceTypes::Presence presence);

    /*!
     *  Gets the presence restriction for the port.
     *
     *      @return The presence restriction for the port.
     */
    PresenceTypes::Presence getPresence() const;
   
    /*!
     *  Sets the number of bits required for the port.
     *
     *      @param [in] widthExpression   The number of bits to set.
     */
    void setWidth(QString const& widthExpression);

    /*!
     *  Gets the number of bits required for the port.
     *
     *      @return The number of bits to required for the port.
     */
    QString getWidth() const;

    /*!
     *  Sets a direction restriction for the port.
     *
     *      @param [in] direction   The required direction.
     */
    void setDirection(DirectionTypes::Direction direction);
   
    /*!
     *  Gets a direction restriction for the port.
     *
     *      @return The required direction.
     */
    DirectionTypes::Direction getDirection() const;
    
    /*!
     *  Sets a timing constraint for the port.
     *
     *      @param [in] timeConstraint      The constraint to set.
     */
    void setTimingConstraint(QSharedPointer<TimingConstraint> timeConstraint);

    /*!
     *  Gets the timing constraint for the port.
     *
     *      @return The timing constraint.
     */
    QSharedPointer<TimingConstraint> getTimingConstraint() const;
        
    /*!
     *  Sets a load constraint for the port.
     *
     *      @param [in] loadConstraint      The constraint to set.
     */
    void setLoadConstraint(QSharedPointer<CellSpecification> loadConstraint);
    
    /*!
     *  Gets the load constraint for the port.
     *
     *      @return The load constraint.
     */
    QSharedPointer<CellSpecification> getLoadConstraint() const;
            
    /*!
     *  Sets a drive constraint for the port.
     *
     *      @param [in] driveConstraint     The constraint to set.
     */
    void setDriveConstraint(QSharedPointer<CellSpecification> driveConstraint);
    
    /*!
     *  Gets the drive constraint for the port.
     *
     *      @return The drive constraint.
     */
    QSharedPointer<CellSpecification> getDriveConstraint() const;
                
    /*!
     *  Sets a timing constraint for the port in the mirrored mode.
     *
     *      @param [in] timeConstraint     The constraint to set.
     */
    void setMirroredTimingConstraint(QSharedPointer<TimingConstraint> timeConstraint);
    
    /*!
     *  Gets the timing constraint for the port in mirrored mode.
     *
     *      @return The timing constraint.
     */
    QSharedPointer<TimingConstraint> getMirroredTimingConstraint() const;
                    
    /*!
     *  Sets a load constraint for the port in the mirrored mode.
     *
     *      @param [in] loadConstraint     The constraint to set.
     */
    void setMirroredLoadConstraint(QSharedPointer<CellSpecification> loadConstraint);
        
    /*!
     *  Gets the load constraint for the port in mirrored mode.
     *
     *      @return The load constraint.
     */
    QSharedPointer<CellSpecification> getMirroredLoadConstraint() const;
                        
    /*!
     *  Sets a drive constraint for the port in the mirrored mode.
     *
     *      @param [in] driveConstraint     The constraint to set.
     */
    void setMirroredDriveConstraint(QSharedPointer<CellSpecification> driveConstraint);
        
    /*!
     *  Gets the drive constraint for the port in mirrored mode.
     *
     *      @return The drive constraint.
     */
    QSharedPointer<CellSpecification> getMirroredDriveConstraint() const;

private:

	// Disable copying.
    WirePort& operator=(WirePort const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! System group name for system interfaces.
    QString systemGroupName_;

    //! Defines if the port is required or forbidden in a bus interface.
    PresenceTypes::Presence presence_;

    //! Number of bits required for the port.
    QString width_;

    //! The direction restriction for the port.
    DirectionTypes::Direction direction_;

    //! The timing constraint for the port.
    QSharedPointer<TimingConstraint> timingConstraint_;

    //! The drive constraint for the port.
    QSharedPointer<CellSpecification> driveConstraint_;

    //! The load constraint for the port.
    QSharedPointer<CellSpecification> loadConstraint_;

    //! The timing constraint for the port in mirrored mode.
    QSharedPointer<TimingConstraint> mirroredTimingConstraint_;

    //! The drive constraint for the port in mirrored mode.
    QSharedPointer<CellSpecification> mirroredDriveConstraint_;

    //! The load constraint for the port in mirrored mode.
    QSharedPointer<CellSpecification> mirroredLoadConstraint_;
};

#endif // WIREPORT_H

