//-----------------------------------------------------------------------------
// File: KactusAttribute.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.6.2011
//
// Description:
// Kactus attribute definitions and related utility functions.
//-----------------------------------------------------------------------------

#ifndef KACTUSATTRIBUTE_H
#define KACTUSATTRIBUTE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/VendorExtension.h>

//-----------------------------------------------------------------------------
//! Kactus attribute definitions and related utility functions.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT KactusAttribute : public VendorExtension
{
public:

    //-----------------------------------------------------------------------------
    //! ProductHierarchy enumeration.
    //-----------------------------------------------------------------------------
    enum ProductHierarchy
    {
        FLAT = 0,
        PRODUCT,
        BOARD,
        CHIP,
        SOC,
        IP,
        KTS_PRODHIER_COUNT
    };

    //-----------------------------------------------------------------------------
    //! Firmness enumeration.
    //-----------------------------------------------------------------------------
    enum Firmness
    {
        MUTABLE = 0,
        TEMPLATE,
        FIXED,
        KTS_REUSE_LEVEL_COUNT
    };

    //--------------------------------------------------------------------------
    //! Firmness enumeration.
    //--------------------------------------------------------------------------
    enum Implementation
    {
        HW = 0,
        SW,
        SYSTEM,
        KTS_IMPLEMENTATION_COUNT
    };

	//! The constructor.
	KactusAttribute();

	//! The destructor.
	~KactusAttribute();

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual VendorExtension* clone() const;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Sets the product hierarchy in the attributes.
     *
     *      @param [in] hierarchy   The hierarchy level to set.
     */
    void setHierarchy(KactusAttribute::ProductHierarchy hierarchy);

    /*!
     *  Gets the product hierarchy in the attributes.
     *
     *      @return The product hierarchy.
     */
    KactusAttribute::ProductHierarchy getHierarchy() const;
    
    /*!
     *  Sets the product implementation in the attributes.
     *
     *      @param [in] implementation   The implementation type to set.
     */
    void setImplementation(KactusAttribute::Implementation implementation);
    
    /*!
     *  Gets the product implementation in the attributes.
     *
     *      @return The implementation.
     */
    KactusAttribute::Implementation getImplementation() const;
        
    /*!
     *  Sets the product firmness in the attributes.
     *
     *      @param [in] firmness   The firmness to set.
     */
    void setFirmness(KactusAttribute::Firmness firmness);
        
    /*!
     *  Gets the product firmness in the attributes.
     *
     *      @return The firmness.
     */
    KactusAttribute::Firmness getFirmness() const;
    
    /*!
     *  Converts the given string to a product hierarchy.
     *
     *      @param [in] string The string to convert.
     *
     *      @return The equivalent hierarchy level.
     */
    static ProductHierarchy hierarchyFrom(QString const& string);
        
    /*!
     *  Converts the given string to a product implementation.
     *
     *      @param [in] string The string to convert.
     *
     *      @return The equivalent implementation type.
     */
    static Implementation implementationFrom(QString const& string);
        
    /*!
     *  Converts the given string to a firmness type.
     *
     *      @param [in] string The string to convert.
     *
     *      @return The equivalent firmness type.
     */
    static Firmness firmnessFrom(QString const& string);

    /*!
     *  Converts the given product hierarchy to a string.
     *
     *      @param [in] prodHier The product hierarchy.
     *
     *      @return The equivalent string.
     */
    static QString hierarchyToString(ProductHierarchy prodHier);

    /*!
     *  Converts the given firmness to a string.
     *
     *      @param [in] firmness specifies the firmness.
     *
     *      @return The equivalent string.
     */
    static QString firmnessToString(Firmness firmness);
    
    /*!
     *  Converts the given firmness to a string.
     *
     *      @param [in] firmness The firmness.
     *
     *      @return The equivalent string.
     */
    static QString implementationToString(Implementation implementation);

private:

	// Disable copying.
	KactusAttribute(KactusAttribute const& rhs);
	KactusAttribute& operator=(KactusAttribute const& rhs);

    //! The product hierarchy level for the attributes.
    ProductHierarchy productHierarchy_;

    //! The product implementation type for the attributes.
    Implementation implementation_;

    //! The product firmness level for the attributes.
    Firmness firmness_;
};

#endif // KACTUSATTRIBUTE_H
