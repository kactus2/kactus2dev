/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#ifndef PORT_H_
#define PORT_H_

#include "generaldeclarations.h"

#include <common/Global.h>

#include <IPXACTmodels/NameGroup.h>

#include <QString>
#include <QDomNode>
#include <QList>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QPointF>

class Wire;
class Transactional;
class VendorExtension;
class Kactus2Placeholder;
class Kactus2Position;

/*! \brief Equals the spirit:port element in IP-Xact specification
 *
 * Port describes a single external port on the component
 */
class KACTUS2_API Port {

public:

	/*! \brief The constructor
	 *
	 * \param portNode A reference to QDomNode to parse the information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Port(QDomNode &portNode);

    /*!
	 *  Custom copy constructor which also changes the name of the duplicate.
     *
     *      @param [in] name   The name for the duplicate port.
     *      @param [in] other  The port to copy.
	 */
	Port(QString const& name, Port const& other);

    /*!
     *  Normal copy constructor.
	 */
	Port(const Port &other);

	/*! \brief The constructor
	 *
	 * \param name Name of the port
	 * \param direction Direction of the port
	 * \param leftBound Left bound of the port
	 * \param rightBound Right bound of the port
	 * \param defaultValue Default value of the port
	 * \param allLogicalDirections Specifies if all logical directions of port are
	 * allowed
	 * 
	*/
	Port(const QString& name, 
		General::Direction direction,
		int leftBound, 
		int rightBound, 
		const QString& defaultValue,
		bool allLogicalDirections);

	/*! \brief The constructor
	 *
	 * \param name Name of the port.
	 * \param direction Direction of the port.
	 * \param leftBound Left bound of the port.
	 * \param rightBound Right bound of the port.
	 * \param typeName Type of the port.
	 * \param typeDefinition Type definition of the port type.
	 * \param defaultValue Default value of the port.
	 * \param description Description of the port.
	 *
	*/
	Port(const QString& name, 
		General::Direction direction,
		int leftBound, 
		int rightBound, 
		const QString& typeName,
		const QString& typeDefinition,
		const QString& defaultValue,
		const QString& description);

	/*! \brief The default constructor
	 *
	 * Constructs an empty wire port which is in invalid state.
	*/
	Port();   

	//! \brief Assignment operator
	Port &operator=(const Port &other);

	/*! \brief The destructor
	 *
	 */
	~Port();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWriter instance that is used to
	* write the document into file.
	* \param viewNames The list of views the component has. (Needed for port types)
	*/
	void write(QXmlStreamWriter& writer, const QStringList& viewNames);

	/*!
     *  Creates a copy of the port, having only a different name than the original one.
     *
     *      @param [in] name The name for the duplicate port.
     */
    Port* copy(QString const& name) const;

	/*! \brief Check if the port element is valid or not.
	* 
	* \param hasViews Should be true if component contains views. This is needed
	* because if the port has a type specified it needs at least one view also.
	* 
	* \return True if the port is in valid state.
	*/
	bool isValid(bool hasViews) const;

	/*! \brief Check if the port is in a valid state.
	 *
	 * \param hasViews Should be true if component contains views. This is needed
	 * because if the port has a type specified it needs at least one view also.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(bool hasViews,
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Get the name of the port
	 *
	 * \return QString containing the name
	 */
	QString getName() const;

	/*! \brief Get the display name of the port.
	 *
	 *
	 * \return QString contains the display name.
	*/
	QString getDisplayName() const;

	/*! \brief Set the display name for the port.
	 *
	 * \param displayName The display name to set.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Get the description of the port.
	 *
	 *
	 * \return QString contains the description of the port.
	*/
	QString getDescription() const;

	/*! \brief Set the description for the port.
	 *
	 * \param description The description to set.
	 *
	*/
	void setDescription(const QString& description);

	/*! \brief Get the type of the port
	 *
	 * \return PortType: WIRE or TRANSACTIONAL
	 */
	General::PortType getPortType() const;

