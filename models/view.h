/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <LibraryManager/vlnv.h>

#include <models/generaldeclarations.h>
#include <models/filebuilder.h>

#include <QString>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QStringList>

class VLNV;
class Parameter;

/*! \brief Equals the spirit:view element in IP-Xact specification
 *
 * Specifies a representation level of a component.
 */
class View {

public:

	/*! \brief The constructor
	 *
	 * \param viewNode A reference to a QDomNode to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	View(QDomNode &viewNode);

	/*! \brief The constructor
	 *
	 * After calling this constructor the other mandatory values must be set
	 * manually. If mandatory values are not set, the document can't be written
	 * to disk.
	 *
	 * \param name The name of the view.
	 */
	View(const QString name);

	/*! \brief The default constructor.
	 *
	*/
	View();

	//! \brief Copy constructor
	View(const View &other);

	//! \brief Assignment operator
	View &operator=(const View &other);

	/*! \brief The destructor
	 *
	 */
	~View();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the view is in a valid state.
	 * 
	 * \param fileSetNames Contains the names of the file sets of the component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& fileSetNames, 
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the view is in a valid state.
	 * 
	 * \param fileSetNames Contains the names of the file sets of the component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& fileSetNames) const;

	/*! \brief Get list of the envIdentifiers for this view
	 *
	 * \return QStringList containing the envIdentifiers
	 */
	const QStringList& getEnvIdentifiers() const;

	/*! \brief Get list of the envIdentifiers for this view
	 *
	 * \return QStringList containing the envIdentifiers
	 */
	QStringList& getEnvIdentifiers();

	/*! \brief Get list of the fileSetRefs for this view
	 *
	 * \return QList containing the fileSetRefs
	 */
	QStringList getFileSetRefs() const;

	/*! \brief Get pointer to the hierarchical desing for this view
	 *
	 * \return A pointer to the vlnv of a hierarchical design if the view is
	 * used to reference a hierachical design. If not a null pointer is
	 * returned.
	 */
	VLNV getHierarchyRef() const;

	/*! \brief Get the language
	 *
	 * \return QString containing the language
	 */
	QString getLanguage() const;

	/*! \brief Get the strict attribute of the language element
	 *
	 * \return boolean value of the strict attribute
	 */
	bool getLanguageStrict() const;

	/*! \brief Get the model name
	 *
	 * \return QString containing the model name
	 */
	QString getModelName() const;

	/*! \brief Get the name of the view
	 *
	 * \return QString containing the name
	 */
	QString getName() const;

	/*! \brief Get the display name of the view
	 *
	 * \return QString containing the display name.
	*/
	QString getDisplayName() const;

	/*! \brief Get the description of the view.
	 *
	 * \return QString containing the description.
	*/
	QString getDescription() const;

	/*! \brief Get the default file builders for this view.
	 *
	 * \return QList containing pointers to the file builders for this view.
	*/
	QList<QSharedPointer<FileBuilder> >& getDefaultFileBuilders();

	/*! \brief Get the default file builders for this view.
	 *
	 * \return QList containing pointers to the file builders for this view.
	*/
	const QList<QSharedPointer<FileBuilder> >& getDefaultFileBuilders() const;

	/*! \brief Set the default file builders for this view.
	 *
	 * \param defaultFileBuilders QList containing pointer to the file builders.
	 *
	*/
	void setDefaultFileBuilders(
		QList<QSharedPointer<FileBuilder> >& defaultFileBuilders);

	/*! \brief Set the display name for the view.
	 *
	 * \param displayName Contains the display name to set.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Set the description for the view.
	 *
	 * \param description Contains the description of the view.
	 *
	*/
	void setDescription(const QString& description);

	/*! \brief Get list of the parameters for this view
	 *
	 * \return QList containing the parameters for this view.
	 */
	QList<QSharedPointer<Parameter> >& getParameters();

	/*! \brief Get list of the parameters for this view
	 *
	 * \return QList containing the parameters for this view.
	 */
	const QList<QSharedPointer<Parameter> >& getParameters() const;

	/*! \brief Set the envIdentifiers for this view
	 *
	 * Calling this function will delete the old envIdentifiers.
	 *
	 * \param envIdentifiers QList containing the envIdentifiers
	 */
	void setEnvIdentifiers(const QStringList& envIdentifiers);

	/*! \brief Set the fileSetRefs for this view
	 *
	 * Calling this function will delete the old fileSetRefs
	 *
	 * \param fileSetRefs QList containing the old fileSetRefs
	 */
	void setFileSetRefs(const QList<QString> &fileSetRefs);

	/*! \brief Set the hierarchyRef for this view
	 *
	 * Calling this function will delete the old hierarchyRef
	 *
	 * \param hierarchyRef A pointer to the new hierarchyRef
	 */
	void setHierarchyRef(const VLNV& hierarchyRef);

	/*! \brief Set the language of this view
	 *
	 * \param language QString containing the language
	 */
	void setLanguage(const QString &language);

	/*! \brief Set the strict attribute of the language element
	 *
	 * \param languageStrict boolean value for the attribute
	 */
	void setLanguageStrict(bool languageStrict);

	/*! \brief Set the model name
	 *
	 * \param modelName QString containing the new model name
	 */
	void setModelName(const QString &modelName);

	/*! \brief Set the name for the view
	 *
	 * \param name QString containing the new name.
	 */
	void setName(const QString &name);

	/*! \brief Set the parameters for this view
	 *
	 * Calling this function will delete the old parameters.
	 *
	 * \param parameters QList containing the new parameters.
	 */
	void setParameters(const QList<QSharedPointer<Parameter> > &parameters);

	/*! \brief Add a new envIdentifier for this view.
	 *
	 * \param envIdentifier The envIdentifier to add.
	 */
	void addEnvIdentifier(const QString envIdentifier);

	/*! \brief Add a new reference to a file set to this view.
	 *
	 * \param fileSetRef The name of the file set.
	 */
	void addFileSetRef(const QString fileSetRef);

	/*! \brief Is the view a hierarchical one or not.
	 *
	 * \return True if view is hierarchical (contains a hierarchyRef)
	*/
	bool isHierarchical() const;

	/*! \brief Get the reference to a view that contains the implementation.
	 *
	 * \return QString Name of the view that contains this component's implementation.
	*/
	QString getTopLevelView() const;

	/*! \brief Set the reference to a top level implementation view.
	 *
	 * \param viewName Name of the view that contains the top level implementation.
	 *
	*/
	void setTopLevelView(const QString& viewName);

	/*! \brief Get the name group struct of this view.
	 *
	 * \return Reference to the struct containing the name group.
	*/
	General::NameGroup& getNameGroup();

	/*! \brief Get the name group struct of this view.
	 *
	 * \return Reference to the struct containing the name group.
	*/
	const General::NameGroup& getNameGroup() const;

	/*! \brief Clear the hierarchical reference and the top level reference.
	 *
	*/
	void clearHierarchy();

