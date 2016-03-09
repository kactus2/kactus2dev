//-----------------------------------------------------------------------------
// File: DesignConfigurationWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.08.2015
//
// Description:
// Writer class for IP-XACT Design configuration element.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONWRITER_H
#define DESIGNCONFIGURATIONWRITER_H

#include "DesignConfiguration.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/DocumentWriter.h>

#include <QObject>
#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Design configuration element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignConfigurationWriter : public DocumentWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this constructor.
     */
    DesignConfigurationWriter(QObject* parent = 0);

    //! The destructor.
    ~DesignConfigurationWriter();

    /*!
     *  Writes the given design configuration into xml.
     *
     *      @param [in] writer                  The xml writer to use.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeDesignConfiguration(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

private:

    //! No copying allowed.
    DesignConfigurationWriter(DesignConfigurationWriter const& rhs);
    DesignConfigurationWriter& operator=(DesignConfigurationWriter const& rhs);

    /*!
     *  Write the document start elements.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeDesignConfigurationStart(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const; 

    /*!
     *  Write the VLNV of the design configuration's referenced design.
     *
     *      @param [in] writer              The used xml writer.
     *      @param [in] designReference     The VLNV of the referenced design.
     */
    void writeDesignReference(QXmlStreamWriter& writer, VLNV const& designReference) const;

    /*!
     *  Write generator chain configuration.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeGeneratorChainConfiguration(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the interconnection configurations.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeInterConnectionConfiguration(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

    /*!
     *  Write the multiple abstractor instances.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] multipleAbstractors     A list of multipleAbastractors.
     */
    void writeMultipleAbstractorInstances(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<MultipleAbstractorInstances> > > multipleAbstractors) const;

    /*!
     *  Write the interface references.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] interfaceReferences     A list of interface references.
     */
    void writeInterfaceReferences(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<InterfaceRef> > > interfaceReferences) const;

    /*!
     *  Write the abstractor instances.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] abstractorInstances     A list of abstractor instances.
     */
    void writeAbstractorInstances(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<AbstractorInstance> > > abstractorInstances) const;

    /*!
     *  Write the view configurations.
     *
     *      @param [in] writer                  The used xml writer.
     *      @param [in] designConfiguration     The selected design configuration.
     */
    void writeViewConfigurations(QXmlStreamWriter& writer,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

};

#endif // DESIGNCONFIGURATIONWRITER_H
