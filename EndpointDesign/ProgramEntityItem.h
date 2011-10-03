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

#ifndef NODEITEM_H
#define NODEITEM_H

#include "SWComponentItem.h"

#include <MCAPI/MCAPIDesignerTypes.h>

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QVector>
#include <QTextStream>

class EndpointDesignDiagram;
class EndpointItem;
class EndpointStack;
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
     *      @param [in] parent                    The parent SW mapping component.
     */
    ProgramEntityItem(QSharedPointer<Component> component,
                      QString const& instanceName,
                      QString const& displayName,
                      QString const& description,
                      QMap<QString, QString> const& configurableElementValues,
                      MappingComponentItem* parent);

    /*!
     *  Destructor.
     */
    ~ProgramEntityItem();

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Sets the application item.
     *
     *      @param [in] item The application item to set.
     */
    void setApplication(ApplicationItem* item);

    /*!
     *  Adds an endpoint to the node.
     *
     *      @param [in] name  The name of the endpoint.
     *      @param [in] type  The endpoint type.
     */
    void addEndpoint(QString const& name, MCAPIEndpointDirection type);

    /*!
     *  Generates a source file thub based on the node's information.
     *
     *      @param [in] filename The name of the source file to generate.
     */
    void createSource(QString const& filename);

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
     *  Generates code for the node to the given stream.
     *
     *      @param [in] stream  The text output stream.
     *      @param [in] indent  The indentation string.
     */
    void generateCode(QTextStream& stream, QString const& indent);

    /*!
     *  Creates an indentation string based on QSettings.
     *
     *      @return The indentation string.
     */
    QString createIndentString();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // Constants.
    enum
    {
        WIDTH = 240,
        TOP_MARGIN = 100,
        BOTTOM_MARGIN = 20,
        SPACING = 0
    };

    //! The parent SW mapping component.
    MappingComponentItem* parentComp_;

    //! The endpoint stack.
    EndpointStack* endpointStack_;

    //! Application placeholder.
    AppPlaceholderItem* appPlaceholder_;

    //! The assigned application item.
    ApplicationItem* appItem_;
};

//-----------------------------------------------------------------------------

#endif // NODEITEM_H
