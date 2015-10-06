//-----------------------------------------------------------------------------
// File: ComponentGeneratorReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Reader class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#ifndef ComponentGeneratorReader_H
#define ComponentGeneratorReader_H

#include "ComponentGenerator.h"

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentGeneratorReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ComponentGeneratorReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ComponentGeneratorReader();

    /*!
     *  Creates a new ComponentGenerator from a given ComponentGenerator node.
     *
     *      @param [in] ComponentGeneratorNode    XML description of the ComponentGenerator.
     */
    QSharedPointer<ComponentGenerator> createComponentGeneratorFrom(QDomNode const& componentGeneratorNode) const;

private:

    //! No copying allowed.
    ComponentGeneratorReader(ComponentGeneratorReader const& rhs);
	ComponentGeneratorReader& operator=(ComponentGeneratorReader const& rhs);

	/*!
	 *  Extracts attributes from parameter rcomponentGeneratorNode.
	 *
	 *      @param [in] componentGeneratorNode		The XML description.
	 *      @param [in] newComponentGenerator		The created component generator.
	 */
	void readAttributes(QDomNode const &componentGeneratorNode,
		QSharedPointer<ComponentGenerator> newComponentGenerator) const;

	/*!
	 *  Extracts parameters from parameter tempNode.
	 *
	 *      @param [in] tempNode					The XML description.
	 *      @param [in] parameters					The created component generator.
	 */
	void readParameters(QDomNode &tempNode, QList<QSharedPointer<Parameter > > &parameters) const;

	/*!
	 *  Extracts API type from parameter tempNode.
	 *
	 *      @param [in] tempNode					The XML description.
	 *      @param [in] newComponentGenerator		The created component generator.
	 */
	void readApiType(QDomNode &tempNode, QSharedPointer<ComponentGenerator> newComponentGenerator) const;
};

#endif // ComponentGeneratorReader_H