//-----------------------------------------------------------------------------
// File: SubSpaceMapWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.01.2022
//
// Description:
// Writer class for ipxact:subSpaceMap element.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAPWRITER_H
#define SUBSPACEMAPWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class SubSpaceMap;

//-----------------------------------------------------------------------------
//! Writer class for ipxact:subSpaceMap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SubSpaceMapWriter : public CommonItemsWriter
{
public:

    /*!
     *  The constructor.
     */
    SubSpaceMapWriter();

    /*!
     *  The destructor.
     */
    ~SubSpaceMapWriter() = default;

    /*!
     *  Write a subspace map to an XML file.
     *
     *      @param [in] writer  The used XML writer.
     *      @param [in] subMap  The subspace map to be written.
     */
    void writeSubSpaceMap(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap) const;

private:

    //! No copying allowed.
    SubSpaceMapWriter(SubSpaceMapWriter const& rhs);
    SubSpaceMapWriter& operator=(SubSpaceMapWriter const& rhs);
    
    /*!
     *  Writes the attributes of a given subspace map into xml.
     *
     *      @param [in] writer  The xml writer to use.
     *      @param [in] subMap  The subspace map whose attributes to write.
     */
    void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap) const;

    /*!
     *  Write the name group.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] subMap  Selected subspace map.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap) const;

    /*!
     *  Write the base address.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] subMap  The subspace map whose base address to write.
     */
    void writeBaseAddress(QXmlStreamWriter& writer, QSharedPointer<SubSpaceMap> subMap) const;
};

#endif // SUBSPACEMAPWRITER_H