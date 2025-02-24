//-----------------------------------------------------------------------------
// File: AbstractionDefinition.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Implementation for ipxact:abstractionDefinition element.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITION_H
#define ABSTRACTIONDEFINITION_H

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/ipxactmodels_global.h>
 
#include <QSharedPointer>

class PortAbstraction;
class Choice;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:abstractionDefinition element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractionDefinition: public Document
{
public:

    AbstractionDefinition(VLNV const& vlnv, Document::Revision revision);

	//! Copy constructor.
	AbstractionDefinition(AbstractionDefinition const& other);

	//! Assignment operator.
	AbstractionDefinition &operator=(AbstractionDefinition const& other);

	//! The destructor.
	virtual ~AbstractionDefinition();

	/*!
	 *  Creates a perfect copy of the Bus Definition.
	 *
	 *    @return Copy of the bus definition.
	 */
	virtual QSharedPointer<Document> clone() const;

	/*! Set the VLNV for the bus definition.
	*
	*    @param [in] vlnv     The vlnv to set
	*/
	void setVlnv(const VLNV& vlnv) final;

	/*! Get the vlnv tag of the bus definition that this abstraction definition details.
	 * 
	 *    @return The VLNV this abstraction definition details.
	 */
	VLNV getBusType() const;

	/*! Set the bus definition detailed in this abstraction definition.
	 *
	 *    @param [in] targetBus   The vlnv of the target bus definition.
	 */
	void setBusType(VLNV const& targetBus);

	/*! Get the vlnv tag of the bus definition that this definition extends.
	 * 
	 *    @return The VLNV this bus extends or empty VLNV if this bus does not extend another.
	 */
	VLNV getExtends() const;

	/*! Set the bus to extend another.
	 *
	 *    @param [in] extends  The vlnv of the extended bus definition.
	 */
	void setExtends(VLNV const& extends);
    
	/*! Get the VLNVs that this busDefinition depends on.
	 *
	 *    @return The dependency VLNVs.
	 */
	virtual QList<VLNV> getDependentVLNVs() const;

	/*! Get the file dependencies of this busDefinition.
     *
	 *    @return The dependency files.
     *
     *      @remark This function never returns anything because busDefinition only has VLNV dependencies.
	 */
	virtual QStringList getDependentFiles() const;

    /*!
     *  Checks if the abstraction definition has a definition for a port in a given mode.
     *
     *    @param [in] portName    The name of the port to check.
     *    @param [in] mode        The mode to check for. Is undefined by default, matching any port with a wire.
     *
     *    @return True, if the port definition is found, otherwise false.
     */
    bool hasPort(QString const& portName, General::InterfaceMode mode=General::INTERFACE_MODE_COUNT) const;

    /*!
     *  Gets the names of the ports defined in the abstraction definition.
     *
     *    @param [in] mode   The interface mode to find ports for.
     *
     *    @return The names of the ports in the given mode.
     */
    QStringList getPortNames(General::InterfaceMode mode) const;
        
    /*!
     *  Gets the direction for a port in a given mode.
     *
     *    @param [in] portName            The name of the port to get the direction for.
     *    @param [in] mode                The mode to get the direction for.
     *    @param [in] newSystemGroup      Used system group of the new bus interface.
     *
     *    @return The direction for the port in the given mode.
     */
    DirectionTypes::Direction getPortDirection(QString const& portName, General::InterfaceMode mode,
        QString const& newSystemGroup) const;

    /*!
     *  Gets the initiative for a port in the selected interface mode.
     *
     *    @param [in] portName        Name of the selected port.
     *    @param [in] mode            Interface mode for the selected port.
     *    @param [in] systemGroup     The used system group.
     *
     *    @return Initiative for the port in the given mode.
     */
    QString getPortInitiative(QString const& portName, General::InterfaceMode mode, QString const& systemGroup)
        const;

    /*!
     *  Gets a logical port with the given name.
     *
     *    @param [in] portName   The name of the port to get.
     *
     *    @return The port with the given name.
     */
    QSharedPointer<PortAbstraction> getPort(QString const& portName) const;

    /*!
     *  Gets the logical ports in the abstraction description.
     *
     *    @return The logical ports.
     */
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > getLogicalPorts() const;

    QList<QSharedPointer<PortAbstraction> > getPortsWithWire() const;

    QList<QSharedPointer<PortAbstraction> > getPortsWithTransactional() const;

    /*!
     *	Sets the choices of the abstraction definition.
     *  
     *    @param [in] choices	Description
     */
    void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices);

    /*!
     *  Gets the choices of the abstraction description.
     *
     *    @return The choices.
     */
    QSharedPointer<QList<QSharedPointer<Choice> > > getChoices() const;

private:

    //! Specifies the bus definition this abstraction definition details.
    VLNV busType_;

	//! Specifies if this definition is an extension from another abstraction definition.
	VLNV extends_;

    //! The logical ports defined in the abstraction definition.
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > logicalPorts_ =
        QSharedPointer<QList<QSharedPointer<PortAbstraction > > >(new QList<QSharedPointer<PortAbstraction> >);

    //! The choices of the abstraction definition.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_ =
        QSharedPointer<QList<QSharedPointer<Choice> > >(new QList<QSharedPointer<Choice> >);
};

#endif // ABSTRACTIONDEFINITION_H
