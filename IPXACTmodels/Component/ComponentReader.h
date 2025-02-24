//-----------------------------------------------------------------------------
// File: ComponentReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.10.2015
//
// Description:
// Reader class for ipxact:component element.
//-----------------------------------------------------------------------------

#ifndef COMPONENTREADER_H
#define COMPONENTREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/DocumentReader.h>

#include <QSharedPointer>
#include <QDomNode>

class Component;
class Model;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:component element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComponentReader : public DocumentReader
{
public:

    //! The constructor.
    ComponentReader();

    /*!
     *  The destructor.
     */
    ~ComponentReader() = default;

    /*!
     *  Creates a new component from a given component document.
     *
     *    @param [in] addressBlockNode    XML description of the component.
     *
     *    @return The created component.
     */
    QSharedPointer<Component> createComponentFrom(QDomDocument const& componentDocument) const;

private:

    //! No copying allowed.
    ComponentReader(ComponentReader const& rhs);
    ComponentReader& operator=(ComponentReader const& rhs);

    void parsePowerDomains(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the bus interfaces.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseBusInterfaces(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;
  
    /*!
     *  Reads the indirect interfaces.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseIndirectInterfaces(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the channels.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseChannels(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the remap states.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseRemapStates(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    void parseModes(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the address spaces.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseAddressSpaces(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the memory maps.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseMemoryMaps(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the model.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseModel(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the views.
     *
     *    @param [in] modelElement    XML description of the model.
     *    @param [in] newModel        The new model item.
     *    @param [in] docRevision     The IP-XACT standard revision to comply to.
     */
    void parseViews(QDomElement const& modelElement, QSharedPointer<Model> newModel,
        Document::Revision docRevision) const;

    /*!
     *  Reads the instantiations.
     *
     *    @param [in] modelElement    XML description of the model.
     *    @param [in] newModel        The new model item.
     */
    void parseInstantiations(QDomElement const& modelElement, QSharedPointer<Model> newModel,
        Document::Revision docRevision) const;

    /*!
     *  Reads the component instantiations.
     *
     *    @param [in] instantiationsElement   XML description of the instantiations element.
     *    @param [in] newModel                The new model item.
     *    @param [in] docRevision             The IP-XACT standard revision to comply to.
     */
    void parseComponentInstantiations(QDomElement const& instantiationsElement, QSharedPointer<Model> newModel,
        Document::Revision docRevision) const;

    /*!
     *  Reads the design instantiations.
     *
     *    @param [in] instantiationsElement   XML description of the instantiations element.
     *    @param [in] newModel                The new model item.
     */
    void parseDesignInstantiations(QDomElement const& instantiationsElement, QSharedPointer<Model> newModel) const;

    /*!
     *  Reads the design configuration instantiations.
     *
     *    @param [in] instantiationsElement   XML description of the instantiations element.
     *    @param [in] newModel                The new model item.
     *    @param [in] docRevision             The IP-XACT standard revision to comply to.
     */
    void parseDesignConfigurationInstantiations(QDomElement const& instantiationsElement,
        QSharedPointer<Model> newModel, Document::Revision docRevision) const;

    /*!
     *  Reads the ports.
     *
     *    @param [in] modelElement    XML description of the model.
     *    @param [in] newModel        The new model item.
     */
    void parsePorts(QDomElement const& modelElement, QSharedPointer<Model> newModel, Document::Revision docRevisio) const;

    /*!
     *  Reads the component generators.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseComponentGenerators(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the choices.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseChoices(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the file sets.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseFileSets(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the cpus.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseCPUs(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the other clock drivers.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseOtherClockDrivers(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the reset types.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseResetTypes(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the component extensions.
     *
     *    @param [in] componentNode   XML description of the component.
     *    @param [in] newComponent    The new component.
     */
    void parseComponentExtensions(QDomNode const& componentNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the swView extensions.
     *
     *    @param [in] swViewsNode     XML description of the sw views.
     *    @param [in] newComponent    The new component.
     */
    void parseSwViews(QDomNode const& swViewsNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the sw property extensions.
     *
     *    @param [in] propertiesNode  XML description of the sw properties.
     *    @param [in] newComponent    The new component.
     */
    void parseSwProperties(QDomNode const& propertiesNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the system view extensions.
     *
     *    @param [in] viewsNode       XML description of the system views.
     *    @param [in] newComponent    The new component.
     */
    void parseSystemViews(QDomNode const& viewsNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the com interface extensions.
     *
     *    @param [in] interfaceNode   XML description of the com interfaces.
     *    @param [in] newComponent    The new component.
     */
    void parseComInterfaces(QDomNode const& interfaceNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the api interface extensions.
     *
     *    @param [in] interfaceNode   XML description of the api interfaces.
     *    @param [in] newComponent    The new component.
     */
    void parseApiInterfaces(QDomNode const& interfaceNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reades the source directories for file sets.
     *
     *    @param [in] singleExtensionNode     XML description of the file source directories.
     *    @param [in] newComponent            The new component.
     */
    void parseFilesetSourceDirectories(QDomNode const& singleExtensionNode, 
        QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the file dependency extensions.
     *
     *    @param [in] fileNode        XML description of the file dependencies.
     *    @param [in] newComponent    The new component.
     */
    void parseFileDependencies(QDomNode const& fileNode, QSharedPointer<Component> newComponent) const;

    /*!
     *  Reads the author vendor extension.
     *
     *    @param [in] authorNode      XML description of the author.
     *    @param [in] newComponent    The new component.
     */
    void parseAuthor(QDomNode const& authorNode, QSharedPointer<Component> newComponent) const;
};

#endif // COMPONENTREADER_H