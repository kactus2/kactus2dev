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
    /*! \brief Default constructor
	 *
	 */
	PortMap();

    /*! \brief The constructor
	 *
	 * \param portMap a reference to the QDomNode that can used to parse the
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs if a mandatory element is missing in
	 * this portMap or one of it's members.
	 */
	PortMap(QDomNode &portMap);

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

	/*! \brief Check if the port map is in a valid state.
	 *
	 * \param physicalPorts List of the physical ports of the component and their bounds.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QList<General::PortBounds>& physicalPorts, 
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the port map is in a valid state.
	 * 
	 * \param physicalPorts List of the physical ports of the component and their bounds.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QList<General::PortBounds>& physicalPorts) const;


    /*!
     *  Writes the port map element to xml.
     *
     *      @param [in] writer   The writer to use for writing.     
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Gets the physical range in the port map.
     *
     *      @param [in] referencedPhysicalPort   The physical port referenced in the port map 
     *                                           in the component containing the port map.
     *
     *      @return The physical range in the port map.
     */
    General::PortBounds getPhysicalRange(QSharedPointer<Port> referencedPhysicalPort) const;

    /*!
     *  Gets the logical range in the port map.
     *
     *      @param [in] referencedPhysicalPort   The physical port referenced in the port map 
     *                                           in the component containing the port map.
     *
     *      @return The logical range in the port map.
     */
    General::PortBounds getLogicalRange(QSharedPointer<Port> referencedPhysicalPort) const;
    
    /*!
     *  Gets the name of the logical port in the port map.
     *
     *      @return The name of the logical port.
     */
    QString logicalPort() const;

    /*!
     *  Sets the name of the logical port in the port map.
     *
     *      @param [in] logicalPort   The name to set.
     */
    void setLogicalPort(QString const& logicalPort);

    /*!
     *  Gets the logical vector in the port map.
     *
     *      @return The logical vector.
     */
    QSharedPointer<Vector> logicalVector() const;

    /*!
     *  Gets the left bound for the logical port.
     *
     *      @return The left bound for the logical port or -1 if not set.
     */
    int getLogicalLeft() const;

    /*!
     *  Sets the left bound for the logical port.
     *
     *      @param [in] left   The left bound to set.
     */
    void setLogicalLeft(int left);

    /*!
     *  Gets the right bound for the logical port.
     *
     *      @return The right bound for the logical port or -1 if not set.
     */
    int getLogicalRight() const;

    /*!
     *  Sets the right bound for the logical port.
     *
     *      @param [in] right   The right bound to set.
     */
    void setLogicalRight(int right);

    /*!
     *  Gets the name of the physical port in the port map.
     *
     *      @return The name of the physical port.
     */
    QString physicalPort() const;

    /*!
     *  Sets the name of the physical port in the port map.
     *
     *      @param [in] physical   The name to set.
     */
    void setPhysicalPort(QString const& physicalPort);

    /*!
     *  Gets the physical vector in the port map.
     *
     *      @return The physical vector.
     */
    QSharedPointer<Vector> physicalVector() const;

    /*!
     *  Gets the left bound for the physical port.
     *
     *      @return The left bound for the physical port or -1 if not set.
     */
    int getPhysicalLeft() const;

    /*!
     *  Sets the left bound for the physical port.
     *
     *      @param [in] left   The right bound to set.
     */
    void setPhysicalLeft(int left);

    /*!
     *  Gets the right bound for the physical port.
     *
     *      @return The right bound for the physical port or -1 if not set.
     */
    int getPhysicalRight() const;

    /*!
     *  Sets the right bound for the physical port.
     *
     *      @param [in] right   The right bound to set.
     */
    void setPhysicalRight(int right);

private:
	//! MANDATORY Name of the logical port
	QString logicalPort_;
   
    //!MANDATORY Name of the physical port
    QString physicalPort_;
         
	/*! \brief The optional vector element for the logical Port
	 *
	 * If this element exists then the port is vectored and it's type is
	 * std_logic_vector. This element specifies the left and right bound of the
	 * vectored port.
	 */
	QSharedPointer<Vector> logicalVector_;

    /*! \brief The optional vector element for the physical port.
	 *
	 * If this element exists then the port is vectored and it's type is
	 * std_logic_vector. This element specifies the left and right bound of the
	 * vectored port.
	 */
	QSharedPointer<Vector> physicalVector_;
};

#endif // PORTMAP_H