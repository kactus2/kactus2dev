//-----------------------------------------------------------------------------
// File: ComponentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Implementation for ipxact:componentGenerator element.
//-----------------------------------------------------------------------------

#ifndef COMPONENTGENERATOR_H
#define COMPONENTGENERATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/BooleanValue.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QList>
#include <QString>

class Choice;
class Parameter;
//-----------------------------------------------------------------------------
//! Implementation for ipxact:componentGenerator element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentGenerator : public NameGroup, public Extendable
{

public:

	//! Indicates the type of API used by the generator.
	enum ApiType 
    {
		TGI_2014_BASE,
		TGI_2014_EXTENDED,
		TGI_2009,
		NONE,
        EMPTY_API_TYPE
	};

	//! Specifies if the generator shall be run once for all instances or once for each instance of this component.
	enum Scope
    {
		INSTANCE,
		ENTITY,
        NO_SCOPE
	};

	//! The constructor.
	ComponentGenerator();

	//! Copy constructor.
	ComponentGenerator(const ComponentGenerator &other);

	//! Assignment operator.
	ComponentGenerator &operator=(const ComponentGenerator &other);

	//! The destructor.
	~ComponentGenerator();

    	/*! Set the hidden setting for this component generator.
	 *
	 *      @param [in] hidden  The hidden value to set.
	 */
	void setHidden(bool hidden);

	/*! Get the hidden setting of this component generator.
	 *
	 *      @return The hidden setting.
	 */
	BooleanValue getHidden() const;

	/*! Set the scope of this component generator.
	 *
	 *      @param [in] scope The scope to be set.
	 */
	void setScope(Scope scope);

	/*! Get the scope of this component generator.
	 *
	 *      @return  The scope of the generator.
	 */
	Scope getScope() const;

	/*! Set the phase for the component generator.
	 *
	 *      @param [in] phase   The phase value to set.
	 */
	void setPhase(QString const& phaseExpression);

    /*! Get the phase of the component generator.
	 *
	 *      @return The generator phase.
	 */
	QString getPhase() const;

	/*! Get the parameters for this component generator.
	 *
	 *      @return QList containing pointers to the parameters.
	 */
	QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters();
    
	/*! Set the ApiType for this Component Generator
	 *
	 *      @param [in] apiType enum ApiType containing the API type.
	 */
	void setApiType(ApiType apiType);

    /*! Get the API type of the generator.
	 *
	 *      @return The generator API type.
	 */
	ApiType getApiType() const;

    /*! Set the supported transport methods.
	 *
	 *      @param [in] transportMethods The transport methods to set.
	 */
	void setTransportMethods(QStringList const& transportMethods);

	/*! Get the supported transport methods.
	 *
	 *      @return The supported transport methods.
	 */
	QStringList getTransportMethods() const;
    
	/*! Set the path to the generator executable for this generator.
	 *
	 *      @param [in] generatorExe The path to the generator executable.
	 */
	void setGeneratorExe(QString const& generatorExe);

	/*! Get the path of the generator executable.
	 *
	 *      @return The generator executable path.
	 */
	QString getGeneratorExe() const;

	/*! Set the groups for this component generator.
	 *
	 *      @param [in] groups The names of the groups for this generator.
	 */
	void setGroups(QStringList const& groups);

	/*! Get list of the groups for this Component Generator
	 *
	 *      @return The names of the groups this generator belongs to.
	 */
	QStringList getGroups() const;

private:

	//! Specifies if this generator can be run as standalone or if it must be run as part of a generator chain.
	BooleanValue hidden_;
    
	//! Indicates if the generator shall be run once for all instances or once for each instance of this component.
	Scope scope_;

	//! Determines the generator phase in a sequence of generator runs. 
	QString phase_;

	// Specifies any component generator specific parameters.
	QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;

	//! Indicates the type of API used by the generator.
	ApiType apiType_;
    
    //! The alternate SOAP transport protocols supported by the component generator.
	QStringList transportMethods_;

	//! The path to the generator executable.
	QString generatorExe_;

    //! Names for assigning this generator to a group with other generators.
	QList<QString> groups_;

};

#endif /* COMPONENTGENERATOR_H */
