//-----------------------------------------------------------------------------
// File: FileSetRef.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 4.9.2023
//
// Description:
// Describes the ipxact:fileSetRef element.
//-----------------------------------------------------------------------------

#ifndef FILESETREF_H
#define FILESETREF_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

#include <IPXACTmodels/common/Extendable.h>

class IPXACTMODELS_EXPORT FileSetRef : public Extendable
{
public:

    //! The constructor.
    FileSetRef();

    ~FileSetRef() override = default;

    //! Copy constructor.
    FileSetRef(FileSetRef const& other);

    //! Copy assignment.
    FileSetRef& operator=(FileSetRef const& other);

    //! Get the file set reference.
    QString getReference() const;

    //! Set the file set reference.
    void setReference(QString const& newName);

    //! Get the isPresent value.
    QString getIsPresent() const;

    //! Set the isPresent value.
    void setIsPresent(QString const& newIsPresent);

private:
    //! The name of the referenced file set.
    QString localName_;

    //! The is present value.
    QString isPresent_;
};

#endif // FILESETREF_H
