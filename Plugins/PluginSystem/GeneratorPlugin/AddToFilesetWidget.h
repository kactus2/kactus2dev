//-----------------------------------------------------------------------------
// File: AddToFilesetWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.11.2025
//
// Description:
// Widget for selecting file set to add generated item into.
//-----------------------------------------------------------------------------

#ifndef ADDTOFILESETWIDGET_H
#define ADDTOFILESETWIDGET_H

#include <QGroupBox>
#include <QComboBox>
#include <QLabel>

class AddToFilesetWidget : public QGroupBox
{
    Q_OBJECT
public:

    /*!
     *	The constructor.
     *  
     *    @param [in] parent     The parent widget of this widget.
     */
    AddToFilesetWidget(QWidget* parent = nullptr);

    //! Disable copying
    AddToFilesetWidget(AddToFilesetWidget const& other) = delete;
    AddToFilesetWidget& operator=(AddToFilesetWidget const& other) = delete;
	
    virtual ~AddToFilesetWidget() = default;

    /*!
     *	Set the available file sets.
     *  
     *    @param [in] fileSetNames     Available file set names.
     */
    void setExistingFileSets(QStringList const& fileSetNames);

    /*!
     *	Select default file set (e.g. based on view), if it exists. Otherwise select first item.
     *  
     *    @param [in] defaultName     The default file set to select.
     */
    void selectDefaultFileSet(QString const& defaultName);
    
    /*!
     *	Set checked state of groupbox.
     *  
     *    @param [in] checked     Flag if group box should be checked (enabled) or not.
     */
    void setChecked(bool checked);

    /*!
     *	Get the currently selected file set.
     *  
     *    @return  The currently selected file set name.
     */
    QString currentSelection() const;

signals:

    /*!
     *	Inform parent that file set has been changed.
     */
    void fileSetChanged(QString const& fileSetName);

private slots:

    /*!
     *	Handle file set change.
     *  
     *    @param [in] fileSetName     
     */
    void onFileSetChanged(QString const& fileSetName);

private:

    //! The combobox to select the fileset.
    QComboBox fileSetSelection_;

    //! The label to give warnings about file set selection.
    QLabel fileSetWarningLabel_;

    //! Existing component file sets.
    QStringList fileSetNames_;
};

#endif // ADDTOFILESETWIDGET_H

