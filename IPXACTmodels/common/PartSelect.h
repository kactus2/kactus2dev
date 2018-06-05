//-----------------------------------------------------------------------------
// File: PartSelect.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.08.2015
//
// Description:
// Describes an ipxact:partSelect element.
//-----------------------------------------------------------------------------

#ifndef PARTSELECT_H
#define PARTSELECT_H

#include "Range.h"

#include <IPXACTmodels/ipxactmodels_global.h>


#include <QString>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Describes an ipxact:partSelect element.
//
// PartSelect defines a selection of subelements from within a referenced port element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PartSelect
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] leftRange   The left value for the range.
     *      @param [in] rightRange  The right value for the range.
     */
    PartSelect(QString const& leftRange = QString(), QString const& rightRange = QString());

    /*!
     *  Copy constructor.
     */
    PartSelect(const PartSelect& other);

    /*!
     *  The destructor.
     */
    virtual ~PartSelect();

    /*!
     *  Assignment operator.
     */
    PartSelect& operator=(const PartSelect& other);

    /*!
     *  Get the left value of the range.
     *
     *      @return The left value of the range.
     */
    QString getLeftRange() const;

    /*!
     *  Get the right value of the range.
     *
     *      @return The right value of the range.
     */
    QString getRightRange() const;

    /*!
     *  Set the left value for the range.
     *
     *      @param [in] newLeftRange    The new value for the left value of the range.
     */
    void setLeftRange(QString const& newLeftRange);

    /*!
     *  Set the right value for the range.
     *
     *      @param [in] newRightRange   The new value for the right value of the range.
     */
    void setRightRange(QString const& newRightRange);

    /*!
     *  Get the list of indexes.
     *
     *      @return Pointer to a list of indexes,
     */
    QSharedPointer<QStringList> getIndices() const;

    /*!
     *  Set the list of indexes.
     *
     *      @param [in] newIndices  A new list of indexes.
     */
    void setIndices(QSharedPointer<QStringList> newIndices);

private:

    //! The range.
    Range range_;

    //! The list of indexes.
    QSharedPointer<QStringList> indices_;
};
#endif // PARTSELECT_H
