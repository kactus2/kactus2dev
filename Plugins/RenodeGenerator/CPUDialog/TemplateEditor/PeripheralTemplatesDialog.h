//-----------------------------------------------------------------------------
// File: PeripheralTemplatesDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.05.2023
//
// Description:
// Dialog for editing templates for python peripherals in Renode generator.
//-----------------------------------------------------------------------------

#ifndef PERIPHERALTEMPLATESDIALOG_H
#define PERIPHERALTEMPLATESDIALOG_H

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <QDialog>

class PeripheralTemplateEditor;

//-----------------------------------------------------------------------------
//! Dialog for editing templates for python peripherals in Renode generator.
//-----------------------------------------------------------------------------
class PeripheralTemplatesDialog : public QDialog
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] configurationFolder     Folder path for the generator.
     *      @param [in] parent                  Parent widget.
     */
	PeripheralTemplatesDialog(QString const& generationFolder, QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~PeripheralTemplatesDialog() = default;

    /*!
     *  Setup the available peripheral templates.
     *	
     *      @param [in] newTemplates    List of the selected peripheral templates.
     */
    void setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates);

    /*!
     *  Get a list of the available peripheral templates.
     *
     *      @return List of peripheral templates.
     */
    QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > getTemplates() const;

private:

    // Disable copying.
    PeripheralTemplatesDialog(PeripheralTemplatesDialog const& rhs);
    PeripheralTemplatesDialog& operator=(PeripheralTemplatesDialog const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! List of available peripheral templates.
    PeripheralTemplateEditor* templatesEditor_;
};

#endif // PERIPHERALTEMPLATESDIALOG_H