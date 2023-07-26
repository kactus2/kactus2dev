//-----------------------------------------------------------------------------
// File: FieldReference.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 26.7.2023
//
// Description:
// Reference to a field in the encapsulating component.
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

    FieldReference();

    virtual ~FieldReference() = default;

    //! The above types as strings matching the IP-XACT element names.
    static const QString TYPES_AS_STRINGS[];

    //! Sets a reference by type.
    void setReference(FieldReference::Type refType, QString const& ref);

    QStringList getReference(FieldReference::Type refType) const;

    //! Sets the indices of a reference.
    void setReferenceIndices(FieldReference::Type refType, QString const& ref, QList<QString> const& indices);

    QStringList getReferenceIndices(FieldReference::Type refType, QString const& ref = QString()) const;
    
    //! Converts a reference type string to type.
    static FieldReference::Type str2Type(QString const& refTypeStr);

    //! Converts a reference type to string.
    static QString type2Str(FieldReference::Type refType);

private:

    //! Reference with indices.
    struct IndexedReference
    {
        //! The reference.
        QString reference_;

        //! The indices of the reference.
        QList<QString> indices_;
    };

    //! The address space reference.
    QString addressSpaceRef_;

    //! The memory map reference.
    QString memoryMapRef_;

    //! The memory remap reference.
    QString memoryRemapRef_;
    
    //! The bank references.
    QList<QString> bankRefs_;

    //! The address block reference.
    IndexedReference addressBlockRef_;

    //! The register file references.
    QList<IndexedReference> registerFileRefs_;

    //! The register reference.
    IndexedReference registerRef_;

    //! The alternate register reference.
    QString alternateRegisterRef_;

    //! The field reference.
    IndexedReference fieldRef_;
};

#endif // FIELDREFERENCE_H
