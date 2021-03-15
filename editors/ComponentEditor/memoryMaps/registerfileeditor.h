//-----------------------------------------------------------------------------
// File: registerfileeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Editor for register files.
//-----------------------------------------------------------------------------

#ifndef REGISTERFILEEDITOR_H
#define REGISTERFILEEDITOR_H

#include <QSharedPointer>
#include <QGroupBox>

class RegisterBase;
class Component;
class LibraryInterface;
class ParameterFinder;
class ExpressionFormatter;
class RegisterFileValidator;
class EditableTableView;
class RegisterFileModel;

//-----------------------------------------------------------------------------
//! Editor for register files.
//-----------------------------------------------------------------------------
class RegisterFileEditor : public QGroupBox
{
    Q_OBJECT

public:

    /*!
     * The constructor.
     *
     *     @param [in] registerFile             The register file to edit.
     *     @param [in] component                The component containing the register file.
     *     @param [in] handler                  The available IP-XACT library.
     *     @param [in] parameterFinder          Finder for parameters.
     *     @param [in] expressionFormatter      Formatter for expressions.
     *     @param [in] registerFileValidator    Validator for register files.
     *     @param [in] parent                   The parent widget.
     */
     RegisterFileEditor(QSharedPointer<QList<QSharedPointer<RegisterBase> > >  registerFile,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<RegisterFileValidator> registerFileValidator,
        QWidget* parent = 0);

    //! The destructor.
    virtual ~RegisterFileEditor() = default;

    //! No copying.
    RegisterFileEditor(const RegisterFileEditor& other) = delete;

    //! No assignment.
    RegisterFileEditor& operator=(const RegisterFileEditor& other) = delete;

    //! Refresh the editor content.
    void refresh();

signals:
    /*!
     *  Informs that the contents of the editor have changed.
     */
    void contentChanged();

    /*!
     *  Informs of a need to redraw the visualizer.
     */
    void graphicsChanged(int index);

    /*!
     *  Sends an error message forward.
     *
     *      @param [in] msg     The error message.
     */
    void errorMessage(const QString& msg) const;

    /*!
     *  Sends a notification message forward.
     *
     *      @param [in] msg     The notification message.
     */
    void noticeMessage(const QString& msg) const;

    /*!
     *  Increase the amount of references made to the given parameter.
     *
     *      @param [in] id  The id of the given parameter.
     */
    void increaseReferences(QString id) const;

    /*!
     *  Decrease the amount of references made to the given parameter.
     *
     *      @param [in] id  The id the given parameter.
     */
    void decreaseReferences(QString id) const;

    /*!
     *  Informs that a new item has been created.
     *
     *      @param [in] index   The index of the new item.
     */
    void childAdded(int index);

    /*!
     *  Informs that an item has been removed.
     *
     *      @param [in] index   The index of the removed item.
     */
    void childRemoved(int index);

    void addressUnitBitsChanged(int);

    void childAddressingChanged(int index);

private:

    //! The view to display the items.
    EditableTableView* view_;

    //! Pointer to the model that manages the details of address block.
    RegisterFileModel* model_;
};

#endif // REGISTERFILEEDITOR_H
