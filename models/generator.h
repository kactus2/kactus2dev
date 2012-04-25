/* 
 *
 *  Created on: 11.8.2010
 *      Author: Antti Kamppi
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <QString>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Parameter;

/*! \brief Equals the spirit:generator element in IP-Xact specification.
 *
 * This class can be used to represent spirit:generator element or it can
 * be used as a base class to generalize ComponentGenerator class to
 * represent spirit:componentGenerator element.
 */
class Generator {

public:

	//! \brief Indicates the type of API used by the generator.
	enum ApiType {
		TGI,
		NONE
	};

	/*! \brief The constructor
	 *
	 * \param generatorNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Generator(QDomNode &generatorNode);

	//! \brief Copy constructor
	Generator(const Generator &other);

	//! \brief Assignment operator
	Generator &operator=(const Generator &other);

	/*! \brief The destructor
	 *
	 */
	virtual ~Generator();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the API type of the generator
	 *
	 * \return enum ApiType containing the API type.
	 */
	virtual ApiType getApiType() const;

	/*! \brief Get the path of the generator executable
	 *
	 * \return QString containing the path
	 */
	virtual QString getGeneratorExe() const;

	/*! \brief Get the hidden setting of this Component Generator
	 *
	 * \return boolean value of the hidden setting
	 */
	virtual bool getHidden() const;

	/*! \brief Get the name of the Component Generator
	 *
	 * \return QString containing the name
	 */
	virtual QString getName() const;

	/*! \brief Get list of the parameters for this Component Generator
	 *
	 * \return QList containing pointers to the parameters.
	 */
	virtual const QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get the phase of this Component Generator
	 *
	 *	Negative phase number means undefined phase.
	 *
	 * \return Double containing the phase number.
	 */
	virtual double getPhase() const;

	/*! \brief Set the ApiType for this Component Generator
	 *
	 * \param apiType enum ApiType containing the API type.
	 */
	virtual void setApiType(ApiType apiType);

	/*! \brief Set the path to the generator executable for this generator.
	 *
	 * \param generatorExe QString containing the path to the generator
	 * executable.
	 */
	virtual void setGeneratorExe(const QString &generatorExe);

	/*! Set the hidden setting for this Component Generator.
	 *
	 * \param hidden boolean value of the hidden element.
	 */
	virtual void setHidden(bool hidden);

	/*! \brief Set the name for the Component Generator.
	 *
	 * \param name QString containing the name.
	 */
	virtual void setName(const QString &name);

	/*! \brief Set the parameters for this Component Generator.
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param parameters QList containing pointers to the new parameters.
	 */
	virtual void setParameters(const QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Set the phase of this Component Generator
	 *
	 * Negative phase number means undefined phase.
	 *
	 * \param phase Double type value of the phase.
	 */
	virtual void setPhase(double phase);

	/*! \brief Set the description of this Component Generator
	 *
	 * \param descriptions QString containing the description.
	 */
	virtual void setDescription(const QString &description);

	/*! \brief Get the description of this Component Generator.
	 *
	 * \return QString containing the description.
	 */
	virtual QString getDescription() const;

	/*! \brief Get the display name
	 *
	 * \return QString containing the display name.
	 */
	virtual QString getDisplayName() const;

	/*! \brief Set the display name.
	 *
	 * \param QString containing the display name.
	 */
	virtual void setDisplayName(const QString &displayName);

protected:

	/*!
	 * OPTIONAL spirit:hidden
	 * Specifies if this generator can be run as standalone or if it must be
	 * run as part of a generator chain.
	 */
	bool hidden_;

	/*!
	 * MANDATORY spirit:name
	 * Identifies the component generator
	 */
	QString name_;

	/*!
	 * MANDATORY spirit:displayName
	 * A short descriptive text to be associated with the containing element.
	 */
	QString displayName_;

	/*!
	 * OPTIONAL spirit:description
	 * Contains a textual description of the Component Generator.
	 */
	QString description_;

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
};

#endif /* GENERATOR_H_ */
