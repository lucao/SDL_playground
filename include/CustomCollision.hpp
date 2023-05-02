#ifndef CUSTOM_COLLISION_H
#define CUSTOM_COLLISION_H

template <class T>
class Collider {
 public:
   virtual bool colideWith(T* collider);
};

#endif