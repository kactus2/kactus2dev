//-----------------------------------------------------------------------------
// File: MemoryRemap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAP_H
#define MEMORYREMAP_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>

#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QTextStream>

class ModeReference;

//-----------------------------------------------------------------------------
//! Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryRemap : public MemoryMapBase
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] name        Name of the memory remap.
     *    @param [in] remapState  The remap state refered by the memory remap.
     */
    MemoryRemap(QString const& name = QString(), QString const& remapState = QString());

    /*!
     *  Copy constructor.
     *
     *    @param [in] other   The memory remap being copied.
     */
    MemoryRemap(const MemoryRemap& other);

    /*!
     *  Assignment operator.
     *
     *    @param [in] other   The memory remap being assigned.
     */
    MemoryRemap& operator=(const MemoryRemap& other);

    /*!
     *  The destructor.
     */
    ~MemoryRemap();

    /*!
     *  Get the element name of the memory remap.
     *
     *    @return QString containing "Memory remap".
     */
    virtual QString elementName() const;

    /*!
     *  Get the remap state of the memory remap.
     *
     *    @return The name of the remap state of the memory remap.
     */
    QString getRemapState() const;

    /*!
     *  Set the remap state of the memory remap.
     *
     *    @param [in] newRemapState   The name of the new remap state of the memory remap.
     */
    void setRemapState(QString newRemapState);

    /*!
     *	Get the memory remap definition reference.
     *  
     * 	    @return The memory remap definition reference.
     */
    QString getMemoryRemapDefinitionReference() const;

    /*!
     *	Set the memory remap definition reference.
     *  
     *    @param [in] newDefinitionRef     The new definition reference.
     */
    void setMemoryRemapDefinitionReference(QString const& newDefinitionRef);

    /*!
     *	Get the type definitions reference of this memory remap.
     *  
     *	    
     * 	    @return The type definitions reference.
     */
    QString getTypeDefinitionsReference() const;

    /*!
     *	Set the type definitions reference for this memory remap
     *  
     *    @param [in] newTypeDefinitionsRef     The type definitions reference to set.
     */
    void setTypeDefinitionsReference(QString const& newTypeDefinitionsRef);

    /*!
     *	Get the remap mode references.
     *  
     * 	    @return The mode references of the memory remap.
     */
    QSharedPointer<QList<QSharedPointer<ModeReference> > > getModeReferences() const;

    /*!
     *	Set the mode references of the remap.
     *  
     *    @param [in] newModeRefs     The mode references to set.
     */
    void setModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > newModeRefs);

private:

    //! The remap state of this memory remap.
    QString remapState_;

    //! The referenced memory remap type definition.
    QString memoryRemapDefinitionReference_;

    //! The referenced type definitions.
    QString typeDefinitionsReference_;

    //! The mode references of the memory remap.
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeReferences_ =
        QSharedPointer<QList<QSharedPointer<ModeReference> > >(new QList<QSharedPointer<ModeReference> >());

};

#endif // MEMORYREMAP_H_
