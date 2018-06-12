#ifndef COMPONENTEDITORREGISTERFILEITEM_H
#define COMPONENTEDITORREGISTERFILEITEM_H


#include "componenteditoritem.h"

#include <IPXACTmodels/Component/RegisterFile.h>
#include <QSharedPointer>

class MemoryVisualizationItem;
class MemoryMapsVisualizer;
class RegisterFileGraphItem;
class RegisterFileValidator;
class ComponentEditorRegisterFileItem : public ComponentEditorItem
{
        Q_OBJECT
public:
    /*!
     *  The constructor.
     *
     *      @param [in] addrBlock               The address block being edited.
     *      @param [in] model                   The model that owns the items.
     *      @param [in] libHandler              The instance that manages the library.
     *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The instance for counting references to parameters.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression formatter.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
     *      @param [in] parent                  The parent item.
     */
        ComponentEditorRegisterFileItem(
                QSharedPointer<RegisterFile> regFile,
                ComponentEditorTreeModel* model,
                LibraryInterface* libHandler,
                QSharedPointer<Component> component,
                QSharedPointer<ParameterFinder> parameterFinder,
                QSharedPointer<ExpressionFormatter> expressionFormatter,
                QSharedPointer<ReferenceCounter> referenceCounter,
                QSharedPointer<ExpressionParser> expressionParser,
                QSharedPointer<RegisterFileValidator> registerFileValidator,
                ComponentEditorItem* parent);

        virtual QString getTooltip() const;
        virtual QString text() const;
        virtual bool isValid() const;
        virtual ItemEditor* editor();
        virtual void createChild(int index);
        virtual ItemVisualizer* visualizer();
        virtual void setVisualizer(MemoryMapsVisualizer* visualizer);
        virtual QGraphicsItem* getGraphicsItem();
        virtual void removeGraphicsItem();

protected slots:
        virtual void updateGraphics();
        virtual void onGraphicsChanged();

private:
        //! No copying. No assignment.
        ComponentEditorRegisterFileItem(const ComponentEditorRegisterFileItem& other);
        ComponentEditorRegisterFileItem& operator=(const ComponentEditorRegisterFileItem& other);

        void resizeGraphicsToCurrentDimensionSize();
        void createDimensionGraphicsItem(int dimensionIndex, MemoryVisualizationItem* parentItem);
        void removeDimensionGraphicsItem(int dimensionIndex, MemoryVisualizationItem* parentItem);

        //! The address block being edited.
        QSharedPointer<RegisterFile> registerFile_;

        //! The visualizer for memory maps.
        MemoryMapsVisualizer* visualizer_;

        //! The graph items that visualizes the register file dimensions.
        QList<RegisterFileGraphItem*> registerFileDimensions_;

        //! The expression parser to use.
        QSharedPointer<ExpressionParser> expressionParser_;

        //! The used address block validator.
        QSharedPointer<RegisterFileValidator> registerFileValidator_;
};

#endif // COMPONENTEDITORREGISTERFILEITEM_H
