
#ifndef ComponentEditorChoicesItem_H
#define ComponentEditorChoicesItem_H

#include "componenteditoritem.h"

#include <QList>


class ComponentEditorChoicesItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	ComponentEditorChoicesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorChoicesItem();

    virtual QFont getFont() const;

    virtual QString getTooltip() const;

    virtual QString text() const;

    virtual bool isValid() const;

    virtual ItemEditor* editor();

private:
	//! No copying
	ComponentEditorChoicesItem(const ComponentEditorChoicesItem& other);

	//! No assignment
	ComponentEditorChoicesItem& operator=(const ComponentEditorChoicesItem& other);



    QSharedPointer<Component> component_;

};

#endif // ComponentEditorChoicesItem_H
