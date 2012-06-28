/* 
 *
 *  Created on: 30.8.2010
 *      Author: Antti Kamppi
 */

#ifndef DESIGNCONFIGURATION_H_
#define DESIGNCONFIGURATION_H_

#include "librarycomponent.h"
#include "designconfabstractor.h"

#include <common/Global.h>

#include <QSharedPointer>
#include <QList>
#include <QString>
#include <QMap>
#include <QDomDocument>
#include <QDomNode>
#include <QStringList>
#include <QFile>
#include <QMap>

class GeneratorChainConfiguration;
class InterconnectionConf;
class VLNV;

/*! \brief Equals the spirit:designConfiguration element in IP-Xact specification
 *
 * Details the configuration for a design or generator chain description.
 */
class KACTUS2_API DesignConfiguration : public LibraryComponent {

public:

	/*! \brief Equals the spirit:generatorChainConfiguration in IP-Xact.
	 *
	 * GeneratorChainConfiguration is an unbounded list of configuration
	 * information associated with a generatorChain or generator.
	 */
	struct GeneratorChainConfiguration {

		/*! \brief MANDATORY spirit:generatorChainRef
		 * specifies the generator chain for this configuration
		 */
		VLNV generatorChainRef_;

		/*! \brief OPTIONAL spirit:configurableElementValues
		 *
		 * Lists the generator chain's configurable parameter values.
		 * Key = spirit:referenceId
		 * Value = configurableValue
		 */
		QMap<QString, QString> configurableElements_;

		/*! \brief The struct constructor
		 *
		 * \param genChainNode A reference to a QDomNode to parse the
		 * information from
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		GeneratorChainConfiguration(QDomNode& genChainNode);

		//! \brief Copy constructor
		GeneratorChainConfiguration(const GeneratorChainConfiguration& other);

		//! \brief Assignment operator
		GeneratorChainConfiguration& operator=(const GeneratorChainConfiguration& other);

		/*! \brief Check if the generator chain configuration is in a valid state.
		*
		* \param errorList The list to add the possible error messages to.
		* \param parentIdentifier String from parent to help to identify the location of the error.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid(QStringList& errorList, 
			const QString& parentIdentifier) const;

		/*! \brief Check if the generator chain configuration is in a valid state.
		*
		* \return bool True if the state is valid and writing is possible.
		*/
		bool isValid() const;
	};

	/*! \brief Equals the spirit:interconnectionConfiguration in IP-Xact
	 *
	 * InterConnectionConf is an unbounded list of information associated
	 * with interface interconnections.
	 */
	struct InterconnectionConf {

		/*! \brief MANDATORY spirit:interconnectionRef
		 * A reference to a design interconnection
		 */
		QString interconnectionRef_;

		/*! \brief MANDATORY spirit:abstractor
		 * QList containing pointers to the abstractor elements within
		 * interconnection configuration.
		 */
		QList<QSharedPointer<DesignConfAbstractor> > abstractors_;

		/*! \brief The struct constructor
		 *
		 * \param interconnectionNode A reference to a QDomNode to parse
		 * the information from
		 *
		 * Exception guarantee: basic
		 * \exception Parse_error Occurs when a mandatory element is missing in
		 * this class or one of it's member classes.
		 */
		InterconnectionConf(QDomNode& interconnectionNode);

		//! \brief Copy constructor
		InterconnectionConf(const InterconnectionConf& other);

		//! \brief Assignment operator
		InterconnectionConf& operator=(const InterconnectionConf& other);
	};

	/*! \brief The constructor
	 *
	 * \param doc A reference to a QDomDocument that represents the document
	 * that holds the information to parse.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	DesignConfiguration(QDomDocument& doc);

	/*! \brief The constructor
	 *
	 * \param vlnv VLNV to be set for the design configuration
	 *
	*/
	DesignConfiguration(const VLNV& vlnv);

	/*! \brief The default constructor.
	 *
	 * 
	*/
	DesignConfiguration();

	//! \brief Copy constructor
	DesignConfiguration(const DesignConfiguration& other);

	//! \brief Assignment operator
	DesignConfiguration& operator=(const DesignConfiguration& other);

	/*! \brief The destructor
	 *
	 */
	virtual ~DesignConfiguration();

	/*! \brief Clone this design configuration and return pointer to the copy.
	 * 
	 * This is virtual function so it can be used to make a copy of classes that
	 * inherit LibraryComponent.
	 *
	 * \return QSharedPointer<LibraryComponent> Pointer to the cloned design configuration.
	*/
	virtual QSharedPointer<LibraryComponent> clone() const;

	/*! \brief Write the document contents into file.
	*
	* Prints all information stored in this document into specified file
	* generating a valid IP-Xact document.
	*
	* \param file A reference to QFile instance representing the file to write
	* the data into.
	*/
	virtual void write(QFile& file);

	/*! \brief Check the validity of the design configuration.
	 * 
	 * This function should be used to check if the design configuration is in 
	 * valid state before writing the xml to the disk.
	 * 
	 * \param errorList The error messages of the detected errors are appended to
	 * this list.
	 *
	 * \return bool True if the design configuration was valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! \brief Check the validity of the design configuration.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid() const;

	/*! \brief Set the vlnv
	 *
	 * \param vlnv Reference to the vlnv to set
	 *
	*/
	virtual void setVlnv(const VLNV& vlnv);

