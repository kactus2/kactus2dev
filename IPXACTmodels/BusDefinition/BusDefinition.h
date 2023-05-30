//-----------------------------------------------------------------------------
// File: BusDefinition.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Implementation for ipxact:busDefinition element.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITION_H
#define BUSDEFINITION_H


#include <IPXACTmodels/common/BooleanValue.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Component/Choice.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <IPXACTmodels/ipxactmodels_global.h>
 
#include <QDomDocument>
#include <QSharedPointer>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QMap>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:busDefinition element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusDefinition: public Document
{
public:

	//! The default constructor.
	BusDefinition();

	BusDefinition(VLNV const& vlnv, Document::Revision revision);

	//! Copy constructor.
	BusDefinition(BusDefinition const& other);

	//! Assignment operator.
	BusDefinition &operator=(BusDefinition const& other);

	//! The destructor.
	virtual ~BusDefinition();

	/*!
	 *  Creates a perfect copy of the Bus Definition.
	 *
	 *      @return Copy of the bus definition.
	 */
	virtual QSharedPointer<Document> clone() const;

	/*! Set the VLNV for the bus definition.
	*
	*      @param [in] vlnv     The vlnv to set
	*/
	void setVlnv(const VLNV& vlnv) final;

	/*! Sets the bus to support/not support direct master-slave connections.
	 *
	 *      @param [in] directConnection The wanted setting.
	 */
	void setDirectConnection(bool directConnection);

	/*! Check if the bus supports direct master-slave connections.
	 *
	 *      @return True, if the bus connects directly from master to slave, otherwise false.
	 */
	bool getDirectConnection() const;
   
    /*! 
     *  Sets the bus to support/not support broadcast mode.
	 *
	 *      @param [in] broadcast The wanted setting.
	 */
    void setBroadcast(bool broadcast);
        
    /*!
     *  Checks if the bus supports broadcast mode.
     *
     *      @return True, if broadcast is supported, otherwise false.
     */
    BooleanValue getBroadcast() const;

	/*! Set the whether this bus uses addressing.
	 *
	 *      @param [in] isAddressable The value to set.
	 */
	void setIsAddressable(bool isAddressable);

	/*! Check if the bus uses addressing.
	 *
	 *      @return True, if the bus is addressable, otherwise false.
	 */
	bool getIsAddressable() const;

	/*! Get the vlnv tag of the bus definition that this definition extends.
	 * 
	 *      @return The VLNV this bus extends or empty VLNV if this bus does not extend another.
	 */
	VLNV getExtends() const;

	/*! Set the bus to extend another.
	 *
	 *      @param [in] extends  The vlnv of the extended bus definition.
	 */
	void setExtends(VLNV const& extends);
        
	/*! Set the maximum number of masters on this bus
	 *
	 *      @param [in] maxMasters The maximum number of masters
	 */
    void setMaxMasters(std::string const& maxMasters);

    /*! Get the maximum number of masters on this bus
	 *
	 *      @return unsigned int containing maximum number of masters
	 */
	std::string getMaxMasters() const;

	/*! Set the maximum number of slaves on this bus
	 *
	 *      @param [in] The maximum number of slaves
	 */
	void setMaxSlaves(std::string const& maxSlaves);

	/*! Get the maximum number of slaves on this bus
	 *
	 *      @return The maximum number of slaves
	 */
	std::string getMaxSlaves() const;

	/*! Set the maximum number of initiators on this bus
	 *
	 *      @param [in] maxInitiators The maximum number of initiators
	 */
    void setMaxInitiators(std::string const& maxInitiators);

    /*! Get the maximum number of initiators on this bus
	 *
	 *      @return unsigned int containing maximum number of initiators
	 */
	std::string getMaxInitiators() const;

	/*! Set the maximum number of targets on this bus
	 *
	 *      @param [in] The maximum number of targets
	 */
	void setMaxTargets(std::string const& maxTargets);

	/*! Get the maximum number of targets on this bus
	 *
	 *      @return The maximum number of targets
	 */
	std::string getMaxTargets() const;

	/*! Set the systemGroupNames.
	 *
	 *      @param [in] systemGroupNames The names of the systemGroups available in onSystem mode.
	 */
	void setSystemGroupNames(QStringList const& systemGroupNames);

	/*! Gets the systemGroupNames.
	 *
	 *      @return The names of available systemGroups
	 */
	QStringList getSystemGroupNames() const;

	/*! Sets the choices.
	 *
	 *      @param [in] choices The choices to set.
	 */
	void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices);
	
	/*! Gets the choices.
	 *
	 *      @return A list of the busDefinition choices
	 */
	QSharedPointer<QList<QSharedPointer<Choice> > > getChoices() const;

	/*! Get the VLNVs that this busDefinition depends on.
	 *
	 *      @return The dependency VLNVs.
	 */
	virtual QList<VLNV> getDependentVLNVs() const;

	/*! Get the file dependencies of this busDefinition.
     *
	 *      @return The dependency files.
     *
     *      @remark This function never returns anything because busDefinition only has VLNV dependencies.
	 */
	virtual QStringList getDependentFiles() const;

private:

	//! Direct connection specifies if direct master-slave connections are allowed.
	bool directConnection_ = true;

    //! Specifies if the bus supports broadcast mode.
    BooleanValue broadcast_;

	//! Specifies if the bus has addressing information.
	bool isAddressable_ = true;

	//! Maximum number of masters allowed on this bus.
	std::string maxMasters_;

	//! Maximum number of slaves allowed on this bus.
	std::string maxSlaves_;

	//! The choices of the bus definition
	QSharedPointer<QList<QSharedPointer<Choice> > > choices_ = 
		QSharedPointer<QList<QSharedPointer<Choice> > >(new QList<QSharedPointer<Choice > >);

	//! List of systemGroupName elements.
	QStringList systemGroupNames_;

	//! specifies if this definition is an extension from another bus.
	VLNV extends_;

};

#endif /* BUSDEFINITION_H */
