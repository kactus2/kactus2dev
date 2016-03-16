//-----------------------------------------------------------------------------
// File: ComDefinition.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 29.3.2012
//
// Description:
// Class which encapsulates the handling of custom communication definition
// object.
//-----------------------------------------------------------------------------

#ifndef COMDEFINITION_H
#define COMDEFINITION_H

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <IPXACTmodels/ipxactmodels_global.h>

//-----------------------------------------------------------------------------
//! Communication definition class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComDefinition : public Document
{
public:
    /*!
     *  Constructor which creates an empty communication definition.
     *
     *      @param [in] vlnv The VLNV of the communication definition.
     */
    ComDefinition(VLNV const& vlnv = VLNV());

    /*!
     *  Copy constructor.
     */
    ComDefinition(ComDefinition const& rhs);

    /*!
     *  Destructor.
     */
    virtual ~ComDefinition();

    /*!
     *  Makes a copy of the document.
     *
     *      @return The created copy of the COM definition.
     */
    virtual QSharedPointer<Document> clone()  const;

    /*!
     *  Returns the dependent files (none).
     */
    virtual QStringList getDependentFiles() const;

    /*!
     *  Returns the dependent VLNVs (none).
     */
    virtual QList<VLNV> getDependentVLNVs() const;

    /*!
     *  Adds a new transfer type to the communication definition.
     *
     *      @param [in] type The name of the transfer type to add.
     */
    void addTransferType(QString const& type);

    /*!
     *  Removes a transfer type from the communication definition.
     *
     *      @param [in] type The name of the transfer type to remove.
     */
    void removeTransferType(QString const& type);

    /*!
     *  Sets the supported transfer types.
     *
     *      @param [in] types A list of transfer type names.
     */
    void setTransferTypes(QStringList const& types);

    /*!
     *  Adds a property to the definition.
     *
     *      @param [in] prop The property to add.
     */
    void addProperty(QSharedPointer<ComProperty> prop);

    /*!
     *  Removes a property from the definition.
     *
     *      @param [in] name The name of the property to remove.
     */
    void removeProperty(QString const& name);

    /*!
     *  Sets the properties.
     *
     *      @param [in] properties A list of properties to set.
     */
    void setProperties(QList< QSharedPointer<ComProperty> > const& properties);

    /*!
     *  Returns the supported transfer types.
     */
    QSharedPointer< QStringList > getTransferTypes() const;

    /*!
     *  Returns the list of properties.
     */
     QSharedPointer< QList< QSharedPointer<ComProperty> > > getProperties() const;

private:
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of transfer types.
    QSharedPointer< QStringList > transferTypes_;

    //! The list of properties.
    QSharedPointer< QList< QSharedPointer<ComProperty> > > properties_;
};

//-----------------------------------------------------------------------------

#endif // COMDEFINITION_H
