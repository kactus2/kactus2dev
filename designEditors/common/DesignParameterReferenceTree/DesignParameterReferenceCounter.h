//-----------------------------------------------------------------------------
// File: DesignParameterReferenceCounter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.10.2017
//
// Description:
// Calculates the amount of references made to design parameters.
//-----------------------------------------------------------------------------

#ifndef DESIGNPARAMETERREFERENCECOUNTER_H
#define DESIGNPARAMETERREFERENCECOUNTER_H

#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

#include <QVector>

class Design;
class Parameter;
class ComponentInstance;

//-----------------------------------------------------------------------------
//! Calculates the amount of references made to design parameters.
//-----------------------------------------------------------------------------
class DesignParameterReferenceCounter : public ParameterReferenceCounter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterFinder     Locates the parameters for increasing and decreasing references.
     */
    DesignParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder);

    /*!
     *  The destructor.
     */
    ~DesignParameterReferenceCounter();

    /*!
     *  Set a new design for the reference counter.
     *
     *      @param [in] newDesign   The selected design.
     */
    void setDesign(QSharedPointer<Design> newDesign);

    /*!
     *  Count the number of references made to the selected parameter in the contained component instances.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *
     *      @return The number of references made to the selected parameter in the contained component instances.
     */
    int countReferencesInComponentInstances(QString const& parameterID) const;

public slots:

    /*!
     *  Recalculate the references made to the selected parameters.
     *
     *      @param [in] parameterList   List of the selected parameters.
     */
    virtual void recalculateReferencesToParameters(QVector<QSharedPointer<Parameter> > parameterList);

private:

	//! No copying. No assignment.
    DesignParameterReferenceCounter(const DesignParameterReferenceCounter& other);
    DesignParameterReferenceCounter& operator=(const DesignParameterReferenceCounter& other);
    
    /*!
     *  Count the number of references made to the selected parameter in a single component instance.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] instance        The selected component instance.
     *
     *      @return The number of references made to the selected parameter in the selected component instance.
     */
    int countReferencesInSingleComponentInstance(QString const& parameterID,
        QSharedPointer<ComponentInstance> instance) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design.
    QSharedPointer<Design> design_;
};

#endif // DESIGNPARAMETERREFERENCECOUNTER_H
