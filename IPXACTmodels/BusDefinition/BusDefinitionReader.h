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

#include <QObject>
#include <QDomNode>
#include <QSharedPointer>

#include <IPXACTmodels/ipxactmodels_global.h>

class BusDefinition;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BusDefinitionReader : public QObject
{
    Q_OBJECT
public:

	//! The constructor.
	BusDefinitionReader(QObject* parent = 0);

	//! The destructor.
	~BusDefinitionReader();

    /*!
     *  Creates a bus definition from XML description.
     *
     *      @param [in] document   The XML document to create the bus definition from.
     *
     *      @return The created bus definition.
     */
    QSharedPointer<BusDefinition> createBusDefinitionFrom(QDomNode const& document) const;
 
private:

	//! Disable copying.
	BusDefinitionReader(BusDefinitionReader const& rhs);
	BusDefinitionReader& operator=(BusDefinitionReader const& rhs);
        
    /*!
     *  Reads the top comments from XML to a bus definition.
     *
     *      @param [in]     document        The XML document to read.
     *      @param [in/out] busDefinition   The bus definition to insert the comments into.
     */
    void parseTopComments(QDomNode const& document, QSharedPointer<BusDefinition> busDefinition) const;
            
    /*!
     *  Reads the VLNV from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the VLNV into.
     */
    void parseVLNV(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                
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
     *  Reads the maximum number of masters from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the maximum number of masters into.
     */
    void parseMaximumMasters(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the maximum number of slaves from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the maximum number of slaves into.
     */
    void parseMaximumSlaves(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the system group names from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the system group names into.
     */
    void parseSystemGroupNames(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the description from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the description into.
     */
    void parseDescription(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the parameters from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the parameters into.
     */
    void parseParameters(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                    
    /*!
     *  Reads the assertions from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the assertions into.
     */
    void parseAssertions(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
                        
    /*!
     *  Reads the vendor extensions from XML to a bus definition.
     *
     *      @param [in]     busNode         The XML description of the bus definition.
     *      @param [in/out] busDefinition   The bus definition to insert the vendor extensions into.
     */
    void parseVendorExtension(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const;
};

#endif // BUSDEFINITIONREADER_H