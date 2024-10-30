#include <iostream>
#include <vector>
#include <stdlib.h>

#include "sequenceClass.h"

using namespace std;

// constructor
sequence::sequence()
{
    size = 0;
    //ballSequence.resize(SQ_MAX_BALL);
    ballSequence.shrink_to_fit();
}

bool sequence::insertBall(ball Ball)
{
    if(size <= SQ_MAX_BALL - 1) { // check if we can insert a ball
        ballSequence.emplace(ballSequence.begin()+size, Ball);
        size++;
        return true;
    }
    else {
        cerr << "Error cannot insert ball because the sequence is too big there already " + to_string(size) + " balls" << endl;
        return false;
    }
}

bool sequence::removeBallAt(unsigned short index)
{
    if(index >= 1 && index <= SQ_MAX_BALL) {
        if(index < size) { // check if the index isn't too big compare to the size of the sequence
            ballSequence.erase(ballSequence.begin()+index);
            size--;
            return true;
        }
        else {
            cerr << "Error cannot erase ball because the index is too big compare to the sequence size" << endl;
            return false;
        }
    }
    else {
        string errStd = (index < 1) ? "too small" : "too big";
        cerr << "Error cannot erase ball because the index is " << errStd << endl;
        return false;
    }
}

bool sequence::replaceBallAt(unsigned short index, ball newBall)
{
    if(index >= 1 && index <= SQ_MAX_BALL) {
        if(index < size) { // check if the index isn't too big compare to the size of the sequence
            ballSequence.erase(ballSequence.begin()+index);
            ballSequence.emplace(ballSequence.begin()+index, newBall);
            return true;
        }
        else {
            cerr << "Error cannot replace ball because the index is too big compare to the sequence size" << endl;
            return false;
        }
    }
    else {
        string errStd = (index < 1) ? "too small" : "too big";
        cerr << "Error replace ball because the index is " << errStd << endl;
        return false;
    }
}

void sequence::resetSequence()
{
    for(unsigned short i=0; i<ballSequence.size(); i++) {
        ballSequence.erase(ballSequence.begin()+i);
    }
    size = 0;
}

short sequence::getSize() const { return size;}

vector<ball>& sequence::getVector() { return ballSequence; }

ball& sequence::getBallAt(unsigned short index)
{
    if(index < size) { // check if the index isn't too big compare to the size of the sequence
        return ballSequence.at(index);
    }
    else {
        string errStd = (index == size) ? "(the index cannot be equal to the amount of ball): " : ": ";
        cerr << "Error in sequence trying to return ball at an index strictly inferior to the number of ball" << errStd;
        cerr << "index= " << to_string(index) << " ; size= " << to_string(size) << endl;
        exit(-1313101);
    }
}


void sequence::play()
{
    //well... will see
}