private:

	//! \brief Contains the name, display name and description of view.
	General::NameGroup nameGroup_;

	/*!
	 * MANDATORY spirit:envIdentifier
	 * Designates and qualifies information about how this model view is
	 * deployed in a particular tool environment.
	 */
	QStringList envIdentifiers_;

	/*!
	 * OPTIONAL spirit:language
	 * Specifies the hardware description language used for a specific view
	 */
	QString language_;

	/*!
	 * OPTIONAL spirit:strict
	 * If true the language shall be strictly enforced
	 * default = false
	 */
	bool languageStrict_;

	/*!
	 * OPTIONAL spirit:modelName
	 * A language-specific identifier for the model
	 */
	QString modelName_;

	/*!
	 * OPTIONAL spirit:fileSetRef
	 * An unbounded list of references to fileSet names
	 */
	QList<QString> fileSetRefs_;

	//! \brief OPTIONAL An unbounded list of references to constraint sets
	//! spirit:constraintSetRef
	QList<QString> constraintSetRefs_;

	/*!
	 * OPTIONAL spirit:parameters
	 * Any additional parameters that describe the view.
	 */
	QList<QSharedPointer<Parameter> > parameters_;

	/*!
	 * MANDATORY spirit:hierarchyRef
	 * References a hierarchical design, mandatory only if the view is used to
	 * reference a hierarchical design.
	 */
	VLNV hierarchyRef_;

	/*! \brief Contains the defaultFileBuilders
	 * OPTIONAL spirit:defaultFileBuilder
	 * Specifies the unbounded default build commands for the files within
	 * this file set.
	 */
	QList<QSharedPointer<FileBuilder> > defaultFileBuilders_;

	/*! \brief References another view that contains files for the top-level implementation
	 * OPTIONAL kactus2:topLevelViewRef
	 */
	QString topLevelViewRef_;
};


#endif /* VIEW_H_ */
