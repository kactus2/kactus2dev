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

	TransactionalAbstraction();

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
     *      @param [in] qualifierType   The qualifier to set.
     */
    void setQualifier(Qualifier::Type qualifierType);
    
    /*!
     *  Gets the qualifier for the information the transaction carries.
     *
     *      @return The wire qualifier.
     */
	Qualifier getQualifier() const;

    /*!
     *  Adds a system port definition for the transaction definition.
     *
     *      @param [in] systemPort   The system transaction port to add.
     */
    void addSystemPort(QSharedPointer<TransactionalPort> systemPort);

    /*!
     *  Gets the system port definitions for the transaction definition.
     *
     *      @return The system ports for the transaction definition.
     */
    QSharedPointer<QList<QSharedPointer<TransactionalPort> > > getSystemPorts() const;
    
    /*!
     *  Checks if the transaction definition has a port definition for master mode.
     *
     *      @return True, if master mode is defined, otherwise false.
     */
    bool hasMasterPort() const;
       
    /*!
     *  Sets the master mode port definitions for the transaction definition.
     *
     *      @param [in] masterPort The master port definition for the transaction definition.
     */ 	
    void setMasterPort(QSharedPointer<TransactionalPort> masterPort);

    /*!
     *  Gets the master mode port definitions for the transaction definition.
     *
     *      @return The master port definition for the  transaction definition.
     */
	QSharedPointer<TransactionalPort> getMasterPort() const;

    /*!
     *  Checks if the transaction definition has a port definition for slave mode.
     *
     *      @return True, if slave mode is defined, otherwise false.
     */
    bool hasSlavePort() const;
      
    /*!
     *  Sets the slave mode port definitions for the transaction definition.
     *
     *      @param [in] slavePort The slave port definition for the transaction definition.
     */
	void setSlavePort(QSharedPointer<TransactionalPort> slavePort);
            
    /*!
     *  Gets the slave mode port definitions for the transaction definition.
     *
     *      @return The slave port definition for the transaction definition.
     */
    QSharedPointer<TransactionalPort> getSlavePort() const;

private:

    //! The qualifier describing the information the transaction carries.
	Qualifier qualifier_;

    //! The port definitions for system mode.
    QSharedPointer<QList<QSharedPointer<TransactionalPort> > > onSystem_;

    //! The port definition for master mode.
	QSharedPointer<TransactionalPort> onMaster_;

    //! The port definition for slave mode.
	QSharedPointer<TransactionalPort> onSlave_;
};

#endif /* TRANSACTIONALABSTRACTION_H */
