//-----------------------------------------------------------------------------
// File: MemoryRemap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAP_H
#define MEMORYREMAP_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>

#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryRemap : public MemoryMapBase
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name        Name of the memory remap.
     *      @param [in] remapState  The remap state refered by the memory remap.
     */
    MemoryRemap(QString const& name = QString(), QString const& remapState = QString());

    /*!
     *  Copy constructor.
     *
     *      @param [in] other   The memory remap being copied.
     */
    MemoryRemap(const MemoryRemap& other);

    /*!
     *  Assignment operator.
     *
     *      @param [in] other   The memory remap being assigned.
     */
    MemoryRemap& operator=(const MemoryRemap& other);

    /*!
     *  The destructor.
     */
    ~MemoryRemap();

    /*!
     *  Get the element name of the memory remap.
     *
     *      @return QString containing "Memory remap".
     */
    virtual QString elementName() const;

    /*!
     *  Get the remap state of the memory remap.
     *
     *      @return The name of the remap state of the memory remap.
     */
    QString getRemapState() const;

    /*!
     *  Set the remap state of the memory remap.
     *
     *      @param [in] newRemapState   The name of the new remap state of the memory remap.
     */
    void setRemapState(QString newRemapState);

private:

    //! The remap state of this memory remap.
    QString remapState_;

};

#endif // MEMORYREMAP_H_
