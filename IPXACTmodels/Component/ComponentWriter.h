//-----------------------------------------------------------------------------
// File: ComponentWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.10.2015
//
// Description:
// Writer class for ipxact:component element.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWRITER_H
#define COMPONENTWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/DocumentWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! Writer class for ipxact:component element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentWriter : public DocumentWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    ComponentWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ComponentWriter();

    /*!
     *  Write a component to an XML file.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component to be written.
     */
    void writeComponent(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

private:

    //! No copying allowed.
    ComponentWriter(ComponentWriter const& rhs);
    ComponentWriter& operator=(ComponentWriter const& rhs);

    /*!
     *  Write the bus interfaces.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeBusInterfaces(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the channels.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeChannels(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the remap states.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeRemapStates(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the address spaces.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeAddressSpaces(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the memory maps.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeMemoryMaps(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the model.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeModel(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the views.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeViews(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the instantiations.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeInstantiations(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the ports.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writePorts(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the component generators.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeComponentGenerators(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the choices.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeChoices(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the file sets.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeFileSets(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the cpus.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeCPUs(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the other clock drivers.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeOtherClockDrivers(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;

    /*!
     *  Write the other component extensions.
     *
     *      @param [in] writer      The used XML writer.
     *      @param [in] component   The component being written.
     */
    void writeComponentExtensions(QXmlStreamWriter& writer, QSharedPointer<Component> component) const;
};

#endif // COMPONENTWRITER_H