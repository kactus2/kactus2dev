//-----------------------------------------------------------------------------
// File: RemapStatesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.04.2015
//
// Description:
// Abstract memory-map contains common memory-map elements.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTMEMORYMAP_H_
#define ABSTRACTMEMORYMAP_H_

#include "memorymapitem.h"
#include "generaldeclarations.h"
#include "ipxactmodels_global.h"

#include <IPXACTmodels/NameGroup.h>

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>

class Choice;

//-----------------------------------------------------------------------------
//! Abstract memory-map contains common memory-map elements.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractMemoryMap
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] abstractMemoryMapNode   A reference to the QDomNode to parse the information from.
     */
    AbstractMemoryMap(QDomNode &abstractMemoryMapNode);

    /*!
     *  The default constructor.
     */
    AbstractMemoryMap();

    /*!
     *  Copy constructor.
     *
     *      @param [in] other   The abstract memory map to be copied.
     */
    AbstractMemoryMap(const AbstractMemoryMap &other);

    /*!
     *  Assignment operator.
     *
     *      @param [in] other   The abstract memory map to be assigned.
     */
    AbstractMemoryMap &operator=(const AbstractMemoryMap &other);

    /*!
     *  The destructor.
     */
    virtual ~AbstractMemoryMap();

    /*!
     *  Writes the abstract memory map information to xml.
     *
     *      @param [in] writer  A reference to a QXMLStreamWriter instance used to write the information.
     */
    void write(QXmlStreamWriter& writer);

    /*!
     *  Check if the memory map is in a valid state.
     *
     *      @param [in] componentChoices    Choices in the containing component.
     *      @param [in] errorList           The list to add the possble error messages to.
     *      @param [in] parentIdentifier    String from parent to help to identify the location of the error.
     *
     *      @return True, if the state is valid and writing is possible, false otherwise.
     */
    bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList& errorList,
        const QString& parentIdentifier) const;

    /*!
     *  Check if the memory map is in a valid state.
     *
     *      @param [in] componentChoices    Choices in the containing component.
     *
     *      @return True if the state is valid and writing is possible, false otherwise.
     */
    bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

    /*!
     *  Check if the address blocks within memory map contains registers with given names.
     *
     *      @param [in] regNames    List of register names to be checked.
     *
     *      @return True, if all register names are unique.
     */
    bool uniqueRegisterNames(QStringList& regNames) const;

    /*!
     *  Check if the names of memory typed address blocks are contained in the given list.
     *
     *      @param [in] memNames    List of memory names.
     *
     *      @return True, if all memory names are unique, false otherwise.
     */
    bool uniqueMemoryNames(QStringList& memNames) const;

    /*!
     *  Check if the memory map contains at sub items.
     *
     *      @return True, if the memory map contains sub items, false otherwise.
     */
    bool containsSubItems() const;

    /*!
     *  Returns the last address of the memory map.
     *
     *      @return A quint64 form of the last address.
     */
    quint64 getLastAddress() const;

    /*!
     *  Get the last address of the memory map.
     *
     *      @return The last address in a qstring form.
     */
    QString getLastAddressStr() const;

    /*!
     *  Get the first address of the memory map.
     *
     *      @return The first address of the memory map in qstring form.
     */
    QString getFirstAddressStr() const;

    /*!
     *  Get a list of memory map items.
     *
     *      @return A list containing pointers to the memory map items stored in this instance.
     */
    const QList<QSharedPointer<MemoryMapItem> >& getItems() const;

    /*!
     *  Get a list of memory map items.
     *
     *      @return A list containing pointers to the memory map items stored in this instance.
     */
    QList<QSharedPointer<MemoryMapItem> >& getItems();

    /*!
     *  Set the memory map elements for this memory map.
     *
     *      @param [in] newItems    A List containing pointers to the items to be stored in this memory map.
     */
    void setItems(const QList<QSharedPointer<MemoryMapItem> > &newItems);

    /*!
     *  Get the name of the memory map.
     *
     *      @return The name of the memory map.
     */
    QString getName() const;

    /*!
     *  Set the name of the memory map.
     *
     *      @param [in] name    The new name for the memory map.
     */
    void setName(const QString &name);

    /*!
     *  Get the display name of the memory map.
     *
     *      @return The display name of the memory map.
     */
    QString getDisplayName() const;

    /*!
     *  Set the display name for the memory map.
     *
     *      @param [in] displayName     The new display name.
     */
    void setDisplayName(const QString& displayName);

    /*!
     *  Get the description of the memory map.
     *
     *      @return The description of the memory map.
     */
    QString getDescription() const;

    /*!
     *  Set the description for the memory map.
     *
     *      @param [in] description     The new description.
     */
    void setDescription(const QString& description);

    /*!
     *  Check if the memory map contains any sub items.
     *
     *      @return True, if the memory map contains sub items.
     */
    bool isEmpty() const;

    /*!
     *  Get reference to the name group-struct of the memory map.
     *
     *      @return Reference to the name group;
     */
    NameGroup& getNameGroup();

private:

    //! Contains the name, display name and description of the memory map.
    NameGroup nameGroup_;

    /*!
     * OPTIONAL (spirit:id)
     * Assigns a unique identifier for the containing element.
     */
    QString id_;

    /*!
     * OPTIONAL
     * Contains pointers to the memory map elements
     */
    QList<QSharedPointer<MemoryMapItem> > items_;

};

#endif // ABSTRACTMEMORYMAP_H_
