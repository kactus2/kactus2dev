//-----------------------------------------------------------------------------
// File: ComInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.4.2012
//
// Description:
// COM interface class for defining properties of a specific COM interface
// in a SW component.
//-----------------------------------------------------------------------------

#ifndef COMINTERFACE_H
#define COMINTERFACE_H

#include "generaldeclarations.h"

#include <common/Global.h>

#include <library/LibraryManager/vlnv.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QMap>
#include <QPointF>

//-----------------------------------------------------------------------------
//! COM interface class for defining properties of a specific COM interface
//! in a SW component.
//-----------------------------------------------------------------------------
class KACTUS2_API ComInterface
{
public:
    /*!
     *  Default constructor.
     */
    ComInterface();

    /*!
     *  Copy constructor.
     */
    ComInterface(ComInterface const& rhs);

    /*!
     *  Constructor which reads the COM interface from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ComInterface(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ComInterface();

    /*!
     *  Writes the contents of the COM interface to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the contents of the COM interface are valid.
     *
     *      @param [out] errorList Error list which is appended with errors if found while validating.
     *      @param [in]  parentId  The identifier of the containing component.
     */
    bool isValid(QStringList& errorList, QString const& parentId) const;

    /*!
     *  Returns true if the contents of the COM interface are valid.
     */
    bool isValid() const;

    /*!
     *  Sets the name of the COM interface.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the COM interface.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the COM interface description.
     *
     *      @param [in] desc The description.
     */
    void setDescription(QString const& desc);

    /*!
     *  Sets the COM type.
     *
     *      @param [in] vlnv The COM definition VLNV.
     */
    void setComType(VLNV const& vlnv);

    /*!
     *  Sets the transfer type.
     *
     *      @param [in] transferType The transfer type to set.
     */
    void setTransferType(QString const& transferType);

    /*!
     *  Sets the direction.
     *
     *      @param [in] dir The direction.
     */
    void setDirection(General::Direction dir);

    /*!
     *  Sets the property values.
     *
     *      @param [in] values The property values.
     */
    void setPropertyValues(QMap<QString, QString> const& values);

    /*!
     *  Sets the default position in the parent component's graphical representation.
     *
     *      @param [in] pos The position to set.
     */
    void setDefaultPos(QPointF const& pos);

    /*!
     *  Returns the name of the COM interface.
     */
    QString const& getName() const;

    /*!
     *  Returns the display name of the COM interface.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the COM interface description.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the COM type (COM definition VLNV).
     */
    VLNV const& getComType() const;

    /*!
     *  Returns the transfer type.
     */
    QString const& getTransferType() const;

    /*!
     *  Returns the direction.
     */
    General::Direction getDirection() const;

    /*!
     *  Returns the property values.
     */
    QMap<QString, QString> const& getPropertyValues() const;

    /*!
     *  Returns the default position in the parent component's graphical representation.
     */
    QPointF const& getDefaultPos() const;

    /*!
     *  Assignment operator.
     */
    ComInterface& operator=(ComInterface const& rhs);

	/*! \brief Get the name group of the com interface.
	 *
	 * \return Reference to the name group.
	*/
	General::NameGroup& getNameGroup();

	/*! \brief Get the name group of the com interface.
	 *
	 * \return Reference to the name group.
	*/
	const General::NameGroup& getNameGroup() const;

	/*! \brief Get reference to the COM interface implementation driver.
	 *
     * \return VLNV identifying the implementation driver.
	 */
	const VLNV& getComImplementation() const;

	/*! \brief Set the COM interface implementation driver.
     *
	 * \param implementationVLNV Reference to the driver which implements the COM interface.
	 *
	 */
	void setComImplementation(const VLNV& implementationVLNV);

private:
    /*!
     *  Parses the property values from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parsePropertyValues(QDomNode& node);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! \brief Contains the name, display name and description.
	General::NameGroup nameGroup_;

    //! The COM type (i.e. a VLNV reference to a COM definition).
    VLNV comType_;

    //! The transfer type that is transmitted through the interface.
    QString transferType_;

    //! Dependency direction.
    General::Direction dir_;    

    //! The set property values.
    QMap<QString, QString> propertyValues_;

	//! \brief Reference to the implementation driver.
	 VLNV comImplementation_;

     //! The default position in the parent component's graphical representation (optional).
     QPointF defaultPos_;
};

//-----------------------------------------------------------------------------

#endif // COMINTERFACE_H
