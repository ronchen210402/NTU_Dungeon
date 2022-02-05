#ifndef PUZZLE_H
#define PUZZLE_H

#include "backpack.h"

class Puzzle: public Backpack {
    virtual void activate() {
        _active = true;
        getItemList();
        Item_Button::setInPuzzle(true);
    }
};

#endif // PUZZLE_H