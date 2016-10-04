//-----------------------------------------------------------------------------
// File: HDLDesignParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 06.07.2016
//
// Description:
// Preliminary design parsing for HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLDESIGNPARSER_H
#define HDLDESIGNPARSER_H

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include "HDLParserCommon.h"

class LibraryInterface;
class PortSorter;
class AbstractionType;
class AbstractionDefinition;

class ListParameterFinder;
class ExpressionParser;
class PortAlignment;

//-----------------------------------------------------------------------------
// Preliminary parsing for HDL generation.
//-----------------------------------------------------------------------------
class HDLDesignParser : public QObject
{
	Q_OBJECT

public:
    //! The constructor.
    HDLDesignParser(LibraryInterface* library, QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConf);

    //! The destructor.
    ~HDLDesignParser();


    void parseDesign(QSharedPointer<GenerationComponent> topComponent, QSharedPointer<View> topComponentView,
        QSharedPointer<GenerationInstance> topInstance = QSharedPointer<GenerationInstance>());

    QList<QSharedPointer<GenerationDesign> > getParsedDesigns(){return parsedDesigns_;}

signals:
	
    /*!
     *  Emitted when reportable error occurs.
     */
	void reportError(const QString& errorMessage) const;

private:

    // Disable copying.
    HDLDesignParser(HDLDesignParser const& rhs);
    HDLDesignParser& operator=(HDLDesignParser const& rhs);

    QString parseExpression(IPXactSystemVerilogParser& parser, const QString& expression) const;

    /*!
     *  Parses all the component instances in the design.
     */
    void parseComponentInstances();
    
    /*!
     *  Finds interconnections within the design and which interfaces they connect to.
     */
	void findInterconnections();
    
    /*!
     *  Creates port/interface assignments and wires out of interconnections.
     */
	void assignInterconnections();
    
    /*!
     *  Does interconnection assignments to port maps of given abstraction type.
     */
    void parsePortMaps(QSharedPointer<AbstractionType> absType, QSharedPointer<GenerationInstance> gi,
        QSharedPointer<GenerationInterconnection> gic, QSharedPointer<AbstractionDefinition> absDef,
        QSharedPointer<BusInterface> busInterface);
    
    /*!
     *  Compares new bounds to existing bounds of the wire and then assigns the largest combination of two
     *  If even one of the existing bounds is missing, candidates will replace them both.
     *
     *      @param [in] wire            The wire which bounds are possibly enlarged.
     *      @param [in] boundCand		The candidate for new bounds.
     */
    void assignLargerBounds(QSharedPointer<GenerationWire> wire, QPair<int,int> &boundCand);
    
    /*!
     *  Finds ad hoc interconnections within the design and which interfaces they connect to.
     */
	void findInternalAdhocs();
    
    /*!
     *  Creates port assignments from ad hoc connections.
     */
	void assignInternalAdHocs();
    
    /*!
     *  Creates special port assignments for hierarchical ad hoc connections.
     */
	void parseHierarchicallAdhocs();

    /*!
     *  Finds the physical bounds for a port map in an instance.
     *
     *      @param [in] instanceName    The name of the instance.
     *      @param [in] port			The port for the which to find the physicalbounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
    QPair<QString, QString> physicalPortBoundsInInstance(QSharedPointer<GenerationInstance> instance,
        QSharedPointer<GenerationPort> port) const;

    /*!
     *  Finds the logical bounds for a port map in an instance.
     *
     *      @param [in] instanceName    The name of the instance.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
	QPair<QString, QString> logicalPortBoundsInInstance(QSharedPointer<GenerationInstance> instance,
        QSharedPointer<PortMap> portMap) const;
    
    /*!
     *  Assigns a tie off value, if applicable.
     *
     *      @param [in] tieOff              The tied value needing assignment.
     *      @param [in] port			    The port where the tied value would belong to.
     *      @param [in] requiredDirection   The direction of the tiedOff: Must be inout or this.
     *
     *      @return The resolved tied value.
     */
	QString connectTieOff(QString tieOff, QSharedPointer<GenerationPort> port, DirectionTypes::Direction requiredDirection);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

     //! The component library.
     LibraryInterface* library_;

     //! The active view for top component.
     QSharedPointer<View> topComponentView_;

     //! The design to parse.
	 QSharedPointer<Design> design_;

	 //! The design configuration to parse.
	 QSharedPointer<DesignConfiguration> designConf_;
     
     //! The list of all parsed designs that are below the current top are in this list.
     QList<QSharedPointer<GenerationDesign> > parsedDesigns_;

     //! The parsed design.
     QSharedPointer<GenerationDesign> retval_;

     //! The finder for top level parameters.
     QSharedPointer<ListParameterFinder> topFinder_;
};

#endif // HDLDESIGNPARSER_H
