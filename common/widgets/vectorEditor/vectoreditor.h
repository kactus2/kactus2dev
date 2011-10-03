/* 
 *
 *  Created on: 12.4.2011
 *      Author: Antti Kamppi
 * 		filename: vectoreditor.h
 */

#ifndef VECTOREDITOR_H
#define VECTOREDITOR_H

#include <QGroupBox>
#include <QSharedPointer>
#include <QString>
#include <QCheckBox>
#include <QSpinBox>

class Vector;

/*! \brief The editor containing the GUI items to edit a vector-model.
 *
 */
class VectorEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param title Title for the group box.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	VectorEditor(const QString& title,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~VectorEditor();

	/*! \brief Set the vector being edited.
	 *
	 * \param vector Pointer to the vector to edit.
	 *
	*/
	void setVector(QSharedPointer<Vector> vector);

	/*! \brief Is the vector element enabled or disabled.
	 *
	 * \return True if the vector is enabled.
	*/
	bool isEnabled() const;

	/*! \brief Is the editor in valid state.
	 *
	 * \return True if editor is in valid state.
	*/
	bool isValid() const;

	/*! \brief Restore the changes from the vector to editor.
	 *
	*/
	void restoreChanges();

	/*! \brief Apply the changes from the editor to model and get pointer to the vector.
	 *
	 * \return QSharedPointer<Vector> That points to the vector that was edited.
	*/
	QSharedPointer<Vector> applyChanges();

	/*! \brief Set the maximum left and right boundary for vector.
	 *
	 * Because left bound can't be larger than right bound the both max values 
	 * are set.
	 * 
	 * \param max The max value.
	*/
	void setLeftMax(int max);

	/*! \brief Set the maximum right boundary for vector.
	 *
	 * \param max The max value.
	 *
	*/
	void setRightMax(int max);

	/*! \brief Clear the editor and clear the vector pointer.
	 *
	*/
	void clear();

	/*! \brief Get the width of the vector.
	 *
	 * \return The number of bits in the vector (left - right).
	*/
	int getWidth() const;

private slots:

	/*! \brief Enables/disables the vector boundary editors.
	 *
	 * \param enabled If true the boundary editors are enabled. If false they are
	 * disabled.
	 *
	*/
	void onToggled(bool enabled);

signals:

	//! \brief Signal is emitted when the contents of the editor change.
	void contentChanged();

private:

	//! \brief No copying
	VectorEditor(const VectorEditor& other);

	//! No assignment
	VectorEditor& operator=(const VectorEditor& other);
	
	//! \brief Pointer to the vector being edited.
	QSharedPointer<Vector> vector_;

	//! \brief Check box to enable/disable the vector bounds
	QCheckBox enableBox_;

	//! \brief Editor to set the left boundary
	QSpinBox left_;

	//! \brief Editor to set the right boundary
	QSpinBox right_;
};

#endif // VECTOREDITOR_H
