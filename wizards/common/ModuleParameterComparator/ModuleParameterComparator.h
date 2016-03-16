//-----------------------------------------------------------------------------
// File: ModuleParameterComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in lists of module parameters.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETERCOMPARATOR_H
#define MODULEPARAMETERCOMPARATOR_H

#include <QList>
#include <QSharedPointer>

#include <wizards/common/IPXactDiff.h>
#include <wizards/common/ListComparator.h>

class ModuleParameter;
//-----------------------------------------------------------------------------
//! Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------
class ModuleParameterComparator : public ListComparator<ModuleParameter>
{
public:
    //! The constructor.
    ModuleParameterComparator();

    //! The destructor.
    ~ModuleParameterComparator();

    /*!
     *  Finds the differences between the two given module parameters.
     *
     *      @param [in] reference   The reference module parameter to compare to.
     *      @param [in] subject     The module parameter to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual bool compare(QSharedPointer<ModuleParameter> first, QSharedPointer<ModuleParameter> second) const;
    
    /*!
     *  Compares the sub-elements of two module parameters.
     *
     *      @param [in] first    The first module parameters.
     *      @param [in] second   The second module parameters.
     *
     *      @return True, if the sub-elements are similar, otherwise false.
     */
    virtual bool compareFields(QSharedPointer<const ModuleParameter> first, 
        QSharedPointer<const ModuleParameter> second) const;

    /*!
     *  Compares the two lists of module parameters.
     *
     *      @param [in] first    The first lists.
     *      @param [in] second   The second lists.
     *
     *      @return True, if the lists are similar, otherwise false.
     */
    bool compare(QList<QSharedPointer<ModuleParameter> > const first, 
        QList<QSharedPointer<ModuleParameter> > const second) ;
    
    /*!
     *  Finds the differences between the sub-elements of two given module parameters.
     *
     *      @param [in] reference   The reference module parameter to compare to.
     *      @param [in] subject     The module parameter to compare against the reference.
     *
     *      @return Set of differences between the sub-elements of reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const ModuleParameter> reference,
        QSharedPointer<const ModuleParameter> subject) const;
    
    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const;

private:

    //! Disable copying.
    ModuleParameterComparator(ModuleParameterComparator const& rhs);
    ModuleParameterComparator& operator=(ModuleParameterComparator const& rhs);

};

#endif // MODULEPARAMETERCOMPARATOR_H
