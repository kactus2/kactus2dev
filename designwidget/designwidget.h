/* 
 * 		filename: designwidget.h
 */

#ifndef DESIGNWIDGET_H
#define DESIGNWIDGET_H

#include "../LibraryManager/vlnv.h"
#include <common/widgets/tabDocument/TabDocument.h>
#include "blockdiagram.h"
#include <common/GenericEditProvider.h>

#include <QMainWindow>
#include <QString>
#include <QGraphicsView>
#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include <QComboBox>
#include <QKeyEvent>
#include <QSharedPointer>
#include <QRectF>

class LibraryInterface;
class VLNV;
class Component;
class DiagramComponent;
class DiagramPort;
class DesignConfiguration;

/*! \brief DesignWidget is used to edit and view one design
 *
 */
class DesignWidget : public TabDocument {

    Q_OBJECT

public:

    DesignWidget(LibraryInterface *lh, QWidget *parent = 0);

	//! \brief The destructor
	virtual ~DesignWidget();

	/*! \brief Set the design to be displayed in the block diagram
	 *
	 * \param vlnv VLNV of the design to open
	 * \param viewName Name of the view to open within component.
	 *
	*/
	void setDesign(const VLNV* vlnv, const QString& viewName);

    /*! \brief Get the VLNV of the design that is being edited
     *
     */
    const VLNV *getOpenDocument() const;

	/*! \brief Get the vlnv of the current component
	 *
	 * \return VLNV of the hierarchical component being edited.
	*/
	virtual VLNV getComponentVLNV() const;

	/*! \brief Get pointer to the top component being edited.
	 *
	 * \return Pointer to the top component.
	*/
	QSharedPointer<Component> getHierComponent() const;

    /*!
     *  Called when a new column is requested to be added to the design.
     */
    void addColumn();

    /*!
     *  Returns the associated graphics view.
     */
    QGraphicsView* getView();

    /*!
     *  Refreshes the document view.
     */
    virtual void refresh();

    /*!
     *  Prints the design.
     */
    virtual void print();

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
     *  Returns the edit provider.
     *
     *      @return Base class implementation returns null.
     *
     *      @remarks Edit support should be queried with getFlags().
     */
    IEditProvider* getEditProvider();

	/*! \brief Get the name of the view that is currently open
	 *
	 * \return QString containing the view name.
	*/
	QString getOpenViewName() const;

	/*! \brief Get the component instances contained in the design.
	 *
	 * \return QList containing pointers to the component instances.
	*/
	QList<DiagramComponent*> getInstances() const;

	/*! \brief Get the configuration for this design.
	 *
	 * \return Pointer to the design configuration of this design.
	*/
	QSharedPointer<DesignConfiguration> getConfiguration() const;

	/*! \brief Get the edit provider that manages the undo stack.
	 *
	 *
	 * \return Pointer to the edit provider.
	*/
	QSharedPointer<GenericEditProvider> getGenericEditProvider() const;

	/*! \brief Check if the editor is editing a hardware implementation or not.
	 *
	 * \return True if the edited object is hardware implementation.
	*/
	virtual bool isHWImplementation() const;

	/*! \brief Get the rect that bounds all items in the design.
	 *
	 * \return QRectF The rect that bounds all items in the design.
	*/
	QRectF itemsBoundingRect() const;

	/*! \brief Get the graphics scene that holds the graphic items.
	 *
	 * \return BlockDiagram* Pointer to the scene instance.
	*/
	BlockDiagram* scene() const;

	/*! \brief Create a design that represents the current drawed design.
	 *
	 * \param vlnv The vlnv to set for the design.
	 *
	 * \return QSharedPointer<Design> Pointer to the created design.
	*/
	QSharedPointer<Design> createDesign(const VLNV& vlnv);

signals:
    /*! \brief Signal openDesign is emitted when user double clicks a hierarchical
     * component
     */
    void openDesign(const VLNV& vlnv, const QString& viewName);

	//! \brief Emitted when component with given vlnv should be opened in editor.
	void openComponent(const VLNV& vlnv);

    //! Signaled when the bus with the given vlnv should be opened for editing.
    void openBus(VLNV const& vlnv, VLNV const& absDefVLNV, bool disableBusDef);

	/*! \brief Emitted when user wants to refresh this widget
	 *
	 * \param designWidget Pointer to this DesignWidget instance
	 *
	*/
	void refresh(const DesignWidget* designWidget);

	/*! \brief Emitted when user selects a component in the draw board.
	 *
	 * \param component Pointer to the DiagramComponent instance that is selected.
	 *
	*/
	void componentSelected(ComponentItem* component);

	/*! \brief Emitted when user selects a port in the draw board.
	 *
	 * \param port Pointer to the DiagramPort instance that is selected.
	 *
	*/
	void portSelected(DiagramPort* port);

    /*! \brief Emitted when user selects an interface in the draw board.
	 *
	 * \param port Pointer to the DiagramPort instance that is selected.
	 *
	*/
	void interfaceSelected(DiagramInterface* interface);

	/*! \brief Emitted when user selects a connection in the draw board.
	 *
	 * \param connection Pointer to the connection that is selected.
	 *
	*/
	void connectionSelected(DiagramInterconnection* connection);

	/*! \brief Emitted when user de-selects all items.
	 *
	*/
	void clearItemSelection();

    void modeChanged(DrawMode mode);
    void zoomChanged();

	//! \brief Emitted when a new component is instantiated to the design.
	void componentInstantiated(DiagramComponent* comp);

	//! \brief Emitted when a component instance is removed from the design.
	void componentInstanceRemoved(DiagramComponent* comp);

public slots:
    /*! \brief Saves the design to disk
     *
     */
    virtual bool save();

	//! \brief Saves the design as new object
	virtual bool saveAs();

	//! \brief Called when user clicks the generate top-vhdl icon
	void onVhdlGenerate();

	//! \brief Called when user clicks the generate quartus project icon
	void onQuartusGenerate();

	//! \brief Called when user clicks the generate modelsim icon
	void onModelsimGenerate();

	//! Called when the graphics view has been scrolled vertically.
    void onVerticalScroll(int y);

protected:
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent* event);

private:
    DesignWidget(const DesignWidget &);
    DesignWidget &operator=(const DesignWidget &);

    enum
    {
        EDIT_HISTORY_SIZE = 50
    };

	/*! \brief Create an empty hierarchical component
     *
     * \return Pointer to the created component.
    */
    QSharedPointer<Component> createEmptyDesign(const VLNV* prevlnv);

	/*! \brief Create an empty design and configuration for a component.
	 *
	 * \param component Pointer to the component to create the design for.
	 * \param dirPath File path to the directory to create the design and
	 * configuration to.
	 *
	*/
	void createDesignForComponent(QSharedPointer<Component> component,
		const QString& dirPath);

	//! \brief Pointer to the hierarchical component that is being edited.
	QSharedPointer<Component> hierComponent_;

	//! \brief The name of the hierarchical component being edited.
	QString viewName_;

	//! \brief Pointer to the design being currently edited
	//Design* designUnderEditing_;

    QGraphicsView *view_;
    BlockDiagram *diagram_;

    LibraryInterface *lh_;

    QDockWidget *selectionDock_;

    //! The edit provider
    QSharedPointer<GenericEditProvider> editProvider_;
};

#endif // DESIGNWIDGET_H
