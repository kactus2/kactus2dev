//-----------------------------------------------------------------------------
// File: MetaInstance.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.01.2017
//
// Description:
// An instance of a component instance within a hierarchy with its parameters and interfaces parsed.
//-----------------------------------------------------------------------------

#ifndef METAINSTANCE_H
#define METAINSTANCE_H

#include <Plugins/common/HDLParser/MetaComponent.h>

#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

class LibraryInterface;
class PortMap;
class MessageMediator;

//-----------------------------------------------------------------------------
// An instance of a component instance within a hierarchy with its parameters and interfaces parsed.
//-----------------------------------------------------------------------------
class MetaInstance : public MetaComponent
{
public:

	/*!
	 *  The constructor.
     *
     *      @param [in] componentInstance   The instance under inspection.
     *      @param [in] library             The library which is expected to contain the parsed documents.
     *      @param [in] messages            For message output.
     *      @param [in] component           The instantiated component.
     *      @param [in] activeView          The active view for the component. May be null.
	 */
    MetaInstance(
        QSharedPointer<ComponentInstance> componentInstance,
        LibraryInterface* library,
        MessageMediator* messages,
        QSharedPointer<Component> component,
        QSharedPointer<View> activeView);

	//! The destructor.
    virtual ~MetaInstance() = default;
    
    // Disable copying.
    MetaInstance(MetaInstance const& rhs) = delete;
    MetaInstance& operator=(MetaInstance const& rhs) = delete;

	/*!
	 *  Parses the interfaces and ports for explicit values of this particular meta instance.
     *
     */
    void parseInstance();

    /*!
    *   Parses the expression using the parser and returns the result.
    */
    static QString parseExpression(IPXactSystemVerilogParser const& parser, QString const& expression);
    
    /*!
     *  Returns list of culled interfaces.
     */
    QSharedPointer<QMap<QString,QSharedPointer<MetaInterface> > > getInterfaces() { return interfaces_; }
    
    /*!
     *  Returns the associated component instance.
     */
    QSharedPointer<ComponentInstance> getComponentInstance() const { return componentInstance_; }

private:
    
    /*!
     *  Parse the interfaces of the component instance.
     */
    void parseInterfaces();
    
    /*!
     *  Culls and parses the ports of the component.
     *
     *      @param [in] parser          Used to parse expressions.
     */
    void parsePorts(IPXactSystemVerilogParser const& parser);
    
    /*!
     *  Creates assignments for port maps.
     *
     *      @param [in] parser          Used to parse expressions.
     */
    void parsePortAssignments(IPXactSystemVerilogParser const& parser);

    /*!
     *  Finds the mapped logical port bounds for a port map.
     *
     *      @param [in] parser          Used for parsing the bounds.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The logical port bounds for the port map in an instance.
     */
	QPair<QString, QString> logicalPortBoundsInMapping(IPXactSystemVerilogParser const& parser,
        QSharedPointer<PortMap> portMap) const;
    
    /*!
     *  Finds the mapped physical port bounds for a port map.
     *
     *      @param [in] parser          Used for parsing the bounds.
     *      @param [in] portMap         The port map for the which to find the physical bounds.
     *
     *      @return The physical port bounds for the port map in an instance.
     */
    QPair<QString, QString> physicalPortBoundsInMapping(IPXactSystemVerilogParser const& parser,
        QSharedPointer<PortMap> portMap) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The matching IP-XACT component instance.
    QSharedPointer<ComponentInstance> componentInstance_;

    //! The component library.
    LibraryInterface* library_;

    //! The parsed interfaces of the instance, keyed with its name.
    QSharedPointer<QMap<QString, QSharedPointer<MetaInterface> > > interfaces_;
};

#endif // METAINSTANCE_H
