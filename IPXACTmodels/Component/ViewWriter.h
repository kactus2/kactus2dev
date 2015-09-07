//-----------------------------------------------------------------------------
// File: ViewWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Writer class for IP-XACT view element.
//-----------------------------------------------------------------------------

#ifndef VIEWWRITER_H
#define VIEWWRITER_H

#include "View.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT view element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ViewWriter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    ViewWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ViewWriter();

    /*!
     *  Write a view to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] view    The view to be written.
     */
    void writeView(QXmlStreamWriter& writer, QSharedPointer<View> view) const;

private:

    //! No copying allowed.
    ViewWriter(ViewWriter const& rhs);
    ViewWriter& operator=(ViewWriter const& rhs);

    /*!
     *  Writes the name group to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] view    The view to be written.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<View> view) const;

    /*!
     *  Writes the presence to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] view    The view to be written.
     */
    void writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<View> view) const;

    /*!
     *  Writes the envIdentifiers to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] view    The view to be written.
     */
    void writeEnvIdentifiers(QXmlStreamWriter& writer, QSharedPointer<View> view) const;

    /*!
     *  Writes the instantiation references to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] view    The view to be written.
     */
    void writeInstantiationReferences(QXmlStreamWriter& writer, QSharedPointer<View> view) const;
};

#endif // VIEWWRITER_H