//-----------------------------------------------------------------------------
// File: ApiInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.4.2012
//
// Description:
// API interface class for defining properties of a specific API interface
// in a SW component.
//-----------------------------------------------------------------------------

#ifndef APIINTERFACE_H
#define APIINTERFACE_H

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QPointF>

//-----------------------------------------------------------------------------
//! Dependency direction enumeration.
//-----------------------------------------------------------------------------
enum DependencyDirection
{
    DEPENDENCY_REQUESTER = 0,   //!< The API interface requires the API.
    DEPENDENCY_PROVIDER         //!< The API interface provides the API for those who need it.
};

/*!
 *  Converts the string to an equivalent enumeration value.
 */
IPXACTMODELS_EXPORT DependencyDirection str2DependencyDirection(QString const& str, DependencyDirection defaultValue);

/*!
 *  Converts the enumeration value to an equivalent string.
 *
 *      @param [in] dir The dependency direction enum value.
 *
 *      @return The equivalent string.
 */
IPXACTMODELS_EXPORT QString dependencyDirection2Str(DependencyDirection dir);

//-----------------------------------------------------------------------------
//! API interface class for defining properties of a specific API interface
//! in a SW component.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiInterface : public NameGroup, public VendorExtension
{
public:

    /*!
     *  Default constructor.
     */
    ApiInterface();

    /*!
     *  Copy constructor.
     */
    ApiInterface(ApiInterface const& rhs);

    /*!
     *  Constructor which reads the API interface from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ApiInterface(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ApiInterface();

    /*!
     *  Clones the api interface.
     *
     *      @return A cloned api interface.
     */
    virtual ApiInterface* clone() const;

    /*!
     *  Gets the api interface type.
     *
     *      @return The type of the api interface.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents of the API interface to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the contents of the API interface are valid.
     *
     *      @param [out] errorList  Error list which is appended with errors if found while validating.
     *      @param [in]  parentId   The identifier of the containing component.
     */
    bool isValid(QStringList& errorList, QString const& parentId) const;

    /*!
     *  Returns true if the contents of the API interface are valid.
     */
    bool isValid() const;

    /*!
     *  Sets the API type.
     *
     *      @param [in] vlnv The API definition VLNV.
     */
    void setApiType(VLNV const& vlnv);

    /*!
     *  Sets the dependency direction.
     *
     *      @param [in] dir The dependency direction.
     */
    void setDependencyDirection(DependencyDirection dir);

    /*!
     *  Sets the default position in the parent component's graphical representation.
     *
     *      @param [in] pos The position to set.
     */
    void setDefaultPos(QPointF const& pos);

    /*!
     *  Returns the API type (API definition VLNV).
     */
    VLNV const& getApiType() const;

    /*!
     *  Returns the dependency direction.
     */
    DependencyDirection getDependencyDirection() const;

    /*!
     *  Returns the default position in the parent component's graphical representation.
     */
    QPointF const& getDefaultPos() const;

    /*!
     *  Assignment operator.
     */
    ApiInterface& operator=(ApiInterface const& rhs);

private:

    void writePosition(QXmlStreamWriter& writer, QPointF const& pos) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The API type (i.e. a VLNV reference to an API definition).
    VLNV apiType_;

    //! Dependency direction.
    DependencyDirection dependencyDir_;

    //! The default position in the parent component's graphical representation (optional).
    QPointF defaultPos_;
};

//-----------------------------------------------------------------------------

#endif // APIINTERFACE_H
