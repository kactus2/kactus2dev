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

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QMap>
#include <QPointF>
#include <QSharedPointer>

#include <IPXACTmodels/kactusExtensions/ComProperty.h>

//-----------------------------------------------------------------------------
//! COM interface class for defining properties of a specific COM interface
//! in a SW component.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComInterface : public NameGroup, public VendorExtension
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
     *  Copies the com interface.
     *
     *      @return The copied com interface.
     */
    virtual ComInterface* clone() const;

    /*!
     *  Get the type of the com interface.
     *
     *      @return The com interface type.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents of the COM interface to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

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
     *  Sets the COM type.
     *
     *      @param [in] vlnv The COM definition VLNV.
     */
    void setComType(VLNV const& vlnv, QList< QSharedPointer<ComProperty> > const* properties = NULL);

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
    void setDirection(DirectionTypes::Direction dir);

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
    DirectionTypes::Direction getDirection() const;

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

	/*!
     *  Get reference to the COM interface implementation driver.
	 *
     *      @return VLNV identifying the implementation driver.
	 */
	const VLNV& getComImplementation() const;

	/*!
     *  Set the COM interface implementation driver.
     *
	 *      @param [in] implementationVLNV  Reference to the driver which implements the COM interface.
	 */
	void setComImplementation(const VLNV& implementationVLNV);

private:
    /*!
     *  Parses the property values from the given XML node.
     *
     *      @param [in] node The source XML node.
     */
    void parsePropertyValues(QDomNode& node);

    /*!
     *  Write the com interface position.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] pos     Com interface position.
     */
    void writePosition(QXmlStreamWriter& writer, QPointF const& pos) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The COM type (i.e. a VLNV reference to a COM definition).
    VLNV comType_;

    //! The transfer type that is transmitted through the interface.
    QString transferType_;

    //! Dependency direction.
    DirectionTypes::Direction dir_;    

    //! The set property values.
    QMap<QString, QString> propertyValues_;

	//! Reference to the implementation driver.
	 VLNV comImplementation_;

     //! The default position in the parent component's graphical representation (optional).
     QPointF defaultPos_;
};

//-----------------------------------------------------------------------------

#endif // COMINTERFACE_H
