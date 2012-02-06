//-----------------------------------------------------------------------------
// File: ProgramEntityItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.2.2011
//
// Description:
// Graphics item for the collection of endpoints and a linked application.
//-----------------------------------------------------------------------------

#ifndef PROGRAMENTITYITEM_H
#define PROGRAMENTITYITEM_H

#include "SWComponentItem.h"

#include <common/CSourceWriter.h>
#include <MCAPI/MCAPIDesignerTypes.h>

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QVector>
#include <QTextStream>
#include <QSet>

class EndpointDesignDiagram;
class EndpointItem;
class EndpointStack;
class EndpointConnection;
class AppPlaceholderItem;
class MCAPIContentMatcher;
class SystemColumn;
class Component;
class MappingComponentItem;
class ApplicationItem;

//-----------------------------------------------------------------------------
//! ProgramEntityItem class.
//-----------------------------------------------------------------------------
class ProgramEntityItem : public SWComponentItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 8 };

    /*!
     *  Constructor.
     *
     *      @param [in] component                 The underlying endpoint component.
     *      @param [in] instanceName              The name of the component instance.
     *      @param [in] displayName               The component instance's display name.
     *      @param [in] description               The component instance's description.
     *      @param [in] configurableElementValue  The component instance's configurable element values.
     */
    ProgramEntityItem(QSharedPointer<Component> component,
                      QString const& instanceName,
                      QString const& displayName = QString(),
                      QString const& description = QString(),
                      QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>());

    /*!
     *  Destructor.
     */
    ~ProgramEntityItem();

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Sets the ordering of the endpoints based on the given map.
     *
     *      @param [in] positions The endpoint positions.
     */
    void setEndpointOrder(QMap<QString, QPointF> const& positions);

    /*!
     *  Expands/collapses the endpoints.
     *
     *      @param [in] expanded If true, the endpoints will be expanded. Otherwise they will be collapsed.
     */
    void setEndpointsExpanded(bool expanded);

    /*!
     *  Sets the program entity fixed/programmable. Fixed program entity does not have an application.
     *
     *      @param [in] fixed If true, the program entity is treated as fixed HW and
     *                        setting application is not possible. False for programmable behavior.
     */
    void setFixed(bool fixed);

    /*!
     *  Sets the application item.
     *
     *      @param [in] item The application item to set.
     */
    void setApplication(ApplicationItem* item);

    /*!
     *  Generates a source file thub based on the node's information.
     *
     *      @param [in] filename The name of the source file to generate.
     */
    void createSource(QString const& filename);

    /*!
     *  Updates the generated code if it exists.
     */
    void updateGeneratedCode();

    /*!
     *  Returns the application item or null if not set.
     */
    ApplicationItem* getApplication();

    /*!
     *  Retrieves the endpoint with the given name.
     *
     *      @param [in] name The  name of the endpoint.
     *
     *      @return The endpoint item or null if not found.
     */
    EndpointItem* getEndpoint(QString const& name);

    /*!
     *  Returns the list of endpoints.
     */
    QList<EndpointItem*> const& getEndpoints() const;

    /*!
     *  Returns the content matcher for this node.
     */
    QSharedPointer<MCAPIContentMatcher> getContentMatcher() const;

    /*!
     *  Returns the name of the source file or empty if not defined.
     */
    QString const& getSourceFile() const;

    /*!
     *  Returns the parent SW mapping component.
     */
    MappingComponentItem* getMappingComponent();

    /*!
     *  Returns the parent SW mapping component.
     */
    MappingComponentItem const* getMappingComponent() const;

    /*!
     *  Returns true if the endpoints are expanded.
     */
    bool isEndpointsExpanded() const;

    /*!
     *  Returns the graphics item type.
     */
    int type() const;

public slots:
    /*!
     *  Updates the size of the node item based on content.
     */
    void onEndpointStackChange(int height);

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    // Disable copying.
    ProgramEntityItem(ProgramEntityItem const& rhs);
    ProgramEntityItem& operator=(ProgramEntityItem const& rhs);

    /*!
     *  Generates MCAPI code for the node, both the source and header files.
     *
     *      @param [in] dir The directory where to place the files.
     */
    void generateCode(QString const& dir);

    /*!
     *  Generates the MCAPI header file.
     */
    void generateHeader(QString const& filename);

    /*!
     *  Generates the MCAPI source file.
     */
    void generateSource(QString const& filename, QList<ProgramEntityItem*> const& remoteNodes);

    /*!
     *  Generates the initializeMCAPI() function code.
     */
    void generateInitializeMCAPIFunc(CSourceWriter& writer);

    /*!
     *  Generates the createConnections() function code.
     */
    void generateCreateConnectionsFunc(CSourceWriter& writer);

    /*!
     *  Creates an indentation string based on QSettings.
     *
     *      @return The indentation string.
     */
    QString createIndentString();

    /*!
     *  Returns the source location for the generated code.
     */
    QString getGenSourceLocation() const;
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // Constants.
    enum
    {
        WIDTH = 240,
        FIXED_TOP_MARGIN = 40,
        TOP_MARGIN = 100,
        BOTTOM_MARGIN = 20,
        SPACING = 0
    };

    //! Icon for imported SW components.
    QGraphicsPixmapItem* importedIcon_;

    //! The endpoint stack.
    EndpointStack* endpointStack_;

    //! Application placeholder.
    AppPlaceholderItem* appPlaceholder_;

    //! The assigned application item.
    ApplicationItem* appItem_;

    //! The program entity's old position before mouse move.
    QPointF oldPos_;

    //! The connections that can change during mouse move.
    QSet<EndpointConnection*> conns_;

    //! If true, the program entity is treated as fixed HW with no application.
    bool fixed_;
};

//-----------------------------------------------------------------------------

#endif // PROGRAMENTITYITEM_H
