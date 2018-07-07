#ifndef STYLES_MIX_H
#define STYLES_MIX_H

// Usage: Mix<A, B, F>
// Mix between A and B using function F
// A, B: COLOR
// F: INTEGER
// return value: COLOR
//
// F = 0 -> return A
// F = 8192 -> return (A+B)/2
// F = 16384 = return B
template<class A, class B, class F>
class Mix {
public:
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
    f_.run(blade);
  }
  OverDriveColor getColor(int led) {
    OverDriveColor a = a_.getColor(led);
    OverDriveColor b = b_.getColor(led);
    a.c = a.c.mix(b.c, f_.getInteger(led));
    return a;
  }
private:
  A a_;
  B b_;
  F f_;
};

#endif
