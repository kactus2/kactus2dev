//-----------------------------------------------------------------------------
// File: NameGroupWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Writer for namegroup element.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUPWRITER_H
#define NAMEGROUPWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <QObject>
#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer for namegroup element.
//-----------------------------------------------------------------------------
namespace NameGroupWriter
{
    /*!
     *  Write the name group.
     *
     *    @param [in] writer      The used xml writer.
     *    @param [in] nameGroup   The selected name group.
     *    @param [in] revision    The IP-XACT standard to comply to.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<NameGroup> nameGroup,
        Document::Revision revision = Document::Revision::Std14);
}

#endif // NAMEGROUPWRITER_H