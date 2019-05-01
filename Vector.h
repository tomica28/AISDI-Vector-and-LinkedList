#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class Vector
{
public:
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using const_pointer = const Type*;
  using const_reference = const Type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

  friend class ConstIterator;
  friend class Iterator;


  pointer ptr;
  size_type number_elem;
  size_type vector_size;



  Vector(): number_elem(0), vector_size(4)
  {
    ptr = new value_type[vector_size];
  }

  Vector(std::initializer_list<Type> l): number_elem(0)
  {
    for(this->vector_size = 4; vector_size < l.size(); vector_size = 2*vector_size)
        continue;
    ptr = new value_type[vector_size];
    for(auto i = l.begin(); i!=l.end(); ++i)
        append(*i);
  }

  Vector(const Vector& other): number_elem(0), vector_size(other.vector_size)
  {
    ptr = new value_type[vector_size];
    for(auto i = other.begin(); i != other.end(); ++i)
        append(*i);
  }

  Vector(Vector&& other): ptr(nullptr), number_elem(0), vector_size(0)
  {
    std::swap(ptr,other.ptr);
    std::swap(number_elem,other.number_elem);
    std::swap(vector_size,other.vector_size);
  }

  ~Vector()
  {
    if(ptr != nullptr)
        delete[] ptr;
  }

  void change(size_type it, const value_type& data)
  {
      Type* tmp = new value_type [vector_size];
      for(size_type i = 0; i < it; ++i)
            tmp[i] = ptr[i];
      tmp[it] = data;
      for(size_type i = it; i < number_elem; ++i)
            tmp[i + 1] = ptr[i];
      std::swap(tmp, ptr);
      delete[] tmp;
      ++number_elem;
  }

  Vector& operator=(const Vector& other)
  {
    if(this != &other)
    {
        delete[] ptr;
        ptr = new value_type[other.vector_size];
        number_elem = 0;
        for(auto it = other.begin(); it != other.end(); ++it)
            append(*it);
    }
    return *this;
  }

  Vector& operator=(Vector&& other)
  {
    if(this != &other)
    {
        std::swap(ptr,other.ptr);
        std::swap(number_elem,other.number_elem);
        std::swap(vector_size,other.vector_size);
        delete[] other.ptr;
        other.ptr = nullptr;
        other.number_elem = 0;
        other.vector_size = 0;
    }
    return *this;
  }

  bool isEmpty() const
  {
    return number_elem == 0;
  }

  size_type getSize() const
  {
    return number_elem;
  }

  void append(const Type& item)
  {
    if(number_elem == vector_size)
    {
        vector_size *= 2;
        Type* tmp = new value_type[vector_size];
        for(size_type i = 0; i < number_elem; ++i)
            tmp[i]=ptr[i];
        std::swap(ptr, tmp);
        delete[] tmp;
    }
    ptr[number_elem]= item;
    ++number_elem;
  }

  void prepend(const Type& item)
  {
    if(number_elem == 0)
        append(item);
    else if(number_elem == vector_size)
    {
        vector_size *= 2;
        change(0, item);
    }
    else
    {
        change(0, item);
    }
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    if(number_elem == vector_size)
    {
        vector_size *= 2;
        change(insertPosition.it, item);
    }
    change(insertPosition.it, item);
  }

  Type popFirst()
  {
    if(number_elem == 0)
        throw std::out_of_range("Empty vector");
    Type tmp = ptr[0];
    erase(begin());
    return tmp;
  }

  Type popLast()
  {
    if(number_elem == 0)
        throw std::out_of_range("Empty vector");
    --number_elem;
    return ptr[number_elem];
  }

  void erase(const const_iterator& possition)
  {
    if(number_elem == 0)
        throw std::out_of_range("Empty vector");
    if(possition == end())
        throw std::out_of_range("Iterator out of range");
    for(auto i  = possition.it; i != number_elem-1; ++i)
        ptr[i] = ptr[i+1];
    --number_elem;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    if(firstIncluded == end())
        throw std::out_of_range("Iterator out of range");
    size_type diffrence = lastExcluded.it - firstIncluded.it;
    if(diffrence <= 0)
        return;
    for(auto i = firstIncluded.it; i + diffrence != number_elem; ++i)
        ptr[i] = ptr[i + diffrence];
    number_elem -= diffrence;
  }

  iterator begin()
  {
    return Iterator(ConstIterator(this,0));
  }

  iterator end()
  {
    return Iterator(ConstIterator(this, number_elem));
  }

  const_iterator cbegin() const
  {
    return ConstIterator(this,0);
  }

  const_iterator cend() const
  {
    return ConstIterator(this, number_elem);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename Type>
class Vector<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;


    const Vector* vector_ptr;
    size_type it;


  explicit ConstIterator(): vector_ptr(nullptr), it(0)
  {}

  ConstIterator(const Vector* vect, size_type i): vector_ptr(vect), it(i)
    {}

  reference operator*() const
  {
    if(it == vector_ptr->number_elem)
        throw std::out_of_range("Iterator out of range");
    return vector_ptr->ptr[it];
  }

  ConstIterator& operator++()
  {
    if(it == vector_ptr->number_elem)
        throw std::out_of_range("Iterator out of range");
    ++it;
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator tmp(vector_ptr, it);
    if(it == vector_ptr->number_elem)
        throw std::out_of_range("Iterator out of range");
    ++it;
    return tmp;
  }

  ConstIterator& operator--()
  {
    if(it == 0)
        throw std::out_of_range("Iterator out of range");
    --it;
    return *this;
  }

  ConstIterator operator--(int)
  {
    if(it == 0)
        throw std::out_of_range("Iterator out of range");
    ConstIterator tmp(vector_ptr, it);
    --it;
    return tmp;
  }

  ConstIterator operator+(difference_type d) const
  {
    if(it + d > vector_ptr->number_elem)
        throw std::out_of_range("Iterator out of range");
    ConstIterator tmp(vector_ptr, it+d);
    return tmp;
  }

  ConstIterator operator-(difference_type d) const
  {
    if(it - d < 0)
        throw std::out_of_range("Iterator out of range");
    ConstIterator tmp(vector_ptr, it-d);
    return tmp;
  }

  bool operator==(const ConstIterator& other) const
  {
    return ((vector_ptr == other.vector_ptr) && (it == other.it));
  }

  bool operator!=(const ConstIterator& other) const
  {
    return ((vector_ptr != other.vector_ptr) || (it != other.it));
  }
};

template <typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;

  explicit Iterator()
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  Iterator operator+(difference_type d) const
  {
    return ConstIterator::operator+(d);
  }

  Iterator operator-(difference_type d) const
  {
    return ConstIterator::operator-(d);
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif // AISDI_LINEAR_VECTOR_H
