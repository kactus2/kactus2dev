//-----------------------------------------------------------------------------
// File: RemapStateReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Reader class for IP-XACT remap state element.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATEREADER_H
#define REMAPSTATEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class RemapState;
class RemapPort;

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT remap state element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RemapStateReader : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    RemapStateReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~RemapStateReader();

    /*!
     *  Creates a new remap state from a given view node.
     *
     *      @param [in] remapStateNode  XML description of the remap state.
     *
     *      @return The created remap state.
     */
    QSharedPointer<RemapState> createRemapStateFrom(QDomNode const& remapStateNode) const;

private:

    //! No copying allowed.
    RemapStateReader(RemapStateReader const& rhs);
    RemapStateReader& operator=(RemapStateReader const& rhs);

    /*!
     *  Read the remap state name group.
     *
     *      @param [in] remapStateNode  XML description of the remap state.
     *      @param [in] newRemapState   The selected remap state item.
     */
    void parseNameGroup(QDomNode const& remapStateNode, QSharedPointer<RemapState> newRemapState) const;

    /*!
     *  Read the remap ports.
     *
     *      @param [in] remapStateNode  XML description of the remap state.
     *      @param [in] newRemapState   The selected remap state item.
     */
    void parseRemapPorts(QDomNode const& remapStateNode, QSharedPointer<RemapState> newRemapState) const;

    /*!
     *  Read the remap port index.
     *
     *      @param [in] remapPortElement    XML description of the remap port.
     *      @param [in] newRemapPort        The selected remap port item.
     */
    void parseRemapPortIndex(QDomElement const& remapPortElement, QSharedPointer<RemapPort> newRemapPort) const;

    /*!
     *  Read the remap port value.
     *
     *      @param [in] remapPortElement    XML description of the remap port.
     *      @param [in] newRemapPort        The selected remap port item.
     */
    void parseRemapPortValue(QDomElement const& remapPortElement, QSharedPointer<RemapPort> newRemapPort) const;
};

#endif // VIEWREADER_H