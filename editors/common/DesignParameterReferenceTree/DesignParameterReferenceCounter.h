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
class AdHocConnection;
class PartSelect;
class AbstractParameterInterface;

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
    ~DesignParameterReferenceCounter() = default;

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

    /*!
     *  Count the number of references made to the selected parameter in the ad hoc connections.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] connections     The selected ad hoc connections.
     *
     *      @return The number of references made to the selected parameter in the ad hoc connections.
     */
    int countReferencesInAdHocConnections(QString const& parameterID,
        QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections) const;

    /*!
     *  Count the number of references made to the selected parameter in a single ad hoc connection.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] connection      The selected ad hoc connection.
     *
     *      @return The number of references made to the selected parameter in the selected ad hoc connection.
     */
    int countReferencesInSingleAdHocConnection(QString const& parameterID,
        QSharedPointer<AdHocConnection> connection) const;

    /*!
     *  Count the number of references made to the selected parameter in a single part select.
     *
     *      @param [in] parameterID     ID of the selected parameter.
     *      @param [in] partSelect      The selected part select.
     *
     *      @return The number of references made to the selected parameter in the selected part select.
     */
    int countReferencesInPartSelect(QString const& parameterID, QSharedPointer<PartSelect> partSelect) const;

public slots:

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *      @param [in] parameterList       The selected parameters.
     *      @param [in] parameterInterface  Interface for accessing parameters.
     */
    virtual void recalculateReferencesToParameters(QVector<QString> const& parameterList,
        AbstractParameterInterface* parameterInterface);

private:

	//! No copying. No assignment.
    DesignParameterReferenceCounter(const DesignParameterReferenceCounter& other);
    DesignParameterReferenceCounter& operator=(const DesignParameterReferenceCounter& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The design.
    QSharedPointer<Design> design_;
};

#endif // DESIGNPARAMETERREFERENCECOUNTER_H
