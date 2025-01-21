//-----------------------------------------------------------------------------
// File: AbsDefParameterReferenceCounter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.08.2024
//
// Description:
// Calculates the amount of references made to abstraction definition parameters.
//-----------------------------------------------------------------------------

#ifndef ABSDEFPARAMETERREFERENCECOUNTER_H
#define ABSDEFPARAMETERREFERENCECOUNTER_H

#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

class AbstractionDefinition;
class AbstractParameterInterface;
class WirePort;
class WireAbstraction;
class TransactionalPort;
class TransactionalAbstraction;

class AbsDefParameterReferenceCounter : public ParameterReferenceCounter
{
    Q_OBJECT

public:

    /*!
     *	The constructor.
     *  
     *    @param [in] parameterFinder     The parameter finder to use.
     *    @param [in] absDef              The abstraction definition whose parameter usage is calculated.
     */
    AbsDefParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<AbstractionDefinition> absDef);
    
    AbsDefParameterReferenceCounter(AbsDefParameterReferenceCounter const& other) = delete;
    AbsDefParameterReferenceCounter& operator=(AbsDefParameterReferenceCounter const& other) = delete;
    
    virtual ~AbsDefParameterReferenceCounter() = default;

    /*!
     *	Set a new abstraction definition.
     *  
     *    @param [in] absDef     The new abstraction definition.
     */
    void setAbstractionDefinition(QSharedPointer<AbstractionDefinition> absDef);

    /*!
     *	Count references to abs def parameters in wire ports.
     *  
     *    @param [in] parameterID     The parameter ID to look for.
     *	    
     * 	    @return The parameter usage count in wire ports.
     */
    int countReferencesInWirePorts(QString const& parameterID);

    /*!
     *	Count references to abs def parameters in transactional ports.
     *
     *    @param [in] parameterID     The parameter ID to look for.
     *
     * 	    @return The parameter usage count in transactional ports.
     */
    int countReferencesInTransactionalPorts(QString const& parameterID);

    /*!
     *	Count references to abs def parameters in a single wire port.
     *  
     *    @param [in] parameterID     The parameter ID to look for.
     *    @param [in] wirePort        The wire port to look into.
     *	    
     * 	    @return The parameter usage count in the wire port.
     */
    int countReferencesInWirePort(QString const& parameterID, QSharedPointer<WireAbstraction> wirePort);

    /*!
     *	Count references to abs def parameters in a single transactional port.
     *
     *    @param [in] parameterID     The parameter ID to look for.
     *    @param [in] transactional   The transactional port to look into.
     *
     * 	    @return The parameter usage count in the transactional port.
     */
    int countReferencesInTransactionalPort(QString const& parameterID, QSharedPointer<TransactionalAbstraction> transactional);

public slots:

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *    @param [in] parameterList       The selected parameters.
     *    @param [in] parameterInterface  Interface for accessing parameters.
     */
    virtual void recalculateReferencesToParameters(QStringList const& parameterList,
        AbstractParameterInterface* parameterInterface) override;

private:

    //! The current abstraction definition in use.
    QSharedPointer<AbstractionDefinition> absDef_;
};

#endif // ABSDEFPARAMETERREFERENCECOUNTER_H