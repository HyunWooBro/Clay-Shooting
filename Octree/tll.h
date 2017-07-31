// Templated Linked List Classes    by Alan Baylis 2001

#ifndef TLLH
#define TLLH

// Forward declarations
template <class T> class Node;
template <class T> class HeadNode;
template <class T> class InternalNode;
template <class T> class TailNode;

enum {smaller, bigger, same};     // Used for the compare functions

template <class T>
class Node
{
public:
   Node() {};
   virtual ~Node() {};
   virtual Node * Insert(T * pNewObject, Node<T> * pPrevious) = 0;
   virtual void ModifyLinkPosition() = 0;
   virtual void SetPrevious(Node<T> * pPrevious) = 0;
   virtual void SetNext(Node<T> * pNext) = 0;
   virtual void DeleteList() = 0;
   virtual T * Get(int val) = 0;
   virtual void Delete(int val) = 0;
   private:
};

template <class T>
class InternalNode: public Node<T>
{
public:
  InternalNode(T * pNewObject, Node<T> * pNext, Node<T> * pPrevious);
  ~InternalNode() {}
  virtual Node<T> * Insert(T * pNewObject, Node<T> * pPrevious);
  virtual void DeleteList() {pMyNext->DeleteList(); delete pThisObject; delete this;}
  virtual void ModifyLinkPosition() {pMyNext->ModifyLinkPosition(); pThisObject->linkPosition--;}
  virtual void SetPrevious(Node<T> * pPrevious) {pMyPrevious = pPrevious;}
  virtual void SetNext(Node<T> * pNext) {pMyNext = pNext;}
  virtual T * Get(int val)
  {
    if (pThisObject->GetMyPosition() == val)
      return pThisObject;
    else
      return pMyNext->Get(val);
  }
  virtual void Delete(int val)
  {
    if (pThisObject->GetMyPosition() == val)
    {
      this->ModifyLinkPosition();
      pMyNext->SetPrevious(pMyPrevious);
      pMyPrevious->SetNext(pMyNext);
      delete this;
    }
    else
      pMyNext->Delete(val);
  }
private:
  T * pThisObject;
  Node<T> * pMyNext;
  Node<T> * pMyPrevious;
};

template <class T>
InternalNode<T>::InternalNode(T * pNewObject, Node<T> * pNext, Node<T> * pPrevious):
pThisObject(pNewObject),pMyNext(pNext),pMyPrevious(pPrevious)
{
}

template <class T>
Node<T> * InternalNode<T>::Insert(T * pNewObject, Node<T> * pPrevious)
{
  int answer = pThisObject->Compare(*pNewObject);
  switch(answer)
  {
    case same:
    case bigger:
    {
      InternalNode<T> * internal = new InternalNode<T>(pNewObject, this, pPrevious);
      pMyPrevious->SetNext(internal);
      pMyPrevious = internal;
      return internal;
    }
    case smaller:
      pMyNext = pMyNext->Insert(pNewObject, this);
    return this;
  }
  return this;
}

template <class T>
class TailNode: public Node<T>
{
public:
  TailNode(HeadNode<T> * pHead);
  ~TailNode() {}
  virtual Node<T> * Insert(T * pNewObject, Node<T> * pPrevious);
  virtual void ModifyLinkPosition() {};
  virtual void SetPrevious(Node<T> * pPrevious) {};
  virtual void SetNext(Node<T> * pNext) {};
  virtual void DeleteList() {delete this;}
//  virtual T * Get(int val) {T * pNewObject = new T(); pNewObject->linkPosition = val; pMyHead->Insert(pNewObject, pMyHead); return pNewObject;}
  virtual T * Get(int val) {return NULL;}
  virtual void Delete(int val) {};
private:
  HeadNode<T> * pMyHead;
};

template <class T>
TailNode<T>::TailNode(HeadNode<T> * pHead):
pMyHead(pHead)
{
}

template <class T>
Node<T> * TailNode<T>::Insert(T * pNewObject, Node<T> * pPrevious)
{
  InternalNode<T> * internal = new InternalNode<T>(pNewObject, this, pPrevious);
  return internal;
}

template <class T>
class HeadNode: public Node<T>
{
public:
  HeadNode();
  ~HeadNode() {};
  virtual Node<T> * Insert(T * pNewObject, Node<T> * pPrevious);
  virtual void ModifyLinkPosition() {};
  virtual void SetPrevious(Node<T> * pPrevious) {};
  virtual void SetNext(Node<T> * pNext) {pMyNext = pNext;}
  virtual void DeleteList() {pMyNext->DeleteList(); delete this;}
  virtual T * Get(int val) {return pMyNext->Get(val);}
  virtual void Delete(int val) {pMyNext->Delete(val);}
private:
  Node<T> * pMyNext;
};

template <class T>
HeadNode<T>::HeadNode()
{
  pMyNext = new TailNode<T>(this);
}

template <class T>
Node<T> * HeadNode<T>::Insert(T * pNewObject, Node<T> * pPrevious)
{
  pMyNext = pMyNext->Insert(pNewObject, pPrevious);
  return this;
}

template <class T>
class LinkedList
{
public:
  LinkedList();
  ~LinkedList() {pMyHead->DeleteList();}
  void Insert(T * pNewObject);
  T * Get(int val) {return pMyHead->Get(val);}
  void Delete(int val) {pMyHead->Delete(val);}
private:
  HeadNode<T> * pMyHead;
};

template <class T>
LinkedList<T>::LinkedList()
{
  pMyHead = new HeadNode<T>;
}

template <class T>
void LinkedList<T>::Insert(T * pNewObject)
{
  pMyHead->Insert(pNewObject, pMyHead);
}

#endif
