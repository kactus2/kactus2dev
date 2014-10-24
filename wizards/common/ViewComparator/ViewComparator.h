//-----------------------------------------------------------------------------
// File: ViewComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.10.2014
//
// Description:
// Comparator for finding differences in views.
//-----------------------------------------------------------------------------

#ifndef VIEWCOMPARATOR_H
#define VIEWCOMPARATOR_H

#include <wizards/common/ListComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <IPXACTmodels/view.h>

//-----------------------------------------------------------------------------
//! Comparator for finding differences in views.
//-----------------------------------------------------------------------------
class ViewComparator : public ListComparator<View>
{
public:

    //! The constructor.    
    ViewComparator();

    //! The destructor.
    virtual ~ViewComparator();
    

    virtual bool compare(QSharedPointer<const View> first, QSharedPointer<const View> second) const;

    /*!
     *  Compares the sub-elements of two views.
     *
     *      @param [in] first    The first view.
     *      @param [in] second   The second view.
     *
     *      @return True, if the sub-elements are similar, otherwise false.
     */
    bool compareFields(QSharedPointer<const View> first, QSharedPointer<const View> second) const;

    /*!
     *  Compares the two lists of views.
     *
     *      @param [in] first    The first lists.
     *      @param [in] second   The second lists.
     *
     *      @return True, if the lists are similar, otherwise false.
     */
    virtual bool compare(QList<QSharedPointer<View> > const first, QList<QSharedPointer<View> > const second) const;

    virtual QList<QSharedPointer<IPXactDiff> > diff(QSharedPointer<const View> reference, 
        QSharedPointer<const View> subject) const;

    /*!
     *  Finds the differences between the sub-elements of two given views.
     *
     *      @param [in] reference   The reference view to compare to.
     *      @param [in] subject     The view to compare against the reference.
     *
     *      @return Set of differences between the sub-elements of reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const View> reference, 
        QSharedPointer<const View> subject) const;

    /*!
     *  Finds the differences between the two given lists.
     *
     *      @param [in] reference   The reference lists to compare to.
     *      @param [in] subject     The lists to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diff(QList<QSharedPointer<View> > const reference, 
        QList<QSharedPointer<View> > const subject) const;
    
    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const;

private:

    /*!
     *  Compares two string lists.
     *
     *      @param [in] first    The first list.
     *      @param [in] second   The second list.
     *
     *      @return True, if the lists contain the same strings in any order, otherwise false.
     */
    bool compareLists(QStringList first, QStringList second) const;

    /*!
     *  Gets the type for a given view.
     *
     *      @param [in] view    The view whose type to get.
     *
     *      @return The type (hierarchical/non-hierarchical) of the view.
     */
    QString viewHierarchyType(QSharedPointer<const View> view) const;


};

#endif // VIEWCOMPARATOR_H
