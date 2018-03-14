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

#include <IPXACTmodels/common/VLNV.h>

#include <QSharedPointer>
#include <QString>

class Document;

//-----------------------------------------------------------------------------
//! Reading and writing of IP-XACT documents using a standard file system.
//-----------------------------------------------------------------------------
class DocumentFileAccess 
{
public:

	//! The constructor.
	DocumentFileAccess();

	//! The destructor.
	~DocumentFileAccess();

    QSharedPointer<Document> readDocument(QString const& path);

    bool writeDocument(QSharedPointer<Document> model, QString const& path);

    VLNV getDocumentVLNV(QString const& path);

private:

	// Disable copying.
	DocumentFileAccess(DocumentFileAccess const& rhs);
	DocumentFileAccess& operator=(DocumentFileAccess const& rhs);

};

#endif // DOCUMENTFILEACCESS_H
