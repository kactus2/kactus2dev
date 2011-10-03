/* 
 *
 *  Created on: 3.8.2010
 *      Author: Antti Kamppi
 */

#ifndef SERVICETYPEDEF_H_
#define SERVICETYPEDEF_H_

#include <QDomNode>
#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Parameter;

/*! \brief Equals the spirit:serviceTypeDef element in IP-Xact Specification
 *
 * Defines a single service type definition.
 */
class ServiceTypeDef {

public:

	/*! \brief The constructor
	 *
	 * \param serviceTypeNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	ServiceTypeDef(QDomNode &serviceTypeNode);

	//! \brief Copy constructor
	ServiceTypeDef(const ServiceTypeDef &other);

	//! \brief Assignment operator
	ServiceTypeDef &operator=(const ServiceTypeDef &other);

	/*! \brief The destructor
	 *
	 */
	~ServiceTypeDef();

	/*! \brief Get list of the parameters for the service type
	 *
	 * \return QList conataining pointers to the parameters
	 */
	const QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get the type definition of the serviceTypeDef
	 *
	 * \return QString containing the type definition
	 */
	QString getTypeDefinition() const;

	/*! \brief Get the name of the serviceTypeDef
	 *
	 * \return QString containting the name.
	 */
	QString getTypeName() const;

	/*! \brief Set the parameters for the serviceTypeDef
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param parameters QList containing pointers to the new parameters
	 */
	void setParameters(const QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Set the type definition for this serviceTypeDef
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param typeDefinition QString containing the type definition
	 */
	void setTypeDefinition(const QString &typeDefinition);

	/*! \brief Set the name for the serviceTypeDef
	 *
	 * \param typeName QString containing the name
	 */
	void setTypeName(const QString &typeName);

	/*! \brief Get the value of the constrained attribute
	 *
	 * \return boolean value of the constrained
	 */
    bool getConstrained() const;

    /*! \brief Get the value of the implicit attribute
     *
     * \return boolean value of the implicit
     */
    bool getImplicit() const;

    /*! \brief Set the value of the constrained attribute
     *
     * \param constrained Boolean setting for the constrained
     */
    void setConstrained(bool constrained);

    /*! \brief Set the value of the implicit attribute
     *
     * \param implicit Boolean setting for the implicit
     */
    void setImplicit(bool implicit);

    /*! \brief Write the contents of the class using the writer.
     *
     * Uses the specified writer to write the class contents into file as valid
     * IP-Xact.
     *
     * \param writer A reference to a QXmlStreamWriter instance that is used to
     * write the document into file.
     *
     * Exception guarantee: basic
     * \exception Write_error Occurs if class or one of it's member classes is
     * not valid IP-Xact in the moment of writing.
     */
    void write(QXmlStreamWriter& writer);

private:

	/*!
	 * MANDATORY spirit:typeName
	 * Defines the name of the service type
	 */
	QString typeName_;

	/*!
	 * OPTIONAL spirit:typeDefinition
	 * indicates a location where the type is defined.
	 */
	QString typeDefinition_;

	/*!
	 * OPTIONAL spirit:parameter
	 * Specifies any service type parameters
	 */
	QList<QSharedPointer<Parameter> > parameters_;

	/*!
	 * OPTIONAL spirit:constrained
	 * Indicates if the port type has constrained the number of bits in the
	 * vector
	 * default = false
	 */
	bool constrained_;

	/*!
	 * OPTIONAL spirit:implicit
	 * Indicates that the netlister should not declare this service in a
	 * language-specific top-level netlist.
	 * default = false
	 */
	bool implicit_;

};

#endif /* SERVICETYPEDEF_H_ */
