//-----------------------------------------------------------------------------
// File: MemoryDesignerGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.08.2016
//
// Description:
// Parent class for memory designer graphics items.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNERGRAPHICSITEM_H
#define MEMORYDESIGNERGRAPHICSITEM_H

#include <IPXACTmodels/common/VLNV.h>

#include <editors/common/diagramgrid.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>
#include <QGraphicsTextItem>

#include <QObject>
#include <QAction>

class MemoryColumn;
class MemoryConnectionItem;
class MemoryCollisionItem;
class ConnectivityComponent;
class MemoryItem;

//-----------------------------------------------------------------------------
//! Parent class for memory designer graphics items.
//-----------------------------------------------------------------------------
class MemoryDesignerGraphicsItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] itemName            Name of the memory graphics item.
     *      @param [in] displayName         Display name of the memory graphics item.
     *      @param [in] identifierChain     List of string containing the identifying information of the item.
     *      @param [in] componentInstance   Component instance containing the memory item.
     *      @param [in] parent              The parent item.
     */
    MemoryDesignerGraphicsItem(QString const& itemName, QString const& displayName,
        QVector<QString> identifierChain, QSharedPointer<ConnectivityComponent const> componentInstance,
        QGraphicsItem* parent = 0);

    // Disable copying.
    MemoryDesignerGraphicsItem(MemoryDesignerGraphicsItem const& rhs) = delete;
    MemoryDesignerGraphicsItem& operator=(MemoryDesignerGraphicsItem const& rhs) = delete;

	/*!
     *  The destructor.
     */
    virtual ~MemoryDesignerGraphicsItem() = default;

    /*!
     *  Get the type of the memory item.
     */
    virtual int type() const = 0;

    /*!
     *  Get the name of the memory item.
     *
     *      @return The name of the memory item.
     */
    QString name() const;

    /*!
     *  Get the display name of the memory item.
     *
     *      @return The display name of the memory item.
     */
    QString displayName() const;

    /*!
     *  Hide the memory range labels.
     */
    void hideMemoryRangeLabels();

    /*!
     *  Hide the colliding memory item range labels.
     *
     *      @param [in] connectionBaseAddress   Base address of the memory connection item.
     *      @param [in] connectionLastAddress   Last address of the memory connection item.
     */
    void hideCollidingRangeLabels(quint64 connectionBaseAddress, quint64 connectionLastAddress);

    /*!
     *  Hide the start memory range label.
     */
    void hideStartRangeLabel();

    /*!
     *  Hide the end memory range label.
     */
    void hideEndRangeLabel();

    /*!
     *  Get the current base address of the memory item.
     *
     *      @return The current base address of the memory item.
     */
    quint64 getBaseAddress() const;

    /*!
     *  Get the original base address of the memory item.
     *
     *      @return The original base address of the memory item.
     */
    quint64 getOriginalBaseAddress() const;

    /*!
     *  Get the current last address of the memory item.
     *
     *      @return The current last address of the memory item.
     */
    quint64 getLastAddress() const;

    /*!
     *  Get the original last address of the memory item.
     *
     *      @return The original last address of the memory item.
     */
    quint64 getOriginalLastAddress() const;

    /*!
     *  Compress this graphics item.
     *
     *      @param [in] newItemHeight   The new height of the graphics item.
     */
    virtual void condense(qreal newItemHeight);

    /*!
     *  Add a memory connection graphics item.
     *
     *      @param [in] connectionItem  The selected memory connection item.
     */
    virtual void addMemoryConnection(MemoryConnectionItem* connectionItem);

    /*!
     *  Get the connected memory connection items.
     *
     *      @return A map containing the memory connection items in the order of their base addresses.
     */
    QMap<quint64, MemoryConnectionItem*> getMemoryConnections() const;

    /*!
     *  Fit the selected label to this item.
     *
     *      @param [in] label   The selected label.
     */
    virtual void fitLabel(QGraphicsTextItem* label);

    /*!
     *  Get the containing component instance.
     *
     *      @return The containing component instance.
     */
    QSharedPointer<ConnectivityComponent const> getContainingInstance() const;

    /*!
     *  Change the address range of the memory item.
     *
     *      @param [in] offset  The offset of the parent item.
     */
    virtual void changeAddressRange(quint64 offset);

    /*!
     *  Compress this item to contain the selected addresses.
     *
     *      @param [in] uncutAddresses              The addresses that remain after the compression.
     *      @param [in] CUTMODIFIER                 Modifier for the cut areas.
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     */
    virtual void compressToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER,
        bool memoryItemsAreCompressed);

    /*!
     *  Compress this item to contain the selected coordinates.
     *
     *      @param [in] unCutCoordinates            Coordinates that remain after the compression.
     *      @param [in] CUTMODIFIER                 Modifier for the cut area.
     *      @param [in] memoryItemsAreCompressed    Flag for condensing memory items.
     */
    virtual void compressToUnCutCoordinates(QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER,
        bool memoryItemsAreCompressed);

    /*!
     *  Get the identifying information of the memory item.
     *
     *      @return The identifying information.
     */
    QVector<QString> getIdentifierChain() const;

    /*!
     *  Set a new identifier chain.
     *
     *      @param [in] newIdentifiers  A list of strings containing the new memory identifier chain.
     */
    virtual void setNewIdentifierChain(QVector<QString> newIdentifiers);

protected:

    /*!
     *  Setup the graphics rectangle.
     *
     *      @param [in] rectangleWidth      Width of the graphics rectangle.
     *      @param [in] rectangleHeight     Height of the graphics rectangle.
     */
    void setGraphicsRectangle(qreal rectangleWidth, qreal rectangleHeight);

    /*!
     *  Setup the memory graphics item.
     *
     *      @param [in] memoryStart     Base address of the memory item.
     *      @param [in] memoryEnd       Last address of the memory item.
     *      @param [in] identifier      Identifier of the memory item.
     */
    void setupGraphicsItem(quint64 memoryStart, quint64 memoryEnd, QString const& identifier);

    /*!
     *  Add text to the tooltip.
     *
     *      @param [in] toolTipAddition     Text to be added to the tooltip.
     */
    void addToToolTip(QString const& toolTipAddition);

    /*!
     *  Get the name label.
     *
     *      @return The label containing the item name.
     */
    QGraphicsTextItem* getNameLabel() const;

    /*!
     *  Get the range start label.
     *
     *      @return The label containing the item range start.
     */
    QGraphicsTextItem* getRangeStartLabel() const;

    /*!
     *  Get the range end label.
     *
     *      @return The label containing the item range end.
     */
    QGraphicsTextItem* getRangeEndLabel() const;

    /*!
     *  Check if the selected label collides with range labels.
     *
     *      @param [in] label       The selected label.
     *      @param [in] fontHeight  Height of the text used in the selected label.
     *
     *      @return True, if the selected label collides with range labels, otherwise false.
     */
    virtual bool labelCollidesWithRangeLabels(QGraphicsTextItem* label, qreal fontHeight) const;

    /*!
     *  Set the label positions.
     */
    virtual void setLabelPositions() = 0;

    /*!
     *  Handler for context menu.
     *
     *      @param [in] event   The context menu event.
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

signals:

    /*!
     *  Open the document from which the containing component instance has been built from. Identifying
     *  information allows the editor to be opened from the memory item.
     *
     *      @param [in] vlnv                VLNV of the component.
     *      @param [in] identifierChain     Identifying information for the memory item.
     */
    void openComponentDocument(VLNV const& vlnv, QVector<QString> identifierChain);

private slots:

    /*!
     *  Open the editor for the containing component.
     */
    void openContainingComponent();

    /*!
     *  Open the item editor from the containing component.
     */
    void openItemEditor();

private:

    /*!
     *  Get the available width of this item.
     *
     *      @return The available width of this item.
     */
    virtual qreal getItemWidth() const;
    
    /*!
     *  Setup range labels.
     *
     *      @param [in] memoryStart     The range start.
     *      @param [in] memoryEnd       The range end.
     */
    void setupLabels(quint64 memoryStart, quint64 memoryEnd);

    /*!
     *  Setup the original addresses for the memory item.
     *
     *      @param [in] memoryStart     Base address of the memory item.
     *      @param [in] memoryEnd       Last address of the memory item.
     */
    void setupOriginalAddresses(quint64 memoryStart, quint64 memoryEnd);

    /*!
     *  Setup the tooltip and the memory range labels.
     *
     *      @param [in] identifier      Selected identifier.
     */
    void setupToolTip(QString const& identifier);

    /*!
     *  Get the VLNV of the component from the containing component instance.
     *
     *      @return The VLNV of the component.
     */
    VLNV getVLNVFromContainingInstance();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! The range start label.
    QGraphicsTextItem* startRangeLabel_;

    //! The range end label.
    QGraphicsTextItem* endRangeLabel_;

    //! The base address.
    quint64 baseAddress_;

    //! The last address.
    quint64 lastAddress_;

    //! The original base address.
    quint64 originalBaseAddress_;

    //! The original last address.
    quint64 originalLastAddress_;

    //! Name of the item.
    QString itemName_;

    //! Display name of the item.
    QString displayName_;

    //! Map containing memory connection items and their base addresses.
    QMap<quint64, MemoryConnectionItem*> memoryConnections_;

    //! The containing component instance.
    QSharedPointer<ConnectivityComponent const> containingInstance_;

    //! VLNV for the containing component.
    VLNV componentVLNV_;

    //! Action for opening the component document editor.
    QAction* openComponentAction_;

    //! Action for opening the component editor in the memory item.
    QAction* openItemEditorAction_;

    //! List of string identifying the memory item.
    QVector<QString> identifierChain_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNERGRAPHICSITEM_H
