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

#include <Plugins/common/HDLParser/HDLParserCommon.h>

#include <QTextStream>

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
     *      @param [out] sortParameters         The list containing the parameters that will be sorted.
     */
	static void sortParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > sortParameters);
    
    /*!
     *  Returns the associated component.
     */
    QSharedPointer<Component> getComponent() const { return component_; }
    
    /*!
     *  Returns the active view.
     */
    QSharedPointer<View> getActiveView() const { return activeView_; }

    /*!
     *  Returns list of culled parameters
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() { return parameters_; }

    /*!
     *  Returns list of culled module parameters
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getModuleParameters() { return moduleParameters_; }

    /*!
     *  Returns list of resolved meta parameters
     */
    QSharedPointer<QMap<QString,QSharedPointer<Parameter> > > getMetaParameters() { return metaParameters_; }

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

    /*!
     *  Returns list of culled file sets
     */
    QSharedPointer<QList<QSharedPointer<FileSet> > > getFileSets() { return fileSets_; }

protected:

    //! The messages.
    MessagePasser* messages_;

private:
	// Disable copying.
	MetaComponent(MetaComponent const& rhs);
    MetaComponent& operator=(MetaComponent const& rhs);
    
    /*!
     *  Associates the meta object file sets pointed by the active component instance.
     */
    virtual void parsesFileSets();
    
    /*!
     *  Culls and copies parameter declarations of the component and active instantiation if any exists.
     */
    virtual void cullParameters();
    
    /*!
     *  Formats the found parameter declarations.
     */
    virtual void formatParameters(QSharedPointer<ExpressionFormatter> formatter);
    
    /*!
     *  Culls and formats the ports of the component.
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
    //! The parameters, which are either formatted or completely parsed.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
    QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters_;
    //! These parameters are actually generated.
    QSharedPointer<QMap<QString,QSharedPointer<Parameter> > > metaParameters_;
    //! The ports of the component keyed with its physical name.
    QSharedPointer<QMap<QString,QSharedPointer<MetaPort> > > ports_;
    //! The filesets referred by the activeInstantiation_;
    QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets_;
    //! The module name for HDL.
    QString moduleName_;
    //! The component instantiation referred by the active view.
    QSharedPointer<ComponentInstantiation> activeInstantiation_;
    //! The formatted remap states.
    QSharedPointer<QList<QSharedPointer<FormattedRemapState> > > remapStates_;
};

#endif // METACOMPONENT_H
