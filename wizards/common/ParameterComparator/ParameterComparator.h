//-----------------------------------------------------------------------------
// File: ParameterComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERCOMPARATOR_H
#define PARAMETERCOMPARATOR_H

#include <QList>
#include <QSharedPointer>

#include <wizards/common/IPXactDiff.h>
#include <wizards/common/ListComparator.h>

class Parameter;

//-----------------------------------------------------------------------------
//! Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------
class ParameterComparator : public ListComparator<Parameter>
{
public:
    //! The constructor.
    ParameterComparator();

    //! The destructor.
    virtual ~ParameterComparator();

    /*!
     *  Finds the differences between the two given model parameters.
     *
     *      @param [in] reference   The reference model parameter to compare to.
     *      @param [in] subject     The model parameters to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual bool compare(QSharedPointer<Parameter> first, QSharedPointer<Parameter> second) const;
    
    /*!
     *  Compares the sub-elements of two model parameters.
     *
     *      @param [in] first    The first model parameters.
     *      @param [in] second   The second model parameters.
     *
     *      @return True, if the sub-elements are similar, otherwise false.
     */
    virtual bool compareFields(QSharedPointer<const Parameter> first, 
        QSharedPointer<const Parameter> second) const;

    /*!
     *  Compares the two lists of model parameters.
     *
     *      @param [in] first    The first lists.
     *      @param [in] second   The second lists.
     *
     *      @return True, if the lists are similar, otherwise false.
     */
    bool compare(QList<QSharedPointer<Parameter> > const first, QList<QSharedPointer<Parameter> > const second);
    
    /*!
     *  Finds the differences between the sub-elements of two given model parameters.
     *
     *      @param [in] reference   The reference model parameter to compare to.
     *      @param [in] subject     The model parameter to compare against the reference.
     *
     *      @return Set of differences between the sub-elements of reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const Parameter> reference,
        QSharedPointer<const Parameter> subject) const;
    
    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const;

private:

    //! Disable copying.
    ParameterComparator(ParameterComparator const& rhs);
    ParameterComparator& operator=(ParameterComparator const& rhs);

};

#endif // PARAMETERCOMPARATOR_H
