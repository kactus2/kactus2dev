//-----------------------------------------------------------------------------
// File: VNoCollisionNoMovementLayout.inl
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.04.2025
//
// Description:
// Vertical layout. No collision and no movement allowed. Used in memory design columns.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: VNoCollisionNoMovementLayout<T>::VNoCollisionNoMovementLayout()
//-----------------------------------------------------------------------------
template <class T>
VNoCollisionNoMovementLayout<T>::VNoCollisionNoMovementLayout(qreal spacing) : spacing_(spacing)
{
}

//-----------------------------------------------------------------------------
// Function: VNoCollisionNoMovementLayout<T>::~VNoCollisionNoMovementLayout()
//-----------------------------------------------------------------------------
template <class T>
VNoCollisionNoMovementLayout<T>::~VNoCollisionNoMovementLayout()
{
}

//-----------------------------------------------------------------------------
// Function: updateItemMove()
//-----------------------------------------------------------------------------
template <class T>
void VNoCollisionNoMovementLayout<T>::updateItemMove(QList<T*>&, T*, qreal)
{
    //! Nothing to do
}

//-----------------------------------------------------------------------------
// Function: setItemPos()
//-----------------------------------------------------------------------------
template <class T>
void VNoCollisionNoMovementLayout<T>::setItemPos(QList<T*> const&, T*, qreal, qreal)
{
    //! Nothing to do
}

//-----------------------------------------------------------------------------
// Function: VNoCollisionNoMovementLayout<T>::updateItemPositions()
//-----------------------------------------------------------------------------
template <class T>
void VNoCollisionNoMovementLayout<T>::updateItemPositions(QList<T*>&, qreal, qreal)
{
    // Nothing to do.
}

