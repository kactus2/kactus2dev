//-----------------------------------------------------------------------------
// File: SystemDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design diagram to work as a drawing canvas.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNDIAGRAM_H
#define SYSTEMDESIGNDIAGRAM_H

#include "SystemColumnLayout.h"

#include <common/IDFactory.h>
#include <common/KactusAttribute.h>
#include <common/DesignDiagram.h>

class MainWindow;
class Component;
class Design;
class DesignConfiguration;
class LibraryInterface;
class VLNV;
class GenericEditProvider;
class SystemDesignWidget;
class HWMappingItem;

//-----------------------------------------------------------------------------
//! SystemDesignDiagram class.
//-----------------------------------------------------------------------------
class SystemDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] mainWnd       The main window.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */
    SystemDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                        GenericEditProvider& editProvider, SystemDesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemDesignDiagram();

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] name The name of the column.
     */
    void addColumn(QString const& name);

    /*!
     * Returns the parent widget.
     */
    SystemDesignWidget* parent() const;

    /*!
     *  Returns the column layout.
     */
    SystemColumnLayout* getColumnLayout();

    void onVerticalScroll(qreal y);

public slots:
    //! Called when a component instance is added to the diagram.
    virtual void onComponentInstanceAdded(ComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    virtual void onComponentInstanceRemoved(ComponentItem* item);

signals:
    //! \brief Emitted when the user double-clicks a hierarchical component.
    void openDesign(const VLNV& vlnv, const QString& viewName);

    //! \brief Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);
     
protected:
    //! Called when the user presses a mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release a mouse button.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the mouse is double-clicked.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //! Called when the mouse wheel has scrolled.
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    //! Called when an drag enters the diagram.
    void dragEnterEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when an object is dragged within the diagram.
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when the drag leaves the diagram.
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    //! Called when an object is dropped to the diagram.
    void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    // Disable copying.
    SystemDesignDiagram(SystemDesignDiagram const& rhs);
    SystemDesignDiagram& operator=(SystemDesignDiagram const& rhs);

    void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Opens the system design for editing.
     */
    void openDesign(QSharedPointer<Design> design);

    /*!
     *  Returns the HW component instance with the given name.
     */
    HWMappingItem* getHWComponent(QString const& instanceName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent widget.
    SystemDesignWidget* parent_;

    //! The node ID factory.
    IDFactory nodeIDFactory_;

    // Node column layout.
    QSharedPointer<SystemColumnLayout> layout_;

    //! Boolean flag for indicating that an SW component is being dragged to the diagram.
    bool dragSW_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNDIAGRAM_H