    /*! \brief Get the description of the designConfiguration
     *
     * \return QString containing the description.
     */
    QString getDescription() const;

    /*! \brief Get the design for this designConfiguration
     *
     * \return A pointer to the VLNV element representing the design.
     */
    VLNV getDesignRef() const;

    /*! \brief Get list of the generatorChainConfigurations
     *
     * \return QList containing pointers to the configurations
     */
    const QList<QSharedPointer<GeneratorChainConfiguration> >&
	getGeneratorChainConfs();

    /*! \brief Get list of the interconnection configurations
     *
     * \return QList containing pointers to the interconnection configurations
     */
    const QList<QSharedPointer<InterconnectionConf> >&
	getInterconnectionConfs();

    /*! \brief Get the view configurations
     *
     * \return QMap containing the id-value pairs of the configurable
     * elements
     */
    const QMap<QString,QString>& getViewConfigurations();

    /*! \brief Get the attributes for the designConfiguration
     *
     * \return QMap containing the attributes.
     */
    const QMap<QString, QString>& getAttributes();

    /*! \brief Set the attributes for the designConfiguration
     *
     * \param attributs QMap containing the attributes.
     */
    void setAttributes(const QMap<QString, QString>& attributes);

    /*! \brief Set the description for this design configuration.
     *
     * \param description A reference to a QString containing the description
     */
    void setDescription(const QString& description);

    /*! \brief Set the design that this design configuration is associated with
     *
     * \param designRef A pointer to the VLNV tag that identifies the design
     * for this design configuration.
     */
    void setDesignRef(const VLNV& designRef);

    /*! \brief Set the generatorChain configurations for this designConfiguration
     *
     * \param generatorChainConfs QList containing pointers to the generator
     * chain configurations
     */
    void setGeneratorChainConfs(
	QList<QSharedPointer<GeneratorChainConfiguration> >&
	generatorChainConfs);

    /*! \brief Set the interconnection configurations
     *
     * \param interconnectionConfs QList containing pointers to the
     * interconnection configurations
     */
    void setInterconnectionConfs(
	QList<QSharedPointer<InterconnectionConf> >&
	interconnectionConfs);

    /*! \brief Set the view configurations for this design configuration
     *
     * \param viewConfigurations QMap containing instance names and view
     * names.
     */
    void setViewConfigurations(QMap<QString,QString>& viewConfigurations);

    /*! \brief Get list of the files needed by this designConfiguration.
     *
     * This function always returns an empty QStringlist because
     * designConfiguration only has VLNV dependencies.
     *
     * \return Empty QStringList.
     */
    virtual const QStringList getDependentFiles() const;

    /*! \brief Get list of the VLNVs needed by this designConfiguration.
     *
     * \return A list of pointers to VLNVs that are needed by this
     * designConfiguration.
     */
    virtual const QList<VLNV> getDependentVLNVs() const;

	/*! \brief Add a new view configuration to the design configuration.
	 *
	 * \param instanceName The name of the component instance that's view is set.
	 * \param viewName The name of the component's view that is set as active.
	 *
	*/
	virtual void addViewConfiguration(const QString& instanceName, 
		const QString& viewName);

	/*! \brief Remove the view configuration for given name.
	 *
	 * \param instanceName Identifies the instance to remove the active view setting.
	 *
	*/
	virtual void removeViewConfiguration(const QString& instanceName);

	/*! \brief Get the active view name for given component instance.
	 *
	 * \param instanceName Identifies the component instance.
	 *
	 * \return QString The name of the active view. If no view is set for given
	 * instance then null QString is returned.
	*/
	QString getActiveView(const QString& instanceName) const;

	/*! \brief Check if active view has been defined for given instance.
	 *
	 * \param instanceName The name of the component instance that's active view
	 * is searched.
	 *
	 * \return bool True if active view has been defined.
	*/
	bool hasActiveView(const QString& instanceName) const;

private:

    /*! \brief MANDATORY spirit:designRef
     * Specifies the design for this design configuration
     */
   VLNV designRef_;

    /*! \brief OPTIONAL spirit:generatorChainConfiguration
     * QList containing pointers to the generatorChainConfiguration instances
     */
    QList<QSharedPointer<GeneratorChainConfiguration> > generatorChainConfs_;

    /*! \brief OPTIONAL spirit:interconnectionConfiguration
     * QList containing pointers to the interconnectionConfiguration instances
     */
    QList<QSharedPointer<InterconnectionConf> > interconnectionConfs_;

    /*! \brief OPTIONAL spirit:viewConfiguration
     *
     * Lists the active view for an instance of the design
     * Key = spirit:instanceName
     * value = spirit:viewName
     */
    QMap<QString, QString> viewConfigurations_;

    /*! \brief OPTIONAL spirit:description
     * Textual description of the design configuration.
     */
    QString description_;

    //! \brief Contains the attributes for the designConfiguration.
    QMap<QString, QString> attributes_;

};

#endif /*DESIGNCONFIGURATION_H_*/