	/*! \brief Get pointer to the transactional element
	 *
	 * \return A pointer to the Transactional instance. If the port type is
	 * WIRE then null pointer is returned.
	 */
	Transactional *getTransactional() const;

	/*! \brief Get pointer to the wire element
	 *
	 * \return A pointer to the Wire instance. If the port type is TRANSACTIONAL
	 * then null pointer is returned.
	 */
	Wire *getWire() const;

	/*! \brief Set the name for the port.
	 *
	 * Note: This function should not be called directly to change the name
	 * of the port because model has stored the ports in a map indexed by name.
	 * Instead use the model's renamePort() function.
	 *
	 * \param name QString containing the name
	 */
	void setName(const QString &name);

	/*! \brief Set the transactional element for the port
	 *
	 * Calling this function will delete the old wire and transactional elements,
	 * if found. This also changes the port type into TRANSACTIONAL.
	 *
	 * transactional A pointer to the new transactional instance.
	 */
	void setTransactional(Transactional *transactional);

	/*! \brief Set the wire element for the port
	 *
	 * Calling this function will delete the old wire and transactional elements,
	 * if found. This also changes the port type into WIRE.
	 *
	 * \param wire A pointer to the new wire instance.
	 */
	void setWire(Wire *wire);

	/*! \brief Get the port access type
	 *
	 * \return QString containing the port access type
	 */
	QString getPortAccessType() const;

	/*! \brief Set the port access type
	 *
	 * \param QString containing the port access type
	 */
    void setPortAccessType(const QString &portAccessType);

    /*! \brief Get the left bound of a vectored port.
     *
     * If the port is not vectored then 0 is returned.
     *
     * \return The left bound of the port.
     */
    int getLeftBound() const;

    /*! \brief Get the right bound of a vectored port.
     *
     * If the port is not vectored then 0 is returned.
     *
     * \return The right bound of the port.
     */
    int getRightBound() const;

    /*! \brief Get the size of the port.
     *
     * \return The width of the port (how many bits needed).
     */
    int getPortSize() const;

    /*! \brief Get the default value of the port.
     *
     * This function can be used to get the default value of the port defined
     * spirit:driver withing spirit:wire. If the default value is not defined
     * a null QString is returned.
     *
     * \return QString containing the default value.
     */
    QString getDefaultValue() const;

    /*! \brief Get the direction of the port.
     *
     * This function can be used to get the direction of a wired port.
     * If the port is not wired type DIRECTION_INVALID is returned.
     *
     * \return Enum direction defining the direction.
     */
    General::Direction getDirection() const;

	/*! \brief Get the value of spirit:allLogicalDirectionsAllowed in spirit:wire
	 *
	 * \return The value of the element. If wire is not defined return false.
	*/
	bool allLogicalDirectionsAllowed() const;

	/*! \brief set the direction of the port.
	 *
	 * \param direction
	 *
	 * \return void
	*/
	void setDirection(General::Direction direction);

	/*! \brief Set the left bound of the port
	 *
	 * \param leftBound The left bound to set.
	 *
	*/
	void setLeftBound(int leftBound);

	/*! \brief Set the right bound of the port
	 *
	 * \param rightBound The right bound to set.
	 *
	*/
	void setRightBound(int rightBound);

	/*! \brief Set the size of the port
	 * 
	 * The port bound are set to size-1 downto 0
	 * 
	 * \param size Number of bits the port contains
	 *
	*/
	void setPortSize(int size);

	/*! \brief Set the default value of the port.
	 *
	 * \param defaultValue The default value to be set.
	 *
	*/
	void setDefaultValue(const QString& defaultValue);

	/*! \brief Set the port to allow/disallow all logical directions.
	 *
	 * \param allowed The setting
	 *
	*/
	void setAllLogicalDirectionsAllowed(bool allowed);

	/*! \brief Get the type name of the port for given view.
	 *
	 * \param viewName The name of the view used.
	 *
	 * \return QString contains the type name. (i.e. std_logic_vector)
	*/
	QString getTypeName(const QString& viewName = QString("")) const;

