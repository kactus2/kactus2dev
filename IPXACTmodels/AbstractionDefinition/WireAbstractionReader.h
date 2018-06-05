//-----------------------------------------------------------------------------
// File: WireAbstractionReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.08.2015
//
// Description:
// Reader class for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef WIREABSTRACTIONREADER_H
#define WIREABSTRACTIONREADER_H

#include <QDomNode>
#include <QObject>
#include <QSharedPointer>

class WireAbstraction;
class WirePort;
class TimingConstraint;
class CellSpecification;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------
class WireAbstractionReader : public QObject
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent object.
     */
    WireAbstractionReader(QObject* parent = 0);

    //! The destructor.
    ~WireAbstractionReader();
        
    /*!
     *  Creates a wire for abstraction definition from XML description.
     *
     *      @param [in] wireNode   The XML document to create the wire definition from.
     *
     *      @return The created wire definition.
     */
    QSharedPointer<WireAbstraction> createWireAbstractionFrom(QDomNode const& wireNode) const;

private:	
    // Disable copying.
    WireAbstractionReader(WireAbstractionReader const& rhs);
    WireAbstractionReader& operator=(WireAbstractionReader const& rhs);
                               
    /*!
     *  Reads the qualifier from XML to a wire.
     *
     *      @param [in]     wireNode    The XML description of the wire.
     *      @param [in/out] wire        The wire definition to insert the qualifier into.
     */
    void parseQualifier(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire) const;
                                   
    /*!
     *  Reads the system ports from XML to a wire.
     *
     *      @param [in]     wireNode    The XML description of the wire.
     *      @param [in/out] wire        The wire definition to insert the system ports into.
     */
    void parseSystems(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire) const;
                                   
    /*!
     *  Creates a wirePort for a wire from XML description.
     *
     *      @param [in]     wirePortNode    The XML description of the wirePort.
     *
     *      @return The created wirePort.
     */
    QSharedPointer<WirePort> parseWirePort(QDomNode const& wirePortNode) const;

    /*!
     *  Reads the constraints for a wirePort from XML to a wire.
     *
     *      @param [in]     wirePortNode    The XML description of the wirePort.
     *      @param [in/out] wirePort        The wirePort to insert the constraints into.
     */
    void parseConstraints(QDomNode const& wirePortNode, QSharedPointer<WirePort> wirePort) const;

    /*!
     *  Creates a timing constraint for a wirePort from a XML description.
     *
     *      @param [in]     timingNode    The XML description of the timing constraint.
     *
     *      @return The created timing constraint.
     */
    QSharedPointer<TimingConstraint> parseTimingConstraint(QDomElement const& timingNode) const;

    /*!
     *  Creates a cell specification for load or drive constraint in a wirePort from a XML description.
     *
     *      @param [in]     cellNode    The XML description of the cell specification.
     *
     *      @return The created cell specification.
     */
    QSharedPointer<CellSpecification> parseCellSpecification(QDomElement const& cellNode) const;
       
    /*!
     *  Reads the constraints for mirrored mode for a wirePort from XML to a wire.
     *
     *      @param [in]     wirePortNode    The XML description of the wirePort.
     *      @param [in/out] wirePort        The wirePort to insert the mirrored constraints into.
     */
    void parseMirroredConstraints(QDomNode const& wirePortNode, QSharedPointer<WirePort> wirePort) const;

    /*!
     *  Reads the master port from XML to a wire.
     *
     *      @param [in]     wireNode    The XML description of the wire.
     *      @param [in/out] wire        The wire definition to insert the master port into.
     */   
    void parseMaster(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire) const;
                                 
    /*!
     *  Reads the slave port from XML to a wire.
     *
     *      @param [in]     wireNode    The XML description of the wire.
     *      @param [in/out] wire        The wire definition to insert the slave port into.
     */ 
    void parseSlave(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire) const;

};

#endif // WIREABSTRACTIONREADER_H
