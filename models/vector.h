/* 
 *
 *  Created on: 18.10.2010
 *      Author: Antti Kamppi
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QString>
#include <QMap>

/*! \brief Equals the spirit:vector element in IP-Xact specification.
 *
 * Determines the left and right vector bounds and attributes for them
 */
class Vector {

public:

	/*! \brief The constructor
	 *
	 * \param vectorNode A reference to a QDomNode to parse the information
	 * from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Vector(QDomNode &vectorNode);

    /*!
     *  Copy constructor.
     */
    Vector(Vector const& other);

	/*! \brief The constructor
	 *
	 * \param left The left bound of vector.
	 * \param right The right bound of vector.
	 *
	*/
	Vector(int left, int right);

	/*! \brief The default constructor
	 *
	 * Constructs an empty vector which is not valid.
	*/
	Vector();

	//! \brief Assignment operator
	Vector& operator=(const Vector& other);

	//! \brief The destructor
	virtual ~Vector();

	/*! \brief Write the contents of the class using the writer.
	 *
	 * Uses the specified writer to write the class contents into file as valid
	 * IP-Xact.
	 *
	 * \param writer A reference to a QXmlStreamWriter instance that is used to
	 * write the document into file.
	 *
	 * Exception guarantee: basic
	 * \exception Write_error Occurs if class or one of it's member classes is
	 * not valid IP-Xact in the moment of writing.
	 */
	void write(QXmlStreamWriter& writer);

	/*! \brief Get the left bound of the vector
	 *
	 * \return The location of the leftmost bit in the vector.
	 */
	int getLeft() const;

	/*! \brief Get the right bound of the vector.
	 *
	 * \return The location of the rightmost bit in the vector.
	 */
	int getRight() const;

	/*! \brief Get the attributes for the left element.
	 *
	 * \return A reference to a QMap containing attributes.
	 */
	const QMap<QString, QString>& getLeftAttributes();

	/*! \rief Get the attributes for the right element.
	 *
	 * \return A reference to a QMap containing the attributes.
	 */
	const QMap<QString, QString>& getRightAttributes();

	/*! \brief Check if the vector is valid or not.
	 *
	 * \return True if vector is in valid state.
	*/
	bool isValid() const;

	/*! \brief Set the left bound of the vector.
	 *
	 * \param left The left bound.
	 *
	*/
	void setLeft(int left);

	/*! \brief Set the right bound of the vector
	 *
	 * \param right The right bound.
	 *
	*/
	void setRight(int right);

private:

	/*!
	 * MANDATORY
	 * Specifies the left bound of the vector
	 */
	int left_;

	/*!
	 * OPTIONAL
	 * The attributes for the left bound
	 */
	QMap<QString, QString> leftAttributes_;

	/*!
	 * MANDATORY
	 * Specifies the right bound of the vector
	 */
	int right_;

	/*!
	 * OPTIONAL
	 * The attributes for the right bound
	 */
	QMap<QString, QString> rightAttributes_;
};

#endif /* VECTOR_H_ */
