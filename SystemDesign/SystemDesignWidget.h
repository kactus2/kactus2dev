//-----------------------------------------------------------------------------
// File: SystemDesignWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design widget.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNWIDGET_H
#define SYSTEMDESIGNWIDGET_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <common/GenericEditProvider.h>

#include <QGraphicsView>

class SystemDesignDiagram;
class MainWindow;
class LibraryInterface;
class Component;
class DesignConfiguration;
class ProgramEntityItem;
class ComponentItem;

//-----------------------------------------------------------------------------
//! SystemDesignWidget class.
//-----------------------------------------------------------------------------
class SystemDesignWidget : public TabDocument
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    SystemDesignWidget(bool onlySW, LibraryInterface* lh, MainWindow* mainWnd, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~SystemDesignWidget();

    /*!
     *  Sets the system identified with the given VLNV as the currently edited one.
     *
     *      @return False if there were errors opening the system; otherwise true.
     */
    bool setDesign(VLNV const& vlnv, QString const& viewName);
    
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
    void openSWDesign(const VLNV& vlnv, const QString& viewName);

    //! \brief Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! Emitted when a C source file should be opened for editing.
    void openCSource(ComponentItem* compItem);

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
    SystemDesignWidget(SystemDesignWidget const& rhs);
    SystemDesignWidget& operator=(SystemDesignWidget const& rhs);

    enum
    {
        SYSTEM_COLUMN_WIDTH = 319,
        SW_COLUMN_WIDTH = 259,
        IO_COLUMN_WIDTH = 119,
        EDIT_HISTORY_SIZE = 50
    };

    bool setDesign(QSharedPointer<Component> comp, const QString& viewName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the widget is used for editing SW designs.
    bool onlySW_;

    //! The library interface.
    LibraryInterface* lh_;

    //! The graphics view.
    QGraphicsView* view_;

    //! The system design diagram.
    SystemDesignDiagram* diagram_;

    //! The system that is being edited.
    QSharedPointer<Component> system_;

    //! The attached design configuration.
    QSharedPointer<DesignConfiguration> designConf_;

    //! The edit provider
    QSharedPointer<GenericEditProvider> editProvider_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNWIDGET_H
