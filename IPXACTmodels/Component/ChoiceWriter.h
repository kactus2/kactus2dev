//-----------------------------------------------------------------------------
// File: ChoiceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Writer class for IP-XACT Choice element.
//-----------------------------------------------------------------------------

#ifndef ChoiceWriter_H
#define ChoiceWriter_H

#include "Choice.h"

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Choice element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ChoiceWriter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ChoiceWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ChoiceWriter();

    /*!
     *  Write a view to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] Choice		The Choice to be written.
     */
    void writeChoice(QXmlStreamWriter& writer, QSharedPointer<Choice> choice) const;

private:

    //! No copying allowed.
    ChoiceWriter(ChoiceWriter const& rhs);
    ChoiceWriter& operator=(ChoiceWriter const& rhs);
};

#endif // ChoiceWriter_H
