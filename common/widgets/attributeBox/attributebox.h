/* 
 *
 *  Created on: 15.1.2011
 *      Author: Antti Kamppi
 */

#ifndef ATTRIBUTEBOX_H_
#define ATTRIBUTEBOX_H_

#include "attributemodel.h"

#include <QGroupBox>
#include <QAbstractTableModel>
#include <QString>
#include <QList>
#include <QMap>
#include <QTableView>

/*! \brief AttributeBox provides services to manage attributes for IP-Xact.
 *
 * The class contains necessary GUI items to add/edit/remove attributes for
 * a certain IP-Xact element.
 */
class AttributeBox: public QGroupBox {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 */
	AttributeBox(QWidget* parent = 0, const QString title = tr("Attributes"));

	//! \brief The destructor
	virtual ~AttributeBox();

	/*! \brief Get the attributes from the widget.
	 *
	 * \param QMap containing the name-value pairs of the attributes.
	 *  Key = Name of the attribute.
	 *  Value = Value of the attribute.
	 */
	const QMap<QString, QString> getAttributes() const;

	/*! \brief Check that attributeBox contains only non-empty name-value pairs.
	 *
	 * \return True if everything is ok.
	 */
	bool isValid() const;

	/*! \brief Set the attributes to be displayed in this widget.
	 *
	 * This function should be used after constructing this widget to display
	 * the existing attributes so user can edit/remove them.
	 */
	void setAttributes(const QMap<QString, QString>& attributes);

	/*! \brief Clear all attributes from the box
	 *
	*/
	void clear();

	/*! \brief Does the attribute box contain any attributes.
	 *
	 *
	 * \return True if there are no attributes specified
	*/
	bool isEmpty() const;

public slots:

	//! \brief Called when add is pressed in the GUI
	void onAdd();

	//! \brief Called when remove is pressed in the GUI
	void onRemove();

signals:

	//! \brief Emitted when contents of the widget change.
	void contentChanged();

private:

	//! No copying
	AttributeBox(const AttributeBox& other);

	//! No assignment
	AttributeBox& operator=(const AttributeBox& other);

	//! \brief Model that contains the attributes to display and edit.
	AttributeModel model_;

	//! \brief View that displays the attributes stored in the model.
	QTableView view_;
};

#endif /* ATTRIBUTEBOX_H_ */