	/*! \brief Check if the port has a type name in given view.
	 * 
	 * If view is not defined then all possibilities are checked and if no type
	 * is specified for any view false is returned.
	 *
	 * \param viewName The name of the view that's type name is searched for.
	 *
	 * \return bool False if type is not defined.
	*/
	bool hasType(const QString& viewName = QString()) const;

	/*! \brief Check if the port has type or type definition set.
	 *
	 *
	 * \return bool True if the either port type name or type definition or both
	 * are defined.
	*/
	bool hasTypeDefinitions() const;

	/*! \brief Set the type name for the port in given view.
	 *
	 * \param typeName The name of the type.
	 * \param viewName The name of the view used for this type. Leave empty to use
	 * in all views.
	 *
	*/
	void setTypeName(const QString& typeName, const QString& viewName = QString(""));

	/*! \brief Use Default vhdl types and type definitions for the port.
	 * 
	 * Default types are:
	 * std_logic for ports sized 1
	 * std_logic_vector for ports larger than one bit.
	 * 
	 * The default type definition is IEEE.std_logic_1164.all
	 *
	*/
	void useDefaultVhdlTypes();

	/*! \brief Get the type definition of the given type name.
	 *
	 * \param typeName The name of the type that's type definition is wanted.
	 *
	 * \return QString contains the type definition.
	*/
	QString getTypeDefinition(const QString& typeName);

	/*! \brief Get the type definitions of the wired port.
	 *
	 *
	 * \return QStringList contains the type definitions set for this port.
	*/
	QStringList getTypeDefinitions() const;

	/*! \brief Set the type definition of the given type name.
	 *
	 * \param typeName Identifies the type name that's definition is set.
	 * \param typeDefinition The type definition to set for the type.
	 * (i.e. for type std_logic could be set IEEE.std_logic_1164.all)
	 *
	*/
	void setTypeDefinition(const QString& typeName, const QString& typeDefinition);

    /*!
     *  Sets default the ad-hoc visibility of the port.
     *
     *      @param [in] visible If true, the port is set visible for ad-hoc connections.
     */
    void setAdHocVisible(bool visible);

    /*!
     *  Sets the default position in the parent component's graphical representation.
     *
     *      @param [in] pos The position to set.
     */
    void setDefaultPos(QPointF const& pos);

    /*!
     *  Returns the default position in the parent component's graphical representation.
     */
    QPointF getDefaultPos() const;

    /*!
     *  Returns true if the ad-hoc visibility is turned on.
     */
    bool isAdHocVisible() const;

private:

    /*!
     *  Parses the vendor extensions from a DOM node.
     *
     *      @param [in] extensionsNode   The DOM node containing all vendor extensions.
     */
    void parseVendorExtensions(QDomNode const& extensionsNode);

    /*!
     *  Copies vendor extensions from another port.
     *
     *      @param [in] other   The port to copy extensions from.
     */
    void copyVendorExtensions(Port const& other);

    void createAdHocVisibleExtension();
    void createPositionExtension();
	//! \brief Contains the name, display name and description for the port.
	NameGroup nameGroup_;

	/*!
	 * Defines the port type
	 */
	General::PortType portType_;

	/*!
	 * MANDATORY (wire OR Transactional must be found) (spirit:transactional)
	 */
	QSharedPointer<Wire> wire_;

	/*!
	 * MANDATORY (Wire OR Transactional must be found) (spirit:Wire)
	 */
	QSharedPointer<Transactional> transactional_;

	/*!
	 * OPTIONAL (spirit:portAccessHandle)
	 * Indicates to a netlister the method to be used to access the object
	 * representing the port.
	 */
	QString portAccessHandle_;

	/*!
	 * OPTIONAL (spirit:portAccessType)
	 * Indicates to a netlister how to access the port.
	 */
	QString portAccessType_;

    QSharedPointer<Kactus2Placeholder> adHocVisible_;

    QSharedPointer<Kactus2Position> defaultPos_;

	/*!
	 * OPTIONAL (spirit: vendorExtensions)
	 * Port vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif /* PORT_H_ */
