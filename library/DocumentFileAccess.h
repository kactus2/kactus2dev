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


#include <QSharedPointer>
#include <QString>

class Document;
class MessageMediator;

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

    //! Disable copying.
    DocumentFileAccess(DocumentFileAccess const& rhs) = delete;
    DocumentFileAccess& operator=(DocumentFileAccess const& rhs) = delete;

    //! Reads the document from given path.
    QSharedPointer<Document> readDocument(QString const& path);

    //! Writes to document model to the given path.
    bool writeDocument(QSharedPointer<Document> model, QString const& path);
    
private:

    //! Channel for messages concerning file access.
    MessageMediator* messageChannel_;
};

#endif // DOCUMENTFILEACCESS_H
