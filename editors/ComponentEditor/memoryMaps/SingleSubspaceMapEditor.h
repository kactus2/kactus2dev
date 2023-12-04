//-----------------------------------------------------------------------------
// File: SingleSubspaceMapEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2022
//
// Description:
// Editor for editing the details of a single subspace map.
//-----------------------------------------------------------------------------

#ifndef SINGLESUBSPACEMAPEDITOR_H
#define SINGLESUBSPACEMAPEDITOR_H

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>

#include <QComboBox>
#include <QSharedPointer>

class ExpressionFormatter;
class ExpressionEditor;
class ExpressionParser;
class LibraryInterface;
class ParameterFinder;
class MemoryMapBase;
class SubspaceMapInterface;
class SubspaceMapValidator;
class BusReferenceComboBox;
class SegmentComboBox;

//-----------------------------------------------------------------------------
//! Editor for editing the details of a single subspace map.
//-----------------------------------------------------------------------------
class SingleSubspaceMapEditor : public ItemEditor
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] subspaceInterface       Interface for accessing subspace maps.
	 *      @param [in] subspaceMap             The subspace map being edited.
     *      @param [in] containingMap           Memory map containing the edited address block.
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance managing the library.
	 *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionParser        The expression parser.
	 *      @param [in] parent                  The parent of this editor.
	 */
    SingleSubspaceMapEditor(SubspaceMapInterface* subspaceInterface,
        QSharedPointer<SubSpaceMap> subspaceMap,
        QSharedPointer<MemoryMapBase> containingMap,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget* parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~SingleSubspaceMapEditor() = default;

    //! No copying. No assignment.
    SingleSubspaceMapEditor(const SingleSubspaceMapEditor& other) = delete;
    SingleSubspaceMapEditor& operator=(const SingleSubspaceMapEditor& other) = delete;

	/*!
	 *  Reload all the contained editors.
	 */
	virtual void refresh() override final;

public slots:

    /*
     *  Handles subspace map name change from memory map editor.
     *
     *      @param [in] oldName     The old name.
     *      @param [in] newName     The new name.
     */
    void onSubspaceNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Handles subspace map name change from name editor.
     */
    void onSubspaceNameChanged();

signals:

    /*!
     *  Inform of a change in subspace map addressing.
     */
    void addressingChanged();

    /*!
     *  Inform of a change in subspace map name.
     */
    void thisEditorNameChange(QString const& oldName, QString const& newName);

protected:

    /*!
	 *  Shows the widget and informs of the required help document.
	 *
	 *      @param [in] event   The show event.
	 */
	virtual void showEvent(QShowEvent* event) override final;

private slots:
    
    /*!
     *  Sets the value in the base address editor.
     */
    void onBaseAddressChanged();

    /*!
     *  Called when isPresent is changed.
     */
    void onIsPresentEdited();

    /*!
     *  Handle the change in master bus interface reference.
     *
     *      @param [in] newMasterReference  The new master bus interface reference.
     */
    void onMasterSelected(QString const& newMasterReference);

    /*!
     *  Handle the change in segment reference.
     *
     *      @param [in] newSegment  The new segment reference.
     */
    void onSegmentSelected(QString const& newSegment);

private:

    /*!
     *  Construct the combo boxes for master bus interface reference and segment reference.
     */
    void constructComboBoxes();

    /*!
     *  Get the address space of the selected bus interface.
     *
     *      @param [in] busReference    Name of the selected bus interface.
     *
     *      @return Address space of the selected bus interface.
     */
    QSharedPointer<AddressSpace> getSpace(QString const& busReference);

    /*!
     *  Sets the layout for the editor.
     */
    void setupLayout();

    /*!
     *  Connects all the signals of the editors.
     */
    void connectSignals() const;

    /*!
     *  Change the status of signal blocking in all the expression editors.
     *
     *      @param [in] blockStatus     The new signal blocking status.
     */
    void changeExpressionEditorSignalBlockStatus(bool blockStatus);

    /*!
     *  Calculates the value of an expression.
     *
     *      @param [in] expression  The expression, whose value is to be calculated.
     *
     *      @return The calculated value of the given expression.
     */
    QString formattedValueFor(QString const& expression) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Editor for containing the name group information of the address block.
    NameGroupEditor nameEditor_;

    //! Editor for the base address of the address block.
    ExpressionEditor* baseAddressEditor_;

    //! Editor for the is present of the address block-
    ExpressionEditor* isPresentEditor_;

    //! Selector for initiator bus interface reference.
    BusReferenceComboBox* initiatorSelector_;

    //! Selector for segment reference.
    SegmentComboBox* segmentSelector_;

    //! Name of the current address block.
    std::string subspaceName_;

    //! Interface for address blocks.
    SubspaceMapInterface* subspaceInterface_;

    //! Memory map item containing the address block.
    QSharedPointer<MemoryMapBase> containingMap_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // SINGLESUBSPACEMAPEDITOR_H
