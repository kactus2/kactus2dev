//-----------------------------------------------------------------------------
// File: TransactionalAbstraction.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Implementation for ipaxct:transactional within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALABSTRACTION_H
#define TRANSACTIONALABSTRACTION_H

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Qualifier.h>

#include <QSharedPointer>
#include <QList>
#include <QDomNode>
#include <QString>

class TransactionalPort;

//-----------------------------------------------------------------------------
//! Implementation for ipaxct:transactional within abstraction definition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT TransactionalAbstraction
{
public:

	TransactionalAbstraction() = default;

	//! \brief Copy constructor
	TransactionalAbstraction(const TransactionalAbstraction& other);

	//! \brief Assignment operator
	TransactionalAbstraction& operator=(const TransactionalAbstraction& other);

	/*! \brief The destructor
	 *
	 */
	~TransactionalAbstraction();

    /*!
     *  Sets a qualifier for the information the transaction carries.
     *
     *    @param [in] qualifier   The qualifier to set.
     */
    void setQualifier(QSharedPointer<Qualifier> qualifier);

    /*!
     *  Add a qualifier type for the information the transaction carries.
     *
     *    @param [in] qualifierType   The qualifier type to add.
     */
    void addQualifier(Qualifier::Type qualifierType);

    /*!
     *  Gets the qualifier for the information the transaction carries.
     *
     *    @return The wire qualifier.
     */
    QSharedPointer<Qualifier> getQualifier();

    /*!
     *  Adds a system port definition for the transaction definition.
     *
     *    @param [in] systemPort   The system transaction port to add.
     */
    void addSystemPort(QSharedPointer<TransactionalPort> systemPort);

    /*!
     *  Gets the system port definitions for the transaction definition.
     *
     *    @return The system ports for the transaction definition.
     */
    QSharedPointer<QList<QSharedPointer<TransactionalPort> > > getSystemPorts() const;
    
    /*!
     *  Find the selected system port.
     *
     *    @param [in] systemGroup     The selected interface system group.
     *
     *    @return The selected system port, if it exists.
     */
    QSharedPointer<TransactionalPort> findSystemPort(QString const& systemGroup) const;

    /*!
     *  Checks if the transactional has a port definition for initiator mode.
     *
     *    @return True, if initiator mode is defined, otherwise false.
     */
    bool hasInitiatorPort() const;

    /*!
     *  Sets the initiator mode port definitions for the transactional.
     *
     *    @param [in] initiatorPort The initiator port definition for the transactional.
     */
    void setInitiatorPort(QSharedPointer<TransactionalPort> initiatorPort);

    /*!
     *  Gets the initiator mode port definitions for the transactional.
     *
     *    @return The initiator port definition for the transactional.
     */
    QSharedPointer<TransactionalPort> getInitiatorPort() const;

    /*!
     *  Checks if the transactional has a port definition for target mode.
     *
     *    @return True, if target mode is defined, otherwise false.
     */
    bool hasTargetPort() const;

    /*!
     *  Sets the target mode port definitions for the transactional.
     *
     *    @param [in] slavePort The target port definition for the transactional.
     */
    void setTargetPort(QSharedPointer<TransactionalPort> targetPort);

    /*!
     *  Gets the target mode port definitions for the transactional.
     *
     *    @return The target port definition for the transactional.
     */
    QSharedPointer<TransactionalPort> getTargetPort() const;

    /*!
     *  Checks if the transaction definition has a port definition for master mode.
     *
     *    @return True, if master mode is defined, otherwise false.
     */
    bool hasMasterPort() const;
       
    /*!
     *  Sets the master mode port definitions for the transaction definition.
     *
     *    @param [in] masterPort The master port definition for the transaction definition.
     */ 	
    void setMasterPort(QSharedPointer<TransactionalPort> masterPort);

    /*!
     *  Gets the master mode port definitions for the transaction definition.
     *
     *    @return The master port definition for the  transaction definition.
     */
	QSharedPointer<TransactionalPort> getMasterPort() const;

    /*!
     *  Checks if the transaction definition has a port definition for slave mode.
     *
     *    @return True, if slave mode is defined, otherwise false.
     */
    bool hasSlavePort() const;
      
    /*!
     *  Sets the slave mode port definitions for the transaction definition.
     *
     *    @param [in] slavePort The slave port definition for the transaction definition.
     */
	void setSlavePort(QSharedPointer<TransactionalPort> slavePort);
            
    /*!
     *  Gets the slave mode port definitions for the transaction definition.
     *
     *    @return The slave port definition for the transaction definition.
     */
    QSharedPointer<TransactionalPort> getSlavePort() const;

    /*!
     *  Check if the transactional has the selected interface mode.
     *
     *    @param [in] mode            The selected interface mode.
     *    @param [in] systemGroup     The selected interface system group.
     *
     *    @return True, if the transactional has the selected mode, otherwise false.
     */
    bool hasMode(General::InterfaceMode mode, QString const& systemGroup) const;

    /*!
     *  Get the initiative of the selected interface mode.
     *
     *    @param [in] mode            The selected interface mode.
     *    @param [in] systemGroup     The selected interface system group.
     *
     *    @return The initiative of the selected mode.
     */
    QString getInitiative(General::InterfaceMode mode, QString const& systemGroup) const;

    /*!
     *  Get the width of the selected interface mode.
     *
     *    @param [in] mode            The selected interface mode.
     *    @param [in] systemGroup     The selected interface system group.
     *
     *    @return The width of the selected mode.
     */
    QString getWidth(General::InterfaceMode mode, QString const& systemGroup) const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The qualifier describing the information the transaction carries.
    QSharedPointer<Qualifier> qualifier_{ new Qualifier() };

    //! The port definition for initiator mode.
	QSharedPointer<TransactionalPort> onInitiator_{ nullptr };

    //! The port definition for target mode.
    QSharedPointer<TransactionalPort> onTarget_{ nullptr };

    //! The port definitions for system mode.
    QSharedPointer<QList<QSharedPointer<TransactionalPort> > > onSystem_{ 
        new QList<QSharedPointer<TransactionalPort> >() };
    
};

#endif /* TRANSACTIONALABSTRACTION_H */
