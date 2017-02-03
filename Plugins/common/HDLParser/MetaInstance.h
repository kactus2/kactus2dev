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


#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/common/HDLParser/MetaComponent.h>

#include <editors/ComponentEditor/common/ListParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

class LibraryInterface;
class PortMap;

//-----------------------------------------------------------------------------
// An instance of a component instance within a hierarchy with its parameters and interfaces parsed.
//-----------------------------------------------------------------------------
class MetaInstance : public MetaComponent
{
public:

	/*!
	 *  The constructor.
     *
     *      @param [in] library             The library which is expected to contain the parsed documents.
     *      @param [in] messages            For message output.
     *      @param [in] component           The instantiated component.
     *      @param [in] activeView          The active view for the component. May be null.
	 */
    MetaInstance(LibraryInterface* library,
        MessagePasser* messages,
        QSharedPointer<Component> component,
        QSharedPointer<View> activeView);

	//! The destructor.
    ~MetaInstance();
    
	/*!
	 *  Parses the interfaces, ports and parameters for explicit values of this particular meta instance.
     *
     *      @param [in] componentInstance   The instance under inspection.
     *      @param [in] topFinder           Finder for parameters in upper level. May be null
     *      @param [in] cevs                Applicable configurable element values for the instance. May be null.
     */
    void parseInstance(QSharedPointer<ComponentInstance> componentInstance,
        QSharedPointer<ListParameterFinder> topFinder,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs);
    
    /*!
     *  Returns list of culled interfaces.
     */
    QSharedPointer<QMap<QString,QSharedPointer<MetaInterface> > > getInterfaces() { return interfaces_; }
    
    /*!
     *  Returns the associated component instance.
     */
    QSharedPointer<ComponentInstance> getComponentInstance() const { return componentInstance_; }

private:
	// Disable copying.
	MetaInstance(MetaInstance const& rhs);
    MetaInstance& operator=(MetaInstance const& rhs);

    /*!
     *  Parses the found parameter declarations.
     */
    virtual void parseParameters(IPXactSystemVerilogParser& parser,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs);
    
    /*!
     *  Culls the interfaces of the component.
     */
    virtual void cullInterfaces();
    
    /*!
     *  Culls and parses the ports of the component.
     *
     *      @param [in] parser          Used to parse expressions.
     */
    virtual void parsePorts(IPXactSystemVerilogParser& parser);
    
    /*!
    *   Parses the expression using the parser and returns the result.
    */
    static QString parseExpression(IPXactSystemVerilogParser& parser, const QString& expression);

    /*!
     *  Finds the mapped logical port bounds for a port map.
     *
     *      @param [in] parser          Used for parsing the bounds.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The logical port bounds for the port map in an instance.
     */
	static QPair<QString, QString> logicalPortBoundsInMapping(IPXactSystemVerilogParser& parser,
        QSharedPointer<PortMap> portMap);
    
    /*!
     *  Finds the mapped physical port bounds for a port map.
     *
     *      @param [in] parser          Used for parsing the bounds.
     *      @param [in] portMap         The port map for the which to find the physical bounds.
     *
     *      @return The physical port bounds for the port map in an instance.
     */
    static QPair<QString, QString> physicalPortBoundsInMapping(IPXactSystemVerilogParser& parser,
        QSharedPointer<PortMap> portMap);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component library.
    LibraryInterface* library_;
    //! The parsed interfaces of the instance, keyed with its name.
    QSharedPointer<QMap<QString,QSharedPointer<MetaInterface> > > interfaces_;
    //! The matching IP-XACT component instance.
    QSharedPointer<ComponentInstance> componentInstance_;

};

#endif // METAINSTANCE_H
