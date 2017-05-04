//-----------------------------------------------------------------------------
// File: AdhocEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2016
//
// Description:
// Editor to edit the details of an Ad hoc port in designAd-hoc editor.
//-----------------------------------------------------------------------------

#ifndef ADHOCEDITOR_H
#define ADHOCEDITOR_H

#include <designEditors/common/ConnectionEditor/AdHocBoundsModel.h>

#include <QWidget>
#include <QLabel>
#include <QTableWidget>

class AdHocItem;
class AdHocConnection;
class PartSelect;
class ExpressionEditor;
class ExpressionParser;
class ComponentParameterFinder;
class Design;
class IEditProvider;
class HWDesignDiagram;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Editor to edit the details of an Ad hoc port in designAd-hoc editor.
//-----------------------------------------------------------------------------
class AdHocEditor : public QWidget
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent The parent widget.
     */
	AdHocEditor(QWidget* parent);

	/*!
     *  The destructor.
     */
	virtual ~AdHocEditor();

    /*!
     *  Set the ad hoc port.
     *
     *      @param [in] endPoint            The selected ad hoc port.
     *      @param [in] containingDiagram   The design diagram containing the ad hoc port.
     *      @param [in] editProvider        The provider for undo commands.
     */
    void setAdhocPort(AdHocItem* endPoint, HWDesignDiagram* containingDiagram,
        QSharedPointer<IEditProvider> editProvider);

public slots:

    /*!
     *  Hides all information in the editor.
     */
	void clear();

private slots:

    /*!
     *  Handles the changes in tied value editor.
     */
    void onTiedValueChanged();

    /*!
     *  Handles the changes in the left bound of the tied value.
     */
    void onTiedValueLeftBoundChanged();

    /*!
     *  Handles the changes in the right bound of the tied value.
     */
    void onTiedValueRightBoundChanged();

signals:

    /*!
     *  Informs of a change in the contents.
     */
    void contentChanged();

private:

	// Disable copying.
	AdHocEditor(AdHocEditor const& rhs);
	AdHocEditor& operator=(AdHocEditor const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Calculate the formatted value for a given expression.
     *
     *      @param [in] expression  The selected expression.
     *
     *      @return The formatted value of a given expression.
     */
    QString formattedValueFor(QString const& expression) const;

    /*!
     *  Get the tied value associated with contained ad hoc port.
     *
     *      @param [in] connectionItem  The tied value connection of the associated ad hoc port.
     *
     *      @return The tied value of the ad hoc port.
     */
    QString getTiedValue(QSharedPointer<AdHocConnection> connectionItem) const;

    /*!
     *  Get the ad hoc connection containing this ad hoc ports tied value.
     *
     *      @param [in] instanceName    Name of the instance containing the port. Empty for top level ports.
     *
     *      @return The selected ad hoc connection, or an empty pointer if no connection was found.
     */
    QSharedPointer<AdHocConnection> getTiedConnection(QString const& instanceName) const;

    /*!
     *  Create a tie off change command.
     *
     *      @param [in] newTiedValue    The new tie off value.
     */
    void createTieOffChangeCommand(QString const& newTiedValue);

    /*!
     *  Calculate the tie off value.
     *
     *      @param [in] tieOffValue     The selected tie off value.
     *
     *      @return The calculated tie off value.
     */
    QString getParsedTieOffValue(QString const& tieOffValue) const;

    /*!
     *  Set a tool tip for the tied value editor.
     *
     *      @param [in] tiedValue   The new tied value.
     */
    void setTiedValueEditorToolTip(QString const& tiedValue);
    
    /*!
     *  Update the bounds of the tied value connection.
     *
     *      @param [in] portPartSelect  Part select of the referenced port.
     */
    void updateTiedValueBounds(QSharedPointer<PartSelect> portPartSelect);

    /*!
     *  Get the part select of the referenced port.
     *
     *      @param [in] adHocConnection     Connection containing the ad hoc item.
     *
     *      @return The part select of the referenced port.
     */
    QSharedPointer<PartSelect> getEndPointPartSelect(QSharedPointer<AdHocConnection> adHocConnection) const;

    /*!
     *  Create a change command for the tie off bounds.
     *
     *      @param [in] newLeftBound    New value for the left bound of the tied value.
     *      @param [in] newRightBound   New value for the right bounds of the tied value.
     */
    void createTieOffBoundsChangeCommand(QString const& newLeftBound, QString const& newRightBound);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used component parameter finder. Allows the changing of the target component.
    QSharedPointer<ComponentParameterFinder> componentFinder_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Used to format the referenced parameter UUIDs to parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! Label containing the name of the ad hoc port.
    QLabel* portName_;

    //! Label containing the direction of the ad hoc port.
    QLabel* portDirection_;

    //! Label containing the left bound value of the ad hoc port.
    QLabel* leftBoundValue_;

    //! Label containing the right bound value of the ad hoc port.
    QLabel* rightBoundValue_;

    //! Expression editor for the tied value.
    ExpressionEditor* tiedValueEditor_;

    //! The selected ad hoc port item.
    AdHocItem* containedPortItem_;

    //! The currently used provider for undo commands.
    QSharedPointer<IEditProvider> editProvider_;

    //! The design diagram containing the ad hoc port item.
    HWDesignDiagram* designDiagram_;

    //! Editor for the left bound value of the tied value.
    ExpressionEditor* tiedValueLeftBoundEditor_;

    //! Editor for the right bound value of the tied value.
    ExpressionEditor* tiedValueRightBoundEditor_;
};

#endif // ADHOCEDITOR_H
