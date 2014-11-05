

#ifndef ChoicesEditor_H
#define ChoicesEditor_H


#include <editors/ComponentEditor/itemeditor.h>

#include <IPXACTmodels/component.h>


#include <QSortFilterProxyModel>
#include <QSharedPointer>

class EditableTableView;
class ChoicesModel;

class ChoicesEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	ChoicesEditor(QSharedPointer<Component> component,
				QWidget* parent = 0);
   


	//! \brief The destructor
	virtual ~ChoicesEditor();

    virtual bool isValid() const;

    virtual void refresh();
protected:

    void showEvent( QShowEvent* event );

private:

    //! No copying
    ChoicesEditor(const ChoicesEditor& other);
    ChoicesEditor& operator=(const ChoicesEditor& other);

    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    EditableTableView* view_;

    ChoicesModel* model_;
};

#endif // ChoicesEditor_H
