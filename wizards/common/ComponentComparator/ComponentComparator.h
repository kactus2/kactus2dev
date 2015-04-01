//-----------------------------------------------------------------------------
// File: ComponentComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.10.2014
//
// Description:
// Comparator for finding differences in components.
//-----------------------------------------------------------------------------

#ifndef COMPONENTCOMPARATOR_H
#define COMPONENTCOMPARATOR_H

#include <wizards/common/IPXactDiff.h>
#include <wizards/common/IPXactElementComparator.h>

#include <QSharedPointer>

class Component;
class ModelParameter;
class Parameter;
class VLNV;

//-----------------------------------------------------------------------------
//! Comparator for finding differences in components.
//-----------------------------------------------------------------------------
class ComponentComparator : public IPXactElementComparator<Component>
{
public:

	//! The constructor.
	ComponentComparator();

	//! The destructor.
	~ComponentComparator();

    /*!
     *  Compares the sub-elements of two components.
     *
     *      @param [in] first    The first components.
     *      @param [in] second   The second components.
     *
     *      @return True, if the sub-elements are similar, otherwise false.
     */
    bool compareFields(QSharedPointer<const Component> first, QSharedPointer<const Component> second) const;

    /*!
     *  Finds the differences between the two given components.
     *
     *      @param [in] reference   The component to compare to.
     *      @param [in] subject     The component to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diff(QSharedPointer<const Component> reference,
        QSharedPointer<const Component> subject) const;
    
    /*!
     *  Finds the differences between the sub-elements of two given elements.
     *
     *      @param [in] reference   The reference element to compare to.
     *      @param [in] subject     The element to compare against the reference.
     *
     *      @return Set of differences between the sub-elements of reference and subject.
     */
    QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const Component> reference, 
        QSharedPointer<const Component> subject) const;
    
    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const;

private:

	// Disable copying.
	ComponentComparator(ComponentComparator const& rhs);
	ComponentComparator& operator=(ComponentComparator const& rhs);

    /*!
     *  Compares the VLNVs in the given components.
     *
     *      @param [in] referenceComponent  The reference component to compare to.
     *      @param [in] subjectComponent    The subject component to compare against the reference.
     *
     *      @return True, if the VLVNs are similar, otherwise false.
     */
    bool compareVLNVs(QSharedPointer<const Component> referenceComponent, 
        QSharedPointer<const Component> subjectComponent) const;
    
    /*!
     *  Compares the model parameters in the given components.
     *
     *      @param [in] referenceComponent  The reference component to compare to.
     *      @param [in] subjectComponent    The subject component to compare against the reference.
     *
     *      @return True, if the model parameters are similar, otherwise false.
     */
    bool compareModelParameters(QSharedPointer<const Component> referenceComponent, 
        QSharedPointer<const Component> subjectComponent) const;

    /*!
     *  Compares the parameters in the given components.
     *
     *      @param [in] referenceComponent  The reference component to compare to.
     *      @param [in] subjectComponent    The subject component to compare against the reference.
     *
     *      @return True, if the parameters are similar, otherwise false.
     */  
    bool compareParameters(QSharedPointer<const Component> referenceComponent, 
        QSharedPointer<const Component> subjectComponent) const;
    
    /*!
     *  Compares the ports in the given components.
     *
     *      @param [in] referenceComponent  The reference component to compare to.
     *      @param [in] subjectComponent    The subject component to compare against the reference.
     *
     *      @return True, if the ports are similar, otherwise false.
     */  
    bool comparePorts(QSharedPointer<const Component> first, QSharedPointer<const Component>second) const;
    
    /*!
     *  Compares the views in the given components.
     *
     *      @param [in] referenceComponent  The reference component to compare to.
     *      @param [in] subjectComponent    The subject component to compare against the reference.
     *
     *      @return True, if the views are similar, otherwise false.
     */  
    bool compareViews(QSharedPointer<const Component> first, QSharedPointer<const Component> second) const;
};

#endif // COMPONENTCOMPARATOR_H
