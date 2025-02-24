//-----------------------------------------------------------------------------
// File: generaleditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 7.2.2011
//
// Description:
// General editor for editing the component's general data.
//-----------------------------------------------------------------------------

#ifndef GENERALEDITOR_H
#define GENERALEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <editors/common/DocumentNameGroupEditor.h>

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ParameterFinder.h>

#include <IPXACTmodels/Component/validators/ComponentValidator.h>

#include <library/LibraryErrorModel.h>

#include <QSharedPointer>
#include <QLineEdit>
#include <QSettings>
#include <QPlainTextEdit>
#include <QTableView>

class Component;
class LibraryInterface;
class Document;
class KactusAttributeEditor;
class ModeConditionParserInterface;


//-----------------------------------------------------------------------------
//! GeneralEditor class.
//-----------------------------------------------------------------------------
class GeneralEditor : public ItemEditor
{
    Q_OBJECT
       
public:
    /*!
     *  Constructor.
     *
     *    @param [in] libHandler The library handler.
     *    @param [in] component  The component model to edit.
     *    @param [in] parent     The parent widget.
	 */
	GeneralEditor(LibraryInterface* libHandler, QSharedPointer<Component> component, QWidget *parent = 0);

    /*!
     *  Destructor.
     */
    ~GeneralEditor() final = default;

    // Disable copying.
    GeneralEditor(const GeneralEditor& other) = delete;
    GeneralEditor& operator=(const GeneralEditor& other) = delete;

    /*!
     *  Check for the validity of the edited item.
     *
     *    @return True is item is valid.
     */
    virtual bool isValid() const;

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

	/*!
	 *  Signaled when the product hierarchy has changed.
	 */
	void hierarchyChanged(QSettings& settings);

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in attribute editor.
	void onAttributesChange();

	//! Handler for changes in XML header.
	void onHeaderChange();

    //! Handler for changes in author field.
    void onAuthorChange();

    //! Handler for changes in license field.
    void onLicenseChange();

private:


    //! Sets the widget layout.
    void setupLayout();

    /*!
     *  Change the extendable item in the vendor extensions editor to edited component.
     */
    void changeExtensionsEditorItem();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The VLNV displayer.
    DocumentNameGroupEditor* nameGroupEditor_;

    //! Kactus attribute editor.
    KactusAttributeEditor* attributeEditor_;

    //! The author editor.
    QLineEdit* authorEditor_;

    //! Editor for component license.
    QLineEdit* licenseEditor_;

	//! The editor to view/edit the header of the XML-file.
	QPlainTextEdit* headerEditor_;

    //! Icon for displaying validity status.
    QLabel* validityIcon_ = new QLabel(this);

    //! Textual summary of validity status.
    QLabel* validityStatus_ = new QLabel(this);

    //! View for validity error listing.
    QTableView* errorView_ = new QTableView(this);

    //! Model for validity errors.
    LibraryErrorModel* errorModel_ = new LibraryErrorModel(this);

    //! Parameter finder for expressin parser.
    QSharedPointer<ParameterFinder> finder_;

    //! Expression parser for validator.
    QSharedPointer<ExpressionParser> parser_;

    //! The mode condition parser factory to use in the component validator.
    QSharedPointer<ModeConditionParserInterface> modeConditionParserInterface_;

    //! Validator for checking the component compliance.
    QSharedPointer<ComponentValidator> validator_;

    //! The available library.
    LibraryInterface* library_ = nullptr;
};

//-----------------------------------------------------------------------------

#endif // GENERALEDITOR_H
