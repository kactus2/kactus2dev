//-----------------------------------------------------------------------------
// File: KactusAttributeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.6.2011
//
// Description:
// Attribute editor for Kactus 2 -specific base attributes.
//-----------------------------------------------------------------------------

#ifndef KACTUSATTRIBUTEEDITOR_H
#define KACTUSATTRIBUTEEDITOR_H

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QGroupBox>
#include <QComboBox>
#include <QLabel>

//-----------------------------------------------------------------------------
//! KactusAttributeEditor class.
//-----------------------------------------------------------------------------
class KactusAttributeEditor : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    KactusAttributeEditor(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~KactusAttributeEditor();

    /*!
     *  Hides the editable attributes.
     */
    void hideAttributes();

    /*!
     *  Sets the given attributes as selected.
     *
     *      @param [in] prodHier    Product hierarchy.
     *      @param [in] firmness	Firmness.
     */
    void setAttributes(KactusAttribute::ProductHierarchy prodHier,
                       KactusAttribute::Firmness firmness);

    /*!
     *  Sets the implementation visible with the given value.
     *
     *      @param [in] implementation Implementation.
     */
    void setImplementation(KactusAttribute::Implementation implementation);
    
    /*!
     *  Returns the selected product hierarchy.
     */
    KactusAttribute::ProductHierarchy getProductHierarchy() const;

    /*!
     *  Returns the selected firmness.
     */
    KactusAttribute::Firmness getFirmness() const;

signals:
    //! Signaled when the attributes have changed.
    void contentChanged();

    //! Signaled when the product hierarchy attribute has changed.
    void productHierarchyChanged();

private:
    // Disable copying.
    KactusAttributeEditor(KactusAttributeEditor const& rhs);
    KactusAttributeEditor& operator=(KactusAttributeEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        PROD_HIER_COUNT = 6,
        FIRMNESS_COUNT = 4
    };

    //! Product hierarchy label.
    QLabel* prodHierLabel_;

    //! Product hierarchy combo box.
    QComboBox* prodHierCombo_;

    //! Firmness label.
    QLabel* firmnessLabel_;

    //! Firmness combo box.
    QComboBox* firmnessCombo_;

    //! Implementation label.
    QLabel* implementationLabel_;

    //! Label for the implementation value.
    QLabel* implementationValue_;
};

//-----------------------------------------------------------------------------

#endif // KACTUSATTRIBUTEEDITOR_H
