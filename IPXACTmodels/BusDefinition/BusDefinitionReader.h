//-----------------------------------------------------------------------------
// File: BusDefinitionReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2015
//
// Description:
// XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONREADER_H
#define BUSDEFINITIONREADER_H

#include <QDomNode>
#include <QSharedPointer>

#include <IPXACTmodels/common/DocumentReader.h>
#include <IPXACTmodels/ipxactmodels_global.h>

class BusDefinition;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusDefinitionReader : public DocumentReader
{
public:

	//! The constructor.
	BusDefinitionReader();

	//! The destructor.
	~BusDefinitionReader();

    //! Disable copying.
    BusDefinitionReader(BusDefinitionReader const& rhs) = delete;
    BusDefinitionReader& operator=(BusDefinitionReader const& rhs) = delete;

    /*!
     *  Creates a bus definition from XML description.
     *
     *      @param [in] document   The XML document to create the bus definition from.
     *
     *      @return The created bus definition.
     */
    QSharedPointer<BusDefinition> createBusDefinitionFrom(QDomNode const& document) const;
 
private:

    /*!
     *  Reads the direct connection property from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the direct connection property into.
     */
    void parseDirectConnection(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                
    /*!
     *  Reads the broadcast property from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the broadcast property into.
     */
    void parseBroadcast(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the isAddressable property from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the isAddressable property into.
     */
    void parseIsAddressable(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the extends property from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the extends property into.
     */
    void parseExtends(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the maximum number of initiators from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the maximum number of initiators into.
     */
    void parseMaximumInitiators(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the maximum number of targets from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the maximum number of targets into.
     */
    void parseMaximumTargets(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the system group names from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the system group names into.
     */
    void parseSystemGroupNames(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;

    /*!
     *  Reads the choices from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the choices into.
     */
    void parseChoices(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
};

#endif // BUSDEFINITIONREADER_H