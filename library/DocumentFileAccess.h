//-----------------------------------------------------------------------------
// File: DocumentFileAccess.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.02.2018
//
// Description:
// Reading and writing of IP-XACT documents using a standard file system.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTFILEACCESS_H
#define DOCUMENTFILEACCESS_H

#include <common/ui/MessageMediator.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QSharedPointer>
#include <QString>
#include <QDomDocument>

class Document;

//-----------------------------------------------------------------------------
//! Reading and writing of IP-XACT documents using a standard file system.
//-----------------------------------------------------------------------------
class DocumentFileAccess 
{
public:

	//! The constructor.
	explicit DocumentFileAccess(MessageMediator* messageChannel);

	//! The destructor.
	~DocumentFileAccess() = default;

    //! Reads the document from given path.
    QSharedPointer<Document> readDocument(QString const& path);

    //! Writes to document model to the given path.
    bool writeDocument(QSharedPointer<Document> model, QString const& path);

    //! Gets the VLVN type of the document in the given path.
    VLNV::IPXactType getDocumentVLNV(QDomDocument const& doc);

private:

	//! Disable copying.
	DocumentFileAccess(DocumentFileAccess const& rhs);
	DocumentFileAccess& operator=(DocumentFileAccess const& rhs);

    //! Channel for messages concerning file access.
    MessageMediator* messageChannel_;
};

#endif // DOCUMENTFILEACCESS_H
