#include <iostream>
#include "ballClass.h"

#ifndef sequenceClass
#define sequenceClass

#include <vector>

#define SQ_MAX_BALL 7

class sequence
{
    private:
        std::vector<ball> ballSequence; // max of 6 balls
        unsigned short size = 0;

    public:
        // main constructor
        sequence();

        // vector modification
        bool insertBall(ball Ball); // return if it succeded
        bool removeBallAt(unsigned short index); // return if it succeded
        bool replaceBallAt(unsigned short index, ball newBall); // return if it succeded

        void resetSequence();

        // property get
        short getSize() const;

        std::vector<ball>& getVector();
        ball& getBallAt(unsigned short index);

        // method for the tenBot
        void play();
};

#endif
