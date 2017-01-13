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

#include <QList>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <Plugins/common/HDLParser/HDLParserCommon.h>

#include <editors/ComponentEditor/common/ListParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

class LibraryInterface;
class PortMap;

//-----------------------------------------------------------------------------
// An instance of a component instance within a hierarchy with its parameters and interfaces parsed.
//-----------------------------------------------------------------------------
class MetaInstance
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The instantiated component.
     *      @param [in] activeView              The active view for the component.
	 */
	MetaInstance(LibraryInterface* library,
        QSharedPointer<Component> component,
        QSharedPointer<View> activeView,
        QSharedPointer<ListParameterFinder> topFinder,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs);

	//! The destructor.
    ~MetaInstance();

    //! The parsed parameters that are to be used with the instance. CEVs used where applicable.
    QList<QSharedPointer<Parameter> > parameters_;
    //! The parsed interfaces of the instance, keyed with its name.
    QMap<QString,QSharedPointer<MetaInterface> > interfaces_;
    //! The parsed ad-hoc ports of the component keyed with its physical name.
    QMap<QString,QSharedPointer<MetaPort> > adHocPorts_;
    //! The module name for HDL.
    QString moduleName_;

private:
	// Disable copying.
	MetaInstance(MetaInstance const& rhs);
    MetaInstance& operator=(MetaInstance const& rhs);
    
    /*!
    *   Parses the expression using the parser and returns the result.
    */
    static QString parseExpression(IPXactSystemVerilogParser& parser, const QString& expression);

    /*!
    *  Culls and copies parameter declarations of the component and active instantiation if any exists.
    */
    void cullParameters();

    /*!
    *  Parses the found parameter declarations.
    */
    void parseParameters(IPXactSystemVerilogParser& parser,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs);
    
    /*!
    *  Culls the interfaces of the component.
    */
    void parseInterfaces();
    
    /*!
    *  Culls and parses the ports of the component.
    */
    void parsePorts(IPXactSystemVerilogParser& parser);

    /*!
     *  Finds the mapped port bounds for a port map.
     *
     *      @param [in] parser          Used for parsing the bounds.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
	static QPair<QString, QString> portBoundsInMapping(IPXactSystemVerilogParser& parser,
        QSharedPointer<PortMap> portMap);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component library.
    LibraryInterface* library_;

    //! The matching IP-XACT component.
    QSharedPointer<Component> component_;

    //! The active view of the instance.
    QSharedPointer<View> activeView_;

    //! The component instantiation referred by the active view.
    QSharedPointer<ComponentInstantiation> activeInstantiation_;
};

#endif // METAINSTANCE_H
