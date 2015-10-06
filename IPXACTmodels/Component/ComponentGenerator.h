/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTGENERATOR_H_
#define COMPONENTGENERATOR_H_

#include <IPXACTmodels/generator.h>
#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QList>
#include <QString>

class Choice;

/*! \brief Equals the spirit:componentGenerator element in IP-Xact specification
 *
 * Defines a generator that is assigned and may be run on this component.
 * This class is a generalization of class Generator and uses it's services.
 */
class IPXACTMODELS_EXPORT ComponentGenerator : public NameGroup, public Extendable
{

public:

	//! \brief Indicates the type of API used by the generator.
	enum ApiType {
		TGI_2014_BASE,
		TGI_2014_EXTENDED,
		TGI_2009,
		NONE
	};

	/*! \brief Used the specify how the generator is  run
	 * Specifies if the generator shall be run once for all instances or
	 * once for each instance of this component.
	 */
	enum Instance {
		INSTANCE,
		ENTITY
	};

	//! \brief The constructor
	ComponentGenerator();

	//! \brief Copy constructor
	ComponentGenerator(const ComponentGenerator &other);

	//! \brief Assignment operator
	ComponentGenerator &operator=(const ComponentGenerator &other);

	/*! \brief The destructor
	 *
	 */
	~ComponentGenerator();

	/*! \brief Check if the component generator is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the component generator is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*! \brief Get list of the groups for this Component Generator
	 *
	 * \return QList containing the names of the groups this generator belongs
	 * to
	 */
	const QList<QString>& getGroups();

	/*! \brief Get the scope of this Component Generator
	 *
	 * \return enum Instance containing the scope.
	 */
	Instance getScope() const;

	/*! \brief Set the groups for this Component Generator
	 *
	 * Calling this function will delete the old groups.
	 *
	 * \param groups QList containing names of the groups for this generator.
	 */
	void setGroups(QList<QString> &groups);

	/*! \brief Set the scope of this Component Generator.
	 *
	 * \param scope The scope to be set.
	 */
	void setScope(Instance scope);

	/*! \brief Get the API type of the generator
	 *
	 * \return enum ApiType containing the API type.
	 */
	ApiType getApiType() const;

	/*! \brief Get the path of the generator executable
	 *
	 * \return QString containing the path
	 */
	QString getGeneratorExe() const;

	/*! \brief Get the hidden setting of this Component Generator
	 *
	 * \return boolean value of the hidden setting
	 */
	bool getHidden() const;

	/*! \brief Get list of the parameters for this Component Generator
	 *
	 * \return QList containing pointers to the parameters.
	 */
	const QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get the phase of this Component Generator
	 *
	 *	Negative phase number means undefined phase.
	 *
	 * \return Double containing the phase number.
	 */
	double getPhase() const;

	/*! \brief Set the ApiType for this Component Generator
	 *
	 * \param apiType enum ApiType containing the API type.
	 */
	void setApiType(ApiType apiType);

	/*! \brief Set the path to the generator executable for this generator.
	 *
	 * \param generatorExe QString containing the path to the generator
	 * executable.
	 */
	void setGeneratorExe(const QString &generatorExe);

	/*! Set the hidden setting for this Component Generator.
	 *
	 * \param hidden boolean value of the hidden element.
	 */
	void setHidden(bool hidden);

	/*! \brief Set the parameters for this Component Generator.
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param parameters QList containing pointers to the new parameters.
	 */
	void setParameters(const QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Set the phase of this Component Generator
	 *
	 * Negative phase number means undefined phase.
	 *
	 * \param phase Double type value of the phase.
	 */
	void setPhase(double phase);

	/*! \brief Get the supported transport methods.
	 *
	 * \return See above.
	 */
	const QStringList& getTransportMethods() const;

	/*! \brief Set the supported transport methods.
	 *
	 * \param See above.
	 */
	void setTransportMethods(QStringList transportMethods);

private:

	/*! \brief OPTIONAL spirit:scope
	 * Indicates if the generator shall be run once for all instances or once
	 * for each instance of this component.
	 */
	Instance scope_;

	/*! \brief OPTIONAL spirit:group
	 * An unbounded list of names used to assign this generator to a group
	 * with other generators.
	 */
	QList<QString> groups_;

	/*!
	 * OPTIONAL spirit:hidden
	 * Specifies if this generator can be run as standalone or if it must be
	 * run as part of a generator chain.
	 */
	bool hidden_;

	/*!
	 * OPTIONAL spirit:phase
	 * Determines the sequence in which the generators are run
	 */
	double phase_;

	/*!
	 * OPTIONAL spirit:parameters
	 * Specifies any ComponentGenerator parameters.
	 */
	QList<QSharedPointer<Parameter> > parameters_;

	/*!
	 * OPTIONAL spirit:apiType
	 * Indicates the type of API used by the generator.
	 */
	ApiType apiType_;

	/*!
	 * MANDATORY spirit:generatorExe
	 * Contains the path to the generator executable.
	 */
	QString generatorExe_;

    /*!
     * OPTIONAL spirit: vendorExtensions
     * Generator vendor extensions.
     */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

    /*!
     * The alternate SOAP transport protocols.
     */
	QStringList transportMethods_;
};

#endif /* COMPONENTGENERATOR_H_ */
