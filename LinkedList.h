#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class LinkedList
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

  struct Node
  {
      value_type data;
      Node* prev;
      Node* next;

      Node(): prev(nullptr), next(nullptr)
      {}
      Node(const value_type& input)
      {
          prev = nullptr;
          next = nullptr;
          data = input;
      }
  };
  Node* head;
  Node* tail;
  int list_size;

  LinkedList()
  {
      head = nullptr;
      tail = nullptr;
      list_size = 0;
  }

  LinkedList(std::initializer_list<Type> l): LinkedList()
  {
    for(auto i = l.begin(); i != l.end(); i++)
        append(*i);
  }

  LinkedList(const LinkedList& other): LinkedList()
  {
    for(auto i = other.begin(); i != other.end(); i++)
        append(*i);
  }

  LinkedList(LinkedList&& other): LinkedList()
  {
    std::swap(list_size, other.list_size);
    std::swap(head, other.head);
    std::swap(tail, other.tail);
  }

  ~LinkedList()
  {
      while(list_size)
        popFirst();
      delete tail;
  }

  LinkedList& operator=(const LinkedList& other)
  {
    if(&other == this)
        return *this;
    erase(begin(), end());
    delete tail;
    head = tail = nullptr;
    list_size = 0;
    for(auto i = other.begin(); i != other.end(); i++)
        append(*i);
    return *this;
  }

  LinkedList& operator=(LinkedList&& other)
  {
    std::swap(list_size, other.list_size);
    std::swap(tail, other.tail);
    std::swap(head, other.head);
    other.erase(other.begin(), other.end());
    return *this;
  }

  bool isEmpty() const
  {
    return (list_size == 0);
  }

  size_type getSize() const
  {
    return list_size;
  }

  void append(const Type& item)
  {
    Node* tmp = new Node(item);
    if(list_size == 0)
    {
        head = tmp;
        tail = new Node();
        head->next = tail;
        tail->prev = head;
    }
    else
    {
        (tail->prev)->next = tmp;
        tmp->prev = tail->prev;
        tmp->next = tail;
        tail->prev = tmp;
    }
    list_size++;
  }

  void prepend(const Type& item)
  {
    Node* tmp = new Node(item);
    if(list_size == 0)
    {
        head = tmp;
        tail = new Node();
        head->next = tail;
        tail->prev = head;
    }
    else
    {
        tmp->next=head;
        head->prev=tmp;
        head = tmp;
    }
    list_size++;
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    if(list_size == 0 || insertPosition == begin())
        prepend(item);
    else
    {
        Node* tmp = new Node(item);
        (insertPosition.it->prev)->next = tmp;
        tmp->prev = (insertPosition.it->prev);
        tmp->next = insertPosition.it;
        insertPosition.it->prev = tmp;
        list_size++;
    }
  }

  Type popFirst()
  {
    if(list_size == 0)
        throw std::logic_error("Empty list");
    value_type tmp = head->data;
    erase(begin());
    return tmp;
  }

  Type popLast()
  {
    if(list_size == 0)
        throw std::logic_error("Empty list");
    value_type tmp = (tail->prev)->data;
    erase(--end());
    return tmp;
  }

  void erase(const const_iterator& possition)
  {
    if(list_size == 0)
        throw std::out_of_range("Erasing empty list");
    if(possition == end())
        throw std::out_of_range("Erasing sentinel");
    if(possition == cbegin())
    {
        head = possition.it->next;
    }
    else
    {
        (possition.it->prev)->next = possition.it->next;
        (possition.it->next)->prev = possition.it->prev;
    }
    delete possition.it;
    list_size--;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    for(auto i = firstIncluded; i != lastExcluded; i++)
        erase(i);
  }

  iterator begin()
  {
    if(list_size == 0)
        return Iterator(ConstIterator(tail));
    return Iterator(ConstIterator(head));
  }

  iterator end()
  {
    return Iterator(ConstIterator(tail));
  }

  const_iterator cbegin() const
  {
    if(list_size == 0)
        return ConstIterator(tail);
    return ConstIterator(head);
  }

  const_iterator cend() const
  {
    return ConstIterator(tail);
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
class LinkedList<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename LinkedList::value_type;
  using difference_type = typename LinkedList::difference_type;
  using pointer = typename LinkedList::const_pointer;
  using reference = typename LinkedList::const_reference;

  Node* it;

  explicit ConstIterator(): it(nullptr)
  {}

  ConstIterator(Node* ptr): it(ptr)
  {}

  reference operator*() const
  {
    if(it != nullptr && it->next != nullptr)
        return it->data;
    else
        throw std::out_of_range("Iterator out of range");
  }

  ConstIterator& operator++()
  {
    if(it == nullptr || it->next == nullptr)
        throw std::out_of_range("Iterator out of range");
    it = it->next;
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator tmp = *this;
    if(it == nullptr || it->next == nullptr)
        throw std::out_of_range("Iterator out of range");
    it = it->next;
    return tmp;
  }

  ConstIterator& operator--()
  {
    if(it == nullptr || it->prev == nullptr)
        throw std::out_of_range("Iterator out of range");
    it = it->prev;
    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator tmp = *this;
    if(it == nullptr || it->prev == nullptr)
        throw std::out_of_range("Iterator out of range");
    it = it->prev;
    return tmp;
  }

  ConstIterator operator+(difference_type d) const
  {
    ConstIterator tmp = *this;
    for(auto i = d; i != 0; i--)
    {
        if(tmp.it->next != nullptr)\
            tmp++;
        else
            throw std::out_of_range("Iterator out of range");
    }
    return tmp;
  }

  ConstIterator operator-(difference_type d) const
  {
    ConstIterator tmp = *this;
    for(auto i = d; i != 0; i--)
    {
        if(tmp.it->prev != nullptr)\
            tmp--;
        else
            throw std::out_of_range("Iterator out of range");
    }
    return tmp;
  }

  bool operator==(const ConstIterator& other) const
  {
    return it == other.it;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return it != other.it;
  }
};

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
public:
  using pointer = typename LinkedList::pointer;
  using reference = typename LinkedList::reference;

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

#endif // AISDI_LINEAR_LINKEDLIST_H
