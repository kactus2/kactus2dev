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
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <QTextStream>
#include "HDLParserCommon.h"

class ExpressionFormatter;
class LibraryInterface;
class PortSorter;
class AbstractionType;

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
	HDLDesignParser(LibraryInterface* library, QSharedPointer<Component> component, QSharedPointer<View> topComponentView, 
		QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConf);

    //! The destructor.
    ~HDLDesignParser();
	
    /*!
	*   Returns an expression parser for the given top component.
	*/
	QSharedPointer<ExpressionFormatter> createFormatterForComponent(QSharedPointer<Component> targetComponent,
		QSharedPointer<View> targetView, QString instanceName);

    /*!
     *  Creates an expression parser for the given component.
     *
	 *      @param [in] targetComponent		The component for which to create the parser.
	 *      @param [in] targetView			The active view of the instance.
	 *      @param [in] instanceName		An instance of the component.
     *
     *      @return Expression parser for the component.
     */
	QSharedPointer<ExpressionParser> createParserForComponent(QSharedPointer<Component> targetComponent,
		QSharedPointer<View> targetView, QString instanceName);

	void parseDesign();

	QList<QSharedPointer<GenerationInstance> > instances_;
	QList<QSharedPointer<GenerationInterconnection> > interConnections_;
	QList<QSharedPointer<GenerationAdHoc> > adHocs_;
	QMap<QString, QString> portTiedValues_;

signals:
	
    /*!
     *  Emitted when reportable error occurs.
     */
	void reportError(const QString& errorMessage) const;

private:

    // Disable copying.
    HDLDesignParser(HDLDesignParser const& rhs);
    HDLDesignParser& operator=(HDLDesignParser const& rhs);

    /*!
     *  Parses all the component instances in the design.
     */
    void parseComponentInstances();

	void findInterconnections();

	void assignInterconnections();

	void findInternalAdhocs();

	void assignInternalAdHocs();

	void parseHierarchicallAdhocs();

    /*!
     *  Finds the physical bounds for a port map in an instance.
     *
     *      @param [in] instanceName    The name of the instance.
     *      @param [in] port			The port for the which to find the physicalbounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
    QPair<QString, QString> physicalPortBoundsInInstance(QSharedPointer<GenerationInstance> instance, QSharedPointer<View> activeView, QSharedPointer<Component> component, QSharedPointer<Port> port) const;

    /*!
     *  Finds the logical bounds for a port map in an instance.
     *
     *      @param [in] instanceName    The name of the instance.
     *      @param [in] portMap         The port map for the which to find the logical bounds.
     *
     *      @return The port bounds for the port map in an instance.
     */
	QPair<QString, QString> logicalPortBoundsInInstance(QSharedPointer<GenerationInstance> instance, QSharedPointer<View> activeView, QSharedPointer<Component> component, QSharedPointer<PortMap> portMap) const;

	bool connectTieOff(QString tieOff, QSharedPointer<Port> port, DirectionTypes::Direction requiredDirection, QString& value);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

     //! The component library.
     LibraryInterface* library_;

     //! The top level component.
     QSharedPointer<Component> topComponent_;

     //! The design to parse.
	 QSharedPointer<Design> design_;

	 //! The design configuration to parse.
	 QSharedPointer<DesignConfiguration> designConf_;

     //! The active view for top component.
     QSharedPointer<View> topComponentView_;

};

#endif // HDLDESIGNPARSER_H
