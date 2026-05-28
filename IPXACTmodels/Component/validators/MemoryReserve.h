//-----------------------------------------------------------------------------
// File: MemoryReserve.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.11.2015
//
// Description:
// Class for determining overlapping within an area.
//-----------------------------------------------------------------------------

#ifndef MEMORYRESERVE_H
#define MEMORYRESERVE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QHash>
#include <QObject>

//-----------------------------------------------------------------------------
//! Class for determining overlapping within an area.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryReserve
{
public:

	/*!
	 *  The constructor.
	 */
    MemoryReserve();

    //! The destructor.
    ~MemoryReserve() = default;

    // Disable copying.
    MemoryReserve(MemoryReserve const& rhs) = delete;
    MemoryReserve& operator=(MemoryReserve const& rhs) = delete;

    /*!
     *  Reserve an area.
     *
     *    @param [in] newId       Id of the reserved area.
     *    @param [in] newBegin    Start point of the area.
     *    @param [in] newEnd      End point of the area.
     */
    void addArea(QString const& newId, quint64 newBegin, quint64 newEnd);

    /*!
     *  Check if reserved areas overlap.
     *
     *    @return True, if overlap exists, otherwise false.
     */
    bool hasOverlap();

    /*!
     *	Check for overlaps and containment issues of items in reserve.
     *  
     *    @param [in/out] erroneousAreas    Areas, that are either overlapping or out of bounds for parent item.
     *    @param [in]     rangeMax          Range/size of the parent item.
     *    @param [in]     checkContainment  Flag indicating if containment should be checked.
     */
    void checkOverlapAndContainment(QSet<QString>& erroneousAreas, quint64 rangeMax, bool checkContainment = true);

    /*!
     *  Check if the reserved ares with the same identifier overlap.
     *
     *    @return True, if overlap exists, otherwise false.
     */
    bool hasIdDependantOverlap();

    /*!
     *  Find errors related to overlapping.
     *
     *    @param [in] errors          The error list.
     *    @param [in] itemIdentifier  Identifier for the overlapping items.
     *    @param [in] context         Context used to help find the error.
     */
    void findErrorsInOverlap(QVector<QString>& errors, QString const& itemIdentifier, QString const& context);

    /*!
     *	Find errors related to memory area containment inside containing item
     *  
     *    @param [in] errors        The error list.
     *    @param [in] itemId        Identifier for the item to check.
     *    @param [in] itemType      Description of type of item, used in error message.
     *    @param [in] context       The context for the error message.
     *    @param [in] areaStart     The start of the containing area.
     *    @param [in] areaEnd       The end of the containing area.
     */
    void findErrorsInContainment(QList<QString>& errors, QString const& itemId, QString const& itemType, QString const& context, 
        quint64 areaStart, quint64 areaEnd);

    /*!
     *  Find errors related to overlapping with same identifiers.
     *
     *    @param [in] errors          The error list.
     *    @param [in] itemIdentifier  Identifier for the overlapping items.
     *    @param [in] context         Context used to help find the errors.
     */
    void findErrorsInIdDependantOverlap(QVector<QString>& errors, QString const& itemIdentifier,
        QString const& context);

private:

    void sortIfNotSorted();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    struct MemoryEndPoint;

    // A memory area.
    struct MemoryArea
    {
        //! Identifier for the area.
        QString id_;
        
        //! Index for items with replicas due to dimension
        uint index_ = 0;

        //! Start point of the area.
        //quint64 begin_ = 0;
        QSharedPointer<MemoryEndPoint> start_{ nullptr };

        //! End point of the area.
        //quint64 end_ = 0;
        QSharedPointer<MemoryEndPoint> end_{ nullptr };

        /*!
         *  Assignment operator for MemoryArea.
         *
         *    @param [in] other   Memory area to be assigned.
         *
         *    @return The newly assigned memory area.
         */
        MemoryArea& operator=(const MemoryArea& other) = default;
        
        /*!
         *  Check if a memory area has lower start point than another memory area.
         *
         *    @param [in] other   Memory area being compared to.
         */
        bool operator<(const MemoryArea& other) const;
    };

    struct MemoryEndPoint
    {
        bool operator<(MemoryEndPoint const& other) const;

        MemoryEndPoint& operator=(MemoryEndPoint const& other)
        {
            if (this != &other)
            {
                coord_ = other.coord_;
                isBegin_ = other.isBegin_;
                parentArea_ = parentArea_;
            }

            return *this;
        }

        quint64 coord_ = 0;
        bool isBegin_ = true;
        MemoryArea* parentArea_ = nullptr;
    };


    //! The currently reserved memory areas.
    QList<QSharedPointer<MemoryArea > > reservedArea_;

    QHash<QString, uint> addedIds_;

    QList<QSharedPointer<MemoryEndPoint> > endPoints_;

    bool areaIsSorted_ = false;
};

#endif // MEMORYRESERVE_H
