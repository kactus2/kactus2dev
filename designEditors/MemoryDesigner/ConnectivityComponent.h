//-----------------------------------------------------------------------------
// File: ConnectivityComponent.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// A component instance in a connectivity graph.
//-----------------------------------------------------------------------------

#ifndef INSTANCE_H
#define INSTANCE_H

#include <QSharedPointer>
#include <QString>
#include <QVector>

class MemoryItem;

//-----------------------------------------------------------------------------
//! A component instance in a connectivity graph.
//-----------------------------------------------------------------------------
class ConnectivityComponent 
{
public:

	//! The constructor.
	ConnectivityComponent(QString const& name);

	//! The destructor.
	~ConnectivityComponent();
        
    /*!
     *  Gets the name of the instance.
     *
     *      @return The instance name.
     */
    QString getName() const;

    /*!
     *  Sets the identifier for the instance.
     *
     *      @param [in] uuid   The identifier to set.
     */
    void setInstanceUuid(QString const& uuid);

    /*!
     *  Gets the identifier for the component instance.
     *
     *      @return The identifier for the instance.
     */
    QString getInstanceUuid() const;

    /*!
     *  Sets the instance VLNV identifier.
     *
     *      @param [in] vlnv   The VLNV to set.
     */
    void setVlnv(QString const& vlnv);

    /*!
     *  Gets the instance VLNV identifier.
     *
     *      @return The instance VLNV identifier.
     */
    QString getVlnv() const;

    /*!
     *  Sets the active view for the instance.
     *
     *      @param [in] viewName   The name of the active view to set.
     */
    void setActiveView(QString const& viewName);

    /*!
     *  Adds a memory associated with the instance.
     *
     *      @param [in] memory      The memory to add.
     */
    void addMemory(QSharedPointer<MemoryItem> memory);

    /*!
     *  Gets the associated memories for the instance.
     *
     *      @return The associated memories.
     */
    QVector<QSharedPointer<MemoryItem> > getMemories() const;

private:

	// Disable copying.
	ConnectivityComponent(ConnectivityComponent const& rhs);
	ConnectivityComponent& operator=(ConnectivityComponent const& rhs);

    //! The name of the instance.
    QString name_;

    //! The VLNV of the instance.
    QString vlnv_;

    //! The unique identifier of the instance.
    QString uuid_;

    //! The name of the active view of the instance.
    QString activeView_;

    //! Memories associated with the instance.
    QVector<QSharedPointer<MemoryItem> > memories_;
};

#endif // INSTANCE_H
