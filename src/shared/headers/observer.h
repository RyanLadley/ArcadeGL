#include <vector>

template <class Subject>

class Observer {
 public:
   virtual void update(Subject* object) = 0;
 protected:
   virtual ~Observer() {}
};



template <class Derived, class Observer>
class Subject {
private:
   std::vector<Observer*> observers;  
   protected:
   typedef typename std::vector<Observer*>::iterator iterator;
   iterator begin() { return observers.begin(); }
   iterator end() { return observers.end(); }
   int num_of_observers() const { return observers.size(); }
public:
   void attach(Observer* o) { observers.push_back(o); }
   void detach(Observer* o) { observers.erase(o); }
   void notify(Derived *d) {
       iterator p = begin();
       while (p != end()) {
          Observer *q = *p;
          ++p;
          q->update(d);
       }
    }
};


