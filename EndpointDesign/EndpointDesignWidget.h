//-----------------------------------------------------------------------------
// File: EndpointDesignWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.5.2011
//
// Description:
// Endpoint design widget.
//-----------------------------------------------------------------------------

#ifndef ENDPOINTDESIGNWIDGET_H
#define ENDPOINTDESIGNWIDGET_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <common/GenericEditProvider.h>

#include <QGraphicsView>

class EndpointDesignDiagram;
class MainWindow;
class LibraryInterface;
class Component;
class DesignConfiguration;
class ProgramEntityItem;
class ComponentItem;

//-----------------------------------------------------------------------------
//! EndpointDesignWidget class.
//-----------------------------------------------------------------------------
class EndpointDesignWidget : public TabDocument
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    EndpointDesignWidget(LibraryInterface* lh, MainWindow* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~EndpointDesignWidget();

    /*!
     *  Sets the system identified with the given VLNV as the currently edited one.
     *
     *      @return False if there were errors opening the system; otherwise true.
     */
    bool setDesign(VLNV const& vlnv);

    /*!
     *  Sets the zoom level of the design.
     *
     *      @param [in] level The zoom level in percents.
     */
    virtual void setZoomLevel(int level);

    /*!
     *  Fits the whole design in view.
     */
    virtual void fitInView();

    /*!
     *  Sets the draw mode of the design.
     *
     *      @param [in] mode The draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

    /*!
     *  Returns the supported draw modes.
     */
    virtual unsigned int getSupportedDrawModes() const;

    /*!
     *  Returns the graphics view.
     */
    QGraphicsView* getView();

    /*!
     *  Returns the VLNV of the currently open system.
     */
    VLNV const* getOpenDocument() const;

    /*! \brief Get the vlnv of the current component
	 *
	 * \return Base class returns invalid vlnv.
	*/
	virtual VLNV getComponentVLNV() const;

    /*!
     *  Returns the edit provider.
     *
     *      @return Base class implementation returns null.
     *
     *      @remarks Edit support should be queried with getFlags().
     */
    IEditProvider* getEditProvider();

    /*!
     *  Returns the generic edit provider that manages the undo stack.
     */
    QSharedPointer<GenericEditProvider> getGenericEditProvider() const;

public slots:
    /*! 
     *  Saves the design to disk.
     */
    virtual bool save();

    /*!
     *  Adds a new column to the diagram.
     */
    void addColumn();

    //! Called when the vertical scrollbar is scrolled.
    void onVerticalScroll(int y);

signals:
    //! \brief Emitted when the user double-clicks a hierarchical component.
    void openDesign(const VLNV& vlnv, const QString& viewName);

    //! \brief Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! \brief Emitted when a program entity source should be opened in editor.
    void openSource(ProgramEntityItem* progEntity);

    //! Signaled when the draw mode of the diagram has changed.
    void modeChanged(DrawMode mode);

    //! Signaled when the zoom level has changed.
    void zoomChanged();

    //! Signaled when user selects a component on the draw board.
    void componentSelected(ComponentItem* component);

    //! Signaled when all items are deselected.
    void clearItemSelection();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    // Disable copying.
    EndpointDesignWidget(EndpointDesignWidget const& rhs);
    EndpointDesignWidget& operator=(EndpointDesignWidget const& rhs);

    enum
    {
        EDIT_HISTORY_SIZE = 50
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* lh_;

    //! The graphics view.
    QGraphicsView* view_;

    //! The endpoint design diagram.
    EndpointDesignDiagram* diagram_;

    //! The system that is being edited.
    QSharedPointer<Component> system_;

    //! The edit provider
    QSharedPointer<GenericEditProvider> editProvider_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTDESIGNWIDGET_H
