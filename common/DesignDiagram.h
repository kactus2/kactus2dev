//-----------------------------------------------------------------------------
// File: DesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.4.2012
//
// Description:
// Base class for all design diagrams.
//-----------------------------------------------------------------------------

#ifndef DESIGNDIAGRAM_H
#define DESIGNDIAGRAM_H

#include <QGraphicsScene>

#include <designwidget/AdHocEnabled.h>
#include <common/DrawMode.h>

class MainWindow;
class LibraryInterface;
class GenericEditProvider;
class Component;
class ComponentItem;
class DesignConfiguration;
class Design;
class VLNV;

//-----------------------------------------------------------------------------
//! Base class for all design diagrams.
//-----------------------------------------------------------------------------
class DesignDiagram : public QGraphicsScene, public AdHocEnabled
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
    DesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                  GenericEditProvider& editProvider, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~DesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*!
     *  Opens a design for editing.
     *
     *      @param [in] component  The component whose design to edit.
     *      @param [in] viewName   The name of the view to open.
     */
    bool setDesign(QSharedPointer<Component> component, QString const& viewName);

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const = 0;

    /*!
     *  Attaches the data source to an ad-hoc editor.
     */
    virtual void attach(AdHocEditor* editor);

    /*!
     *  Detaches the data source from the ad-hoc editor.
     */
    virtual void detach(AdHocEditor* editor);

    /*!
     *  Adds an instance name to the list of used instance names.
     *
     *      @param [in] name The name of the instance.
     *
     *      @remarks This function should be called when a component instance is added to the diagram.
     */
    void addInstanceName(QString const& name);

    /*!
     *  Removes the instance name from the list so the name can be used again in the design.
	 *
	 *      @param name The name of the removed instance.
	 *
     *      @remarks This function should be called when user removes component instance.
	 */
	void removeInstanceName(const QString& name);

	/*!
     *  Updates the list of instance names.
	 *
	 *      @param oldName The old name of the component instance.
	 *      @param newName The new name of the component instance.
	 */
	void updateInstanceName(const QString& oldName, const QString& newName);

    /*!
     *  Sets the draw mode of the diagram.
     *
     *      @param [in] mode The draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Sets the block diagram locked/unlocked.
     *
     *      @param [in] locked If true, the diagram is locked. Otherwise it is unlocked.
     */
    void setProtection(bool locked);

    /*!
     *  Returns the current draw mode.
     */
    DrawMode getMode() const;

    /*!
     *  Returns true if the diagram is in locked state.
     */
    bool isProtected() const;

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface() const;

    /*!
     *  Returns the main window.
     */
    MainWindow* getMainWindow() const;

    /*!
     *  Returns the edit provider.
     */
    GenericEditProvider& getEditProvider();

    /*!
     *  Returns the component whose design is being edited.
     */
    QSharedPointer<Component> getEditedComponent() const;

    /*!
     *  Returns the design configuration.
     */
    QSharedPointer<DesignConfiguration> getDesignConfiguration() const;

public slots:
    //! Called when a component instance is added to the diagram.
    virtual void onComponentInstanceAdded(ComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    virtual void onComponentInstanceRemoved(ComponentItem* item);

signals:
    //! Signaled when the draw mode has changed.
    void modeChanged(DrawMode mode);

    //! Signaled when the contents of the diagram have changed.
    void contentChanged();

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage);

    //! Sends a notification to the user.
    void noticeMessage(const QString& noticeMessage);

    //! Signaled when a new component is instantiated to the design.
    void componentInstantiated(ComponentItem* item);

    //! Signaled when a component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem* item);

    //! Signaled when user selects a component on the draw board.
    void componentSelected(ComponentItem* component);

    //! Signaled when all items are deselected.
    void clearItemSelection();

protected:
    /*!
     *  Creates a unique instance name for the given component.
     *
     *      @param [in] component The component whose instance is being created.
     *
     *      @return The name for the component instance.
     */
    QString createInstanceName(QSharedPointer<Component> component);

    /*!
     *  Creates a unique instance name with the given base name.
     *
     *      @param [in] baseName The base name for the instance.
     */
    QString createInstanceName(QString const& baseName);

    /*!
     *  Draws the diagram background.
     */
     virtual void drawBackground(QPainter* painter, QRectF const& rect);

private:
    // Disable copying.
    DesignDiagram(DesignDiagram const& rhs);
    DesignDiagram& operator=(DesignDiagram const& rhs);

    /*!
     *  Opens the given design for editing.
     *
     *      @param [in] design The design to open.
     */
    virtual void openDesign(QSharedPointer<Design> design) = 0;
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Library interface.
    LibraryInterface* lh_;

    //! A pointer to the main window.
    MainWindow* mainWnd_;

    //! The edit provider for undo/redo.
    GenericEditProvider& editProvider_;

    //! The component whose design is being edited.
    QSharedPointer<Component> component_;

    //! The design configuration.
    QSharedPointer<DesignConfiguration> designConf_;

    //! The current draw mode.
    DrawMode mode_;

    //! The list of used instance names.
    QStringList instanceNames_;

    //! If true, the diagram is locked and cannot be modified.
    bool locked_;
};

//-----------------------------------------------------------------------------

#endif // DESIGNDIAGRAM_H
