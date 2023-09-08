//-----------------------------------------------------------------------------
// File: FieldReference.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.7.2023
//
// Description:
// Reference to a field in the encapsulating component. Describes the elements 
// in the fieldReferenceGroup and fieldSliceReferenceGroup.
//-----------------------------------------------------------------------------

#ifndef FIELDREFERENCE_H
#define FIELDREFERENCE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <array>

#include <QString>
#include <QList>

class IPXACTMODELS_EXPORT FieldReference
{
public:
    //! Type of reference in the field reference group.
    enum Type
    {
        ADDRESS_SPACE = 0,
        MEMORY_MAP,
        MEMORY_REMAP,
        BANK,
        ADDRESS_BLOCK,
        REGISTER_FILE,
        REGISTER,
        ALTERNATE_REGISTER,
        FIELD,
        REFERENCE_TYPE_COUNT
    };

    //! Reference with indices.x
    struct IPXACTMODELS_EXPORT IndexedReference
    {
        //! The reference.
        QString reference_;

        //! The indices of the reference. Is empty for references without indices.
        QList<QString> indices_;

        IndexedReference() = default;

        IndexedReference(QString const& reference, QList<QString> const& indices);
        
        IndexedReference(IndexedReference const& other);
        IndexedReference& operator=(IndexedReference const& other);
    };

    /*!
     *	The constructor.
     */
    FieldReference();

    /*!
     *	Copy constructor.
     */
    FieldReference(FieldReference const& other);

    FieldReference& operator=(FieldReference const& other);

    virtual ~FieldReference() = default;

    //! The above types as strings matching the IP-XACT element names.
    static const QString TYPES_AS_STRINGS[];

    /*!
     *	Sets a reference by type. If the reference is of type bank or register file,
     *  it will be added to the respective reference list.
     *  
     *      @param [in] reference     The reference to set.
     *      @param [in] refType       The reference type.
     */
    void setReference(QSharedPointer<IndexedReference> reference, FieldReference::Type refType);

    /*!
     *	Get the reference of a specific type.
     *  
     *      @param [in] refType     The reference type.
     *	    
     * 	    @return The reference, or nullptr if invalid refType or the reference is a multiple reference.
     */
    QSharedPointer<IndexedReference> getReference(FieldReference::Type refType) const;
    
    void clearReference(FieldReference::Type refType);

    /*!
     *	Get a list of references of a specific type. Can be called for all types, but can have multiple references
     *  only for types bank and register file.
     *  
     *      @param [in] refType     The type of reference to get.
     *	    
     * 	    @return A list of references of a specific type.
     */
    QSharedPointer<QList<QSharedPointer<IndexedReference> > > getMultipleReference(FieldReference::Type refType) const;
    
    //! Converts a reference type string to type.
    static FieldReference::Type str2Type(QString const& refTypeStr);

    //! Converts a reference type to string.
    static QString type2Str(FieldReference::Type refType);

private:

    //! The address space reference.
    QSharedPointer<IndexedReference> addressSpaceRef_;

    //! The memory map reference.
    QSharedPointer<IndexedReference> memoryMapRef_;

    //! The memory remap reference.
    QSharedPointer<IndexedReference> memoryRemapRef_;
    
    //! The bank references.
    QSharedPointer<QList<QSharedPointer<IndexedReference> > > bankRefs_ = 
        QSharedPointer<QList<QSharedPointer<IndexedReference> > >(new QList<QSharedPointer<IndexedReference> >());

    //! The address block reference.
    QSharedPointer<IndexedReference> addressBlockRef_;

    //! The register file references.
    QSharedPointer<QList<QSharedPointer<IndexedReference> > > registerFileRefs_ =
        QSharedPointer<QList<QSharedPointer<IndexedReference> > >(new QList<QSharedPointer<IndexedReference> >());

    //! The register reference.
    QSharedPointer<IndexedReference> registerRef_;

    //! The alternate register reference.
    QSharedPointer<IndexedReference> alternateRegisterRef_;

    //! The field reference.
    QSharedPointer<IndexedReference> fieldRef_;
};

#endif // FIELDREFERENCE_H
