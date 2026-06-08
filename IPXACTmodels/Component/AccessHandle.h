//-----------------------------------------------------------------------------
// File: AccessHandle.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Describes the ipxact:accessHandle element.
//-----------------------------------------------------------------------------

#pragma once

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/BooleanValue.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/Slice.h>

#include <IPXACTmodels/ipxactmodels_global.h>

struct PathSegment;

class IPXACTMODELS_EXPORT AccessHandle : public Extendable
{
public:

    //! The different types of access handles, by standard revision
    enum Type
    {
        Simple14 = 0,       // banks (std14)
        Simple22,           // banks, register files, registers and alternate registers (std22)
        NonIndexedLeaf14,   // address blocks and fields (std14)
        Indexed14,          // alternate register, register and register files (std14)
        Leaf14,             // ports (std14)
        Sliced22,           // address blocks and fields (std22)
        Port22,             // ports (std22)
        Unknown
    };

    //! The different elements that can contain access handles
    enum class ElementType
    {
        AddressBlock = 0,
        Bank,
        Register,           // Includes register file and alternate register
        Field,
        Port,
        Unknown
    };

    /*!
     *  Computes the correct access handle type based on containing element type and standard revision.
     *
     *    @param [in] element           The containing element type (e.g. address block).
     *    @param [in] docRevision       The standard revision in use.
     *
     *    @return The computed access handle type.
     */
    static Type typeFromElementAndRevision(ElementType element, Document::Revision docRevision);

    /*!
     *  The constructor.
     */
    AccessHandle();

    /*!
     *  Copy constructor.
     */
    AccessHandle(AccessHandle const& other);

    /*!
     *  Assignment operator.
     */
    AccessHandle& operator=(AccessHandle const& other);

    /*!
     *  The destructor.
     */
    virtual ~AccessHandle() = default;

    /*!
     *  Set the force value.
     *
     *    @param [in] force   The force value to set.
     */
    void setForce(bool force);

    /*!
     *  Get the force value.
     *
     *    @return The force value.
     */
    QString getForce() const;

    /*!
     *  Set the indices.
     *
     *    @param [in] indices   The indices to set.
     */
    void setIndices(QStringList const& indices);

    /*!
     *  Get the indices.
     *
     *    @return The indices.
     */
    QStringList getIndices() const;

    /*!
     *  Set the slices.
     *
     *    @param [in] slices   A pointer to the list of slices to set.
     */
    void setSlices(QSharedPointer<QList<QSharedPointer<Slice> > > slices);

    /*!
     *  Get the slices.
     *
     *    @return A pointer to the list of slices.
     */
    QSharedPointer<QList<QSharedPointer<Slice> > > getSlices() const;

    /*!
     *  Set the view references.
     *
     *    @param [in] viewRefs   The view references to set.
     */
    void setViewReferences(QStringList const& viewRefs);

    /*!
     *  Get the view references.
     *
     *    @return The view references.
     */
    QStringList getViewReferences() const;

    /*!
     *  Set the path segments.
     *
     *    @param [in] pathSegments   A pointer to the list of path segments to set.
     */
    void setPathSegments(QSharedPointer<QList<QSharedPointer<PathSegment> > > pathSegments);

    /*!
     *  Get the path segments.
     *
     *    @return A pointer to the list of path segments.
     */
    QSharedPointer<QList<QSharedPointer<PathSegment> > > getPathSegments() const;

private:

    // 2014 nonIndexedLeafAccessHandle and leafAccessHandle, 2022 slicedAccessHandle, portAccessHandle
    BooleanValue force_;

    // 2014 indexedAccessHandle, portAccessHandle
    QStringList indices_;

    // 2014 leafAccessHandle and 2022 slicedAccessHandle, portAccessHandle
    QSharedPointer<QList<QSharedPointer<Slice> > > slices_ =
        QSharedPointer<QList<QSharedPointer<Slice> > >(new QList<QSharedPointer<Slice> >());

    // All access handles
    QStringList viewRefs_;

    // 2014 and 2022 simpleAccessHandle, 2014 indexedAccessHandle
    QSharedPointer<QList<QSharedPointer<PathSegment> > > pathSegments_ =
        QSharedPointer<QList<QSharedPointer<PathSegment> > >(new QList<QSharedPointer<PathSegment> >());
};