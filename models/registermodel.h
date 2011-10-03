/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#ifndef REGISTERMODEL_H_
#define REGISTERMODEL_H_

#include <QList>
#include <QDomNode>
#include <QString>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class Parameter;

/*! \brief A base class for Register, RegisterFile and AlternateRegister classes
 *
 * This class holds the information that is common for these three classes. This
 * class needs to be inherited by one of the mentioned classes, this doesn't
 * represent any IP-Xact element alone.
 *
 * Base classes and their mathing elements in IP-Xact:
 *
 * Register = 			spirit:register
 * AlternateRegister = 	spirit:alternateRegister
 * RegisterFile = 		spirit:registerFile
 */
class RegisterModel {

public:

	/*! \brief The constructor
	 *
	 * \param registerNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	RegisterModel(QDomNode& registerNode);

	//! The copy constructor
	RegisterModel(const RegisterModel& other);

	//! The assignment operator
	RegisterModel& operator=(const RegisterModel& other);

	//! \brief The destructor
	virtual ~RegisterModel();

	/*! \brief Get the description of the register.
	 *
	 * \return QString containing the description
	 */
    QString getDescription() const;

    /*! \brief Get the display name of the register.
     *
     * \return QString containing the display name.
     */
    QString getDisplayName() const;

    /*! \brief Get the id of the register.
     *
     * \return QString containing the id
     */
    QString getId() const;

    /*! \brief Get the name of the register.
     *
     * \return QString containing the name.
     */
    QString getName() const;

    /*! \brief Get the parameters of the register.
     *
     * \return QList containing pointers to the parameters.
     */
    const QList<QSharedPointer<Parameter> >& getParameters() const;

    /*! \brief Set the description of the register.
     *
     * \param description QString containing the description.
     */
    void setDescription(const QString& description);

    /*! \brief Set the display name of the register.
     *
     * \param displayName QString containing the name
     */
    void setDisplayName(const QString& displayName);

    /*! \brief Set the id of the register.
     *
     * \param id QString containing the id.
     */
    void setId(const QString& id);

    /*! \brief Set the name for the register.
     *
     * \param name QString containing the name.
     */
    void setName(const QString& name);

    /*! \brief Set the parameters for the register.
     *
     * Calling this function will erase old parameters.
     *
     * \param parameters QList containing new parameters
     */
    void setParameters(const QList<QSharedPointer<Parameter> >& parameters);

    /*! \brief Writes the base class information with the given writer
     *
     * This function must be called by the inherited class right after writing
     * the startElement() call because this function writes the attribute for
     * the base element.
     *
     * \param writer A reference to a QXmlStreamWriter instance to write the
     * contents of the class into file.
     *
     * Exception guarantee: basic
     * \exception Write_error Occurs if this class or one of it's member
     * classes is not valid IP-Xact at the moment of writing.
     */
    virtual void write(QXmlStreamWriter& writer);

private:

	/*! \brief Uniquely identifies an element within this object.
	 * OPTIONAL spirit:id
	 */
	QString id_;

	/*! \brief Name of the register.
	 * MANDATORY spirit:name
	 */
	QString name_;

	/*! \brief Contains the display name.
	 * OPTIONAL spirit:displayName
	 */
	QString displayName_;

	/*! \brief Contains the description.
	 * OPTIONAL spirit:description
	 */
	QString description_;

	/*! \brief Contains the parameters.
	 * OPTIONAL spirit:parameters
	 */
	QList<QSharedPointer<Parameter> > parameters_;
};

#endif /* REGISTERMODEL_H_ */
