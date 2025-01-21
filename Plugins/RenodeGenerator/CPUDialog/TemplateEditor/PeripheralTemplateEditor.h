//-----------------------------------------------------------------------------
// File: PeripheralTemplateEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.05.2023
//
// Description:
// Editor for peripheral templates in renode generator.
//-----------------------------------------------------------------------------

#ifndef PERIPHERALTEMPLATEEDITOR_H
#define PERIPHERALTEMPLATEEDITOR_H

#include <QWidget>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

class EditableTableView;
class PeripheralTemplateModel;
class PeripheralTemplateDelegate;

//-----------------------------------------------------------------------------
//! Editor for peripheral templates in renode generator.
//-----------------------------------------------------------------------------
class PeripheralTemplateEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] parent  Pointer to the owner of this widget.
     */
    PeripheralTemplateEditor(QString const& generationFolder, QWidget *parent = 0);

    /*!
     *  Setup the peripheral template.
     *	
     *    @param [in] newTemplates    List of new peripheral templates.
     */
    void setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplars);

    /*!
     *  The destructor.
     */
    virtual ~PeripheralTemplateEditor() = default;

    /*!
     *  Get the current peripheral templates.
     *	
     *    @return The current peripheral templates.
     */
    QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > getTemplates() const;

    //! No copying. No assignment.
    PeripheralTemplateEditor(const PeripheralTemplateEditor& other) = delete;
    PeripheralTemplateEditor& operator=(const PeripheralTemplateEditor& other) = delete;

private:

    /*!
     *  Connect editor signals.
     */
    void connectSignals() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view for the table.
    EditableTableView* view_;

    //! The containing model.
    PeripheralTemplateModel* model_;

    //! The delegate.
    PeripheralTemplateDelegate* templateDelegate_;
};

#endif // PERIPHERALTEMPLATEEDITOR_H
