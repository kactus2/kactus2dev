//-----------------------------------------------------------------------------
// File: NameGroupWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Writer class for namegroup element.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUPWRITER_H
#define NAMEGROUPWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <QObject>
#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for namegroup element.
//-----------------------------------------------------------------------------
class NameGroupWriter
{
public:

    /*!
     *  The constructor.
     */
    NameGroupWriter();

    /*!
     *  The destructor.
     */
    ~NameGroupWriter() = default;

    /*!
     *  Write the name group.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] nameGroup   The selected name group.
     */
    static void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<NameGroup> nameGroup);

private:

    // Disable copying.
    NameGroupWriter(NameGroupWriter const& rhs);
    NameGroupWriter& operator=(NameGroupWriter const& rhs);
};

#endif // NAMEGROUPWRITER_H