//-----------------------------------------------------------------------------
// File: MemoryDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.9.2012
//
// Description:
// Declares the memory design diagram class.
//-----------------------------------------------------------------------------

#ifndef MEMORYDESIGNDIAGRAM_H
#define MEMORYDESIGNDIAGRAM_H

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/IDFactory.h>
#include <common/KactusAttribute.h>
#include <common/DesignDiagram.h>

#include <QVector>

class MainWindow;
class Component;
class Design;
class LibraryInterface;
class VLNV;
class GenericEditProvider;
class MemoryDesignWidget;

//-----------------------------------------------------------------------------
//! MemoryDesignDiagram class.
//-----------------------------------------------------------------------------
class MemoryDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    // Column width.
    enum
    {
        COLUMN_WIDTH = 339,
    };

    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] mainWnd       The main window.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */    
    MemoryDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                        GenericEditProvider& editProvider, MemoryDesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~MemoryDesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent();

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] desc The column description.
     */
    void addColumn(ColumnDesc const& desc);

    /*!
     *  Returns the parent widget.
     */
    MemoryDesignWidget* parent() const;

    /*!
     *  Returns the column layout.
     */
    GraphicsColumnLayout* getColumnLayout();

    void onVerticalScroll(qreal y);

public slots:
    /*!
     *  Called when the selection changes in the diagram.
     */
    void onSelectionChanged();

protected:
    //! Called when the user presses a mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release a mouse button.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the mouse is double-clicked.
    //void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Called when the mouse wheel has scrolled.
    void wheelEvent(QGraphicsSceneWheelEvent *event);

private:
    // Disable copying.
    MemoryDesignDiagram(MemoryDesignDiagram const& rhs);
    MemoryDesignDiagram& operator=(MemoryDesignDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Opens the system design for editing.
     */
    void loadDesign(QSharedPointer<Design> design);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent widget.
    MemoryDesignWidget* parent_;

    // Graphics column layout.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! The old item selection.
    QGraphicsItem* oldSelection_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYDESIGNDIAGRAM_H
