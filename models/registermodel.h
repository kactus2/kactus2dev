/* 
 *
 *  Created on: 26.10.2010
 *      Author: Antti Kamppi
 */

#ifndef REGISTERMODEL_H_
#define REGISTERMODEL_H_

#include <common/Global.h>
#include <models/generaldeclarations.h>

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
 * Base classes and their matching elements in IP-Xact:
 *
 * Register = 			spirit:register
 * AlternateRegister = 	spirit:alternateRegister
 * RegisterFile = 		spirit:registerFile
 */
class KACTUS2_API RegisterModel {

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

	//! \brief The default constructor.
	RegisterModel();

	//! The copy constructor
	RegisterModel(const RegisterModel& other);

	//! The assignment operator
	RegisterModel& operator=(const RegisterModel& other);

	//! \brief The destructor
	virtual ~RegisterModel();

	/*! \brief Clone the register model and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit register model.
	 *
	 * \return QSharedPointer<RegisterModel> Pointer to the cloned register model.
	*/
	virtual QSharedPointer<RegisterModel> clone() = 0;

	/*! \brief Writes the base class information with the given writer
	*
	* This function must be called by the inherited class right after writing
	* the startElement() call because this function writes the attribute for
	* the base element.
	*
	* \param writer A reference to a QXmlStreamWriter instance to write the
	* contents of the class into file.
	*/
	virtual void write(QXmlStreamWriter& writer);

	/*! \brief Check if the register model is in a valid state.
	 *
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, 
		const QString& parentIdentifier) const = 0;

	/*! \brief Check if the register model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const = 0;

	/*! \brief Get the description of the register.
	 *
	 * \return QString containing the description
	 */
    virtual QString getDescription() const;

    /*! \brief Get the display name of the register.
     *
     * \return QString containing the display name.
     */
    virtual QString getDisplayName() const;

    /*! \brief Get the id of the register.
     *
     * \return QString containing the id
     */
    virtual QString getId() const;

    /*! \brief Get the name of the register.
     *
     * \return QString containing the name.
     */
    virtual QString getName() const;

    /*! \brief Get the parameters of the register.
     *
     * \return QList containing pointers to the parameters.
     */
    virtual const QList<QSharedPointer<Parameter> >& getParameters() const;

    /*! \brief Set the description of the register.
     *
     * \param description QString containing the description.
     */
    virtual void setDescription(const QString& description);

    /*! \brief Set the display name of the register.
     *
     * \param displayName QString containing the name
     */
    virtual void setDisplayName(const QString& displayName);

    /*! \brief Set the id of the register.
     *
     * \param id QString containing the id.
     */
    virtual void setId(const QString& id);

    /*! \brief Set the name for the register.
     *
     * \param name QString containing the name.
     */
    virtual void setName(const QString& name);

    /*! \brief Set the parameters for the register.
     *
     * Calling this function will erase old parameters.
     *
     * \param parameters QList containing new parameters
     */
    virtual void setParameters(const QList<QSharedPointer<Parameter> >& parameters);

protected:

	/*! \brief Uniquely identifies an element within this object.
	 * OPTIONAL spirit:id
	 */
	QString id_;

	//! \brief Contains the name, display name and description of the register item.
	General::NameGroup nameGroup_;

	/*! \brief Contains the parameters.
	 * OPTIONAL spirit:parameters
	 */
	QList<QSharedPointer<Parameter> > parameters_;
};

#endif /* REGISTERMODEL_H_ */
