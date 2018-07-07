#ifndef STYLES_STYLE_PTR_H
#define STYLES_STYLE_PTR_H

#include "blade_style.h"

// Usage: StylePtr<BLADE>
// BLADE: COLOR
// return value: suitable for preset array
// Most blade styls are created by taking a blade style template and wrapping it
// this class, which implements the BladeStyle interface. We do this so that the
// getColor calls will be inlined in this loop for speed.

template<class T>
class Style : public BladeStyle {
public:
  void activate() override { }
  void run(BladeBase* blade) override {
    base_.run(blade);
    int num_leds = blade->num_leds();
    for (int i = 0; i < num_leds; i++) {
      OverDriveColor c = base_.getColor(i);
      if (c.overdrive) {
         blade->set_overdrive(i, c.c);
      } else {
         blade->set(i, c.c);
      }
    }
  }
private:
  T base_;
};

// Get a pointer to class.
template<class STYLE>
StyleAllocator StylePtr() {
  static StyleFactoryImpl<Style<STYLE> > factory;
  return &factory;
};


#endif
