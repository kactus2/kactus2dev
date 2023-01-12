//-----------------------------------------------------------------------------
// File: KactusAttributeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 23.6.2011
//
// Description:
// Attribute editor for Kactus 2 -specific base attributes.
//-----------------------------------------------------------------------------

#ifndef KACTUSATTRIBUTEEDITOR_H
#define KACTUSATTRIBUTEEDITOR_H

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/common/TagData.h>

#include <QGroupBox>
#include <QComboBox>
#include <QLabel>

class TagContainer;

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
    virtual ~KactusAttributeEditor();

    /*!
     *  Hides the editable attributes.
     */
    void hideAttributes();

    /*!
     *  Sets the given attributes as selected.
     *
     *      @param [in] prodHier    Product hierarchy.
     *      @param [in] firmness	Firmness.
     *      @param [in] tags        Document tags.
     */
    void setAttributes(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
        QVector<TagData> tags);

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

    /*!
     *  Get document tags.
     *
     *      @return Document tags.
     */
    QVector<TagData> getTags() const;

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

    //! Editor for document tags.
    TagContainer* tagEditor_;
};

//-----------------------------------------------------------------------------

#endif // KACTUSATTRIBUTEEDITOR_H
