//-----------------------------------------------------------------------------
// File: ConnectivityInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// An interface in a connectivity graph.
//-----------------------------------------------------------------------------

#ifndef INTERFACE_H
#define INTERFACE_H

#include <QString>
#include <QVector>
#include <QSharedPointer>

#include <IPXACTmodels/generaldeclarations.h>

class ConnectivityComponent;
class MemoryItem;

//-----------------------------------------------------------------------------
//! An interface in a connectivity graph.
//-----------------------------------------------------------------------------
class ConnectivityInterface 
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] name   The name of the interface.
	 */
	explicit ConnectivityInterface(QString const& name);

	/*!
     *  The destructor.
     */
	~ConnectivityInterface() = default;

    /*!
     *  Gets the name of the interface.
     *
     *      @return The name of the interface.
     */
    QString getName() const;

    /*!
     *  Sets the mode for the interface.
     *
     *      @param [in] newMode     The mode to set.
     */
    void setMode(General::InterfaceMode newMode);

    /*!
     *  Gets the mode of the interface.
     *
     *      @return The mode of the interface.
     */
    General::InterfaceMode getMode() const;

    /*!
     *  Sets the base address for the memories connected to the interface.
     *
     *      @param [in] address   The base address to set.
     */
    void setBaseAddress(QString const& address);

    /*!
     *  Gets the base address for the memories connected to the interface. 
     *
     *      @return The base address for the memories connected to the interface.
     */
    QString getBaseAddress() const;

    /*!
     *  Sets the remap address for the memories connected to the interface.
     *
     *      @param [in] address   The remap address to set.
     */
    void setRemapAddress(QString const& address);

    /*!
     *  Gets the remap address for the memories connected to the interface.
     *
     *      @return The remap address.
     */
    QString getRemapAddress() const;

    /*!
     *  Sets the remap range for the memories connected to the interface.
     *
     *      @param [in] range   The range to set.
     */
    void setRemapRange(QString const& range);

    /*!
     *   Gets the remap range for the memories connected to the interface.
     *
     *      @return The remap range.
     */
    QString getRemapRange() const;

    /*!
     *  Sets the instance the interface is a part of.
     *
     *      @param [in] instance   The instance to set.
     */
    void setInstance(QSharedPointer<ConnectivityComponent const> instance);

    /*!
     *  Gets the instance the interface is part of.
     *
     *      @return The instance containing the interface.
     */
    QSharedPointer<ConnectivityComponent const> getInstance() const;

    /*!
     *  Checks if the interface is associated with a memory.
     *
     *      @return True, if the interface is connected to a memory, otherwise false.
     */
    bool isConnectedToMemory() const;

    /*!
     *  Sets the associated memory for the interface.
     *
     *      @param [in] memory      The memory to associate the interface with.
     */
    void setConnectedMemory(QSharedPointer<MemoryItem> memory);

    /*!
     *  Gets the memory associated with the interface.
     *
     *      @return The associated memory if any.
     */
    QSharedPointer<MemoryItem> getConnectedMemory() const;

    //! Marks the interface as hierarchical.     
    void setHierarchical();

    /*!
     *  Checks if the interface is hierarchical.
     *
     *      @return True, if the interface is hierarchical, otherwise false.
     */
    bool isHierarchical() const;

    //! Marks the interface as bridged.     
    void setBridged();

    /*!
     *  Checks if the interface is bridged.
     *
     *      @return True, if the interface is bridged, otherwise false.
     */
    bool isBridged() const;

    /*!
     *  Get the next interface nodes for a connection tree.
     *
     *      @return List of the next interface nodes.
     */
    QVector<QSharedPointer<ConnectivityInterface> > getChildInterfaceNodes() const;

    /*!
     *  Add an interface node to the tree.
     *
     *      @param [in] newChild    The new tree interface node.
     */
    void addChildInterfaceNode(QSharedPointer<ConnectivityInterface> newChild);

    /*!
     *  Remove a node from the tree.
     *
     *      @param [in] indexOfInterface    Index of the tree node.
     */
    void removeChildInterface(int const& indexOfInterface);

    void setParent(QSharedPointer<ConnectivityInterface> parentInterface);

    [[nodiscard]] QSharedPointer<ConnectivityInterface> getParent() const;

private:

	// Disable copying.
    ConnectivityInterface(ConnectivityInterface const& rhs);
    ConnectivityInterface& operator=(ConnectivityInterface const& rhs);

    //! The name of the interface.
    QString name_;

    //! The mode of the interface.
    General::InterfaceMode mode_ = General::INTERFACE_MODE_COUNT;

    //! The base address for the associated memories. Applicable for master interfaces.
    QString baseAddress_;

    //! The remap address for the associated memories. Applicable for mirrored slave interfaces.
    QString remapAddress_;

    //! The remap range for the associated memories. Applicable for mirrored slave interfaces.
    QString range_;

    //! The instance containing the interface.
    QSharedPointer<ConnectivityComponent const> instance_;

    //! The associated memory for the interface.
    QSharedPointer<MemoryItem> memory_;

    //! Whether the interface is hierarchical or not.
    bool hierarchical_;

    //! Whether the interface is a part of a bridge or not.
    bool bridged_;

    //! List of the next interface nodes in the tree.
    QVector<QSharedPointer<ConnectivityInterface> > childInterfaceNodes_;

    QSharedPointer<ConnectivityInterface> bfsParent_{ nullptr };
};

#endif // INTERFACE_H
