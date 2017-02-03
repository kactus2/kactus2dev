//-----------------------------------------------------------------------------
// File: MetaComponent.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 03.02.2017
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#ifndef METACOMPONENT_H
#define METACOMPONENT_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Port.h>

#include <Plugins/common/PortSorter/PortSorter.h>
#include <Plugins/common/HDLParser/HDLParserCommon.h>

#include <QSharedPointer>
#include <QTextStream>
#include <QList>

class ExpressionFormatter;

//-----------------------------------------------------------------------------
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------
class MetaComponent
{
public:
    
	/*!
	 *  The constructor.
     *
     *      @param [in] messages            For message output.
	 *      @param [in] component           The instantiated component.
     *      @param [in] activeView          The active view for the component. May be null
	 */
    MetaComponent(MessagePasser* messages,
        QSharedPointer<Component> component,
        QSharedPointer<View> activeView);

	//! The destructor.
	~MetaComponent();
    
    /*!
     *  Formats the parameters, ports and remap states, so that they are writable HDL.
     */
    void formatComponent();

    /*!
     *  Sorts list of parameters based on their interdependencies.
     *
     *      @param [in] refParameters		    The list containing the parameters, to be used as a reference.
     *      @param [out] sortParameters         The list containing the same parameters as in refParameters, that will be sorted.
     */
	static void sortParameters(QList<QSharedPointer<Parameter> >& refParameters,
        QSharedPointer<QList<QSharedPointer<Parameter> > > sortParameters);
    
    /*!
     *  Returns the associated component.
     */
    QSharedPointer<Component> getComponent() const { return component_; }

    /*!
     *  Returns list of culled parameters
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() { return parameters_; }

    /*!
     *  Returns list of culled ports.
     */
    QSharedPointer<QMap<QString,QSharedPointer<MetaPort> > > getPorts() { return ports_; }
    
    /*!
     *  Returns name of the associated HDL module.
     */
    QString getModuleName() const { return moduleName_; }

    /*!
     *  Sets name of the associated HDL module.
     */
    void setModuleName(QString const& val) { moduleName_ = val; }
    
    /*!
     *  Returns list of formatted remap states.
     */
    QSharedPointer<QList<QSharedPointer<FormattedRemapState> > > getRemapStates() const { return remapStates_; }
protected:

    //! The messages.
    MessagePasser* messages_;

private:
	// Disable copying.
	MetaComponent(MetaComponent const& rhs);
    MetaComponent& operator=(MetaComponent const& rhs);
    
    /*!
     *  Culls and copies parameter declarations of the component and active instantiation if any exists.
     */
    virtual void cullParameters();
    
    /*!
    *  Parses the found parameter declarations.
    */
    virtual void formatParameters(QSharedPointer<ExpressionFormatter> formatter);
    
    /*!
    *  Culls and parses the ports of the component.
    */
    virtual void formatPorts(QSharedPointer<ExpressionFormatter> formatter);

    /*!
    *  Goes through the remap states, connects remaps to correct ports.
    */
    void parseRemapStates(QSharedPointer<ExpressionFormatter> formatter);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The active view of the instance.
    QSharedPointer<View> activeView_;
    //! The matching IP-XACT component.
    QSharedPointer<Component> component_;
    //! The parsed parameters that are to be used with the instance. CEVs used where applicable.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
    //! The ports of the component keyed with its physical name.
    QSharedPointer<QMap<QString,QSharedPointer<MetaPort> > > ports_;
    //! The module name for HDL.
    QString moduleName_;
    //! The component instantiation referred by the active view.
    QSharedPointer<ComponentInstantiation> activeInstantiation_;
    //! The formatted remap states.
    QSharedPointer<QList<QSharedPointer<FormattedRemapState> > > remapStates_;
};

#endif // METACOMPONENT_H
