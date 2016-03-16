//-----------------------------------------------------------------------------
// File: RemapStateWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Writer class for IP-XACT remapState element.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATEWRITER_H
#define REMAPSTATEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class RemapState;
class RemapPort;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT remapState element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RemapStateWriter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    RemapStateWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~RemapStateWriter();

    /*!
     *  Write a remap state to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] view    The remap state to be written.
     */
    void writeRemapState(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState) const;

private:

    //! No copying allowed.
    RemapStateWriter(RemapStateWriter const& rhs);
    RemapStateWriter& operator=(RemapStateWriter const& rhs);

    /*!
     *  Write the name group.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] remapState  The selected remap state.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState) const;

    /*!
     *  Write the remap ports.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] remapState  The selected remap state.
     */
    void writeRemapPorts(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState) const;

    /*!
     *  Write a single remap port.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] remapPort   The selected remap port.
     */
    void writeSingleRemapPort(QXmlStreamWriter& writer, QSharedPointer<RemapPort> remapPort) const;
};

#endif // REMAPSTATEWRITER_H