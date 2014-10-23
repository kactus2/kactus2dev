//-----------------------------------------------------------------------------
// File: VLNVComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in VLVN elements.
//-----------------------------------------------------------------------------

#ifndef VLNVCOMPARATOR_H
#define VLNVCOMPARATOR_H

#include <wizards/common/IPXactDiff.h>
#include <wizards/common/IPXactElementComparator.h>

#include <IPXACTmodels/vlnv.h>

#include <QList>

//-----------------------------------------------------------------------------
//! Comparator for finding differences in VLVN elements.
//-----------------------------------------------------------------------------
class VLNVComparator : public IPXactElementComparator<VLNV>
{
public:

	//! The constructor.
	VLNVComparator();

	//! The destructor.
	~VLNVComparator();
    
    /*!
     *  Compares the two VLNV elements.
     *
     *      @param [in] first    The first VLNV.
     *      @param [in] second   The second VLNV.
     *
     *      @return True, if the VLNVs are similar, otherwise false.
     */
    virtual bool compareFields(QSharedPointer<const VLNV> first, QSharedPointer<const VLNV> second) const;

    /*!
     *  Finds the differences between the two given VLNVs.
     *
     *      @param [in] reference   The VLNVs to compare to.
     *      @param [in] subject     The VLNVs to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const VLNV> reference, 
        QSharedPointer<const VLNV> subject) const;
    
    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
     virtual QString elementType() const;

private:

	//! Disable copying.
	VLNVComparator(VLNVComparator const& rhs);
	VLNVComparator& operator=(VLNVComparator const& rhs);
};

#endif // VLNVCOMPARATOR_H
