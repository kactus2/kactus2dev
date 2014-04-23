/* 
 *
 *  Created on: 14.7.2010
 *      Author: Antti Kamppi
 */

#ifndef IPXACTWIDGET_H_
#define IPXACTWIDGET_H_

#include <QWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QSharedPointer>

class IPXactModel;

/*! \brief This widget is the XML viewing tool.
 *
 */
class IPXactWidget: public QWidget {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent a pointer to the owner of this widget
	 * \param f Windowflags that set the properties of this widget.
	 */
	IPXactWidget(const QString &filePath, QWidget *parent = 0,
			Qt::WindowFlags f = 0);

	/*! \brief The destructor.
	 *
	 */
	virtual ~IPXactWidget();

	/*! \brief Sets the recommended size for this widget
	 *
	 * \return QSize that holds the info about the recommended size
	 */
	virtual QSize sizeHint() const;

private:

	/*! \brief no copying
	 *
	 */
	IPXactWidget(const IPXactWidget &other);

	/*! \brief no assignment
	 *
	 */
	IPXactWidget &operator=(const IPXactWidget &other);

	/*!
	 * \brief A pointer to the IPXactModel instance that controls the data and presents
	 * it to the view
	 */
	QSharedPointer<IPXactModel> model_;

	/*!
	 * \brief A pointer to the tree view instance that shows the data stored in the
	 * model.
	 */
	QSharedPointer<QTreeView> view_;

	/*!
	 * \brief A pointer to the layout that controls the tree view.
	 */
	QSharedPointer<QVBoxLayout> layout_;
};
#endif /* IPXACTWIDGET_H_ */
