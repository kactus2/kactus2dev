//-----------------------------------------------------------------------------
// File: Utils.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 9.5.2012
//
// Description:
// Common utilities for writing/parsing IP-XACT documents.
//-----------------------------------------------------------------------------

#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QDomNode>
#include <QString>
#include <QMap>
#include <QPointF>
#include <QSharedPointer>

//-----------------------------------------------------------------------------

namespace XmlUtils
{

    /*! \brief removes white spaces from the given QString
     *
     *	This function removes white spaces from the beginning and end of the string
     *	and also replaces any embedded whitespaces with characted '_'. If you only
     *	want to remove the white spaces from the start and end use
     *	QString::trimmed() instead of this.
     *
     *	IP-Xact types used for this function:
     *	-Name
     *	-NMTOKEN
     *	-PortName
     *	-ID
     *	-IDREF
     *
     * \param str A reference to the QString where white spaces are wanted to
     * be removed from.
     *
     * \return QString containing the string that has been stripped from
     * whitespaces and embedded whitespaces have been replaces with '_'
     */
    IPXACTMODELS_EXPORT QString removeWhiteSpace(QString const& str);

}

//-----------------------------------------------------------------------------

#endif // XMLUTILS_H
