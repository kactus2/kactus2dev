//-----------------------------------------------------------------------------
// File: ComponentGeneratorWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Writer class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------

#ifndef ComponentGeneratorWriter_H
#define ComponentGeneratorWriter_H

#include "ComponentGenerator.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT ComponentGenerator element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentGeneratorWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ComponentGeneratorWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ComponentGeneratorWriter();

    /*!
     *  Write a ComponentGenerator to an XML file.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] ComponentGenerator		The ComponentGenerator to be written.
     */
	void writeComponentGenerator(QXmlStreamWriter& writer, QSharedPointer<ComponentGenerator> componentGenerator) const;

private:

    //! No copying allowed.
    ComponentGeneratorWriter(ComponentGeneratorWriter const& rhs);
	ComponentGeneratorWriter& operator=(ComponentGeneratorWriter const& rhs);

	/*!
	 *  Writes the component generator attributes to XML.
	 *
	 *      @param [in] writer					The used xml writer.
	 *      @param [in] componentGenerator		The component generator whose attributes to write.
	 */
	void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<ComponentGenerator> componentGenerator) const;

	/*!
	 *  Writes the component generator API type to XML.
	 *
	 *      @param [in] writer					The used xml writer.
	 *      @param [in] componentGenerator		The component generator whose API type to write.
	 */
	void writeApiType(QXmlStreamWriter& writer, QSharedPointer<ComponentGenerator> componentGenerator) const;
    
	/*!
	 *  Writes the component generator transport methods to XML.
	 *
	 *      @param [in] writer					The used xml writer.
	 *      @param [in] componentGenerator		The component generator whose transport mehtods to write.
	 */
    void writeTransportMethods(QXmlStreamWriter& writer,
        QSharedPointer<ComponentGenerator> componentGenerator) const;
};

#endif // ComponentGeneratorWriter_H