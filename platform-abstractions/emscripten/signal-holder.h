#ifndef DALI_SIGNAL_HOLDER_H
#define DALI_SIGNAL_HOLDER_H

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

emscripten::val JavascriptValue( const Dali::Property::Value& v );

struct BaseSignalSlot : public Dali::ConnectionTracker
{
};

class SignalHolder : public Dali::ConnectionTracker
{
public:
  typedef Dali::Vector<BaseSignalSlot*> Slots;
  Slots mSlots;
  SignalHolder() {}

  ~SignalHolder()
  {
    for(Slots::Iterator iter = mSlots.Begin(); iter != mSlots.End(); ++iter)
    {
      delete *iter;
    }
  }
  void add(BaseSignalSlot* s) { mSlots.PushBack(s); }

private:
  SignalHolder(const SignalHolder& nocopy);
  SignalHolder& operator=(const SignalHolder& noassign);
};


}; // Emscripten
}; // Internal
}; // Dali


#endif // header
