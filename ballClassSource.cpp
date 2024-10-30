#include <iostream>
#include <fstream>
#include <string>

#include <stdlib.h> // lib use for terminating the app

#include "ballClass.h"

using namespace std;

// set the ball config using a dataframe
void ball::setStdConfig(string dataFrame)
{
    size_t endFound = dataFrame.find_last_of('!');

    size_t find = dataFrame.find("type:");   //variable declaration in the dataframe
    size_t findEnd = dataFrame.find(',');    //end of the content of the variable
    short numInWork = 0;                     //used for testing number in the dataframe

    // --- ball type ---
    //cout << "test ball type: '" << dataFrame.substr(find, findEnd-find) << "'" << endl; //looking at the substr for debugging
    find += 5; //nb of characters in 'type:'
    if(find != string::npos) { //test if we correctly found what we needed (in this case 'type:' )
        if(dataFrame.substr(find, findEnd-find) == "vert") {    // what there declared in the dataframe variable
            ballType = vert;
         }
         else if(dataFrame.substr(find, findEnd-find) == "jaune") {
            ballType = jaune;
        }
        else if(dataFrame.substr(find, findEnd-find) == "orange") {
           ballType = orange;
       }
        else {
            cerr << "Error in the dataFrame: ball type is incorrect: '" << dataFrame.substr(find,findEnd-find) << "'" << endl;
            exit(-1313001);
        }
    }
    else {
        cerr << "Error in the dataFrame: couldn't find 'type:'" << endl;
        exit(-1313002);
    }

    // --- ball velocity ---
    find = dataFrame.find("velocity:") + 9;     // +9 because it's the number of characters in 'velocity:'
    findEnd = dataFrame.find(',', findEnd+1);   // end of the variable content

    //cout << "test ball velocity: '" << dataFrame.substr(find, findEnd-find) << "'" << endl;
    if(find != string::npos) {
        numInWork = stoi(dataFrame.substr(find, findEnd-find));
        if(numInWork >= BALL_VELOCITY_MIN && numInWork <= BALL_VELOCITY_MAX) {
            velocity = numInWork;
        }
        else {
            string errStd = (numInWork < BALL_VELOCITY_MIN) ? "too small" : "too big";
            cerr << "Error in the dataframe: the ball velocity is " << errStd << ", the limit are: " << BALL_VELOCITY_MIN << " to " << BALL_VELOCITY_MAX << endl;
            exit(-1313003);
        }
    }
    else {
        cerr << "Error in the dataframe: couldn't find 'velocity:'" << endl;
        exit(-1313004);
    }

    // --- ball effect ---
    find = dataFrame.find("effect:") + 7;
    findEnd = dataFrame.find(',', findEnd+1);

    //cout << "test ball effect: '" << dataFrame.substr(find, findEnd-find) << "'" << endl;
    if(find != string::npos) {
        numInWork = stoi(dataFrame.substr(find, findEnd-find));
        if(numInWork >= BALL_EFFECT_MIN && numInWork <= BALL_EFFECT_MAX) {
            effect = numInWork;
        }
        else {
            string errStd = (numInWork < BALL_EFFECT_MIN) ? "too small" : "too big";
            cerr << "Error in the dataframe: the ball effect is " << errStd << ", the limit are " << BALL_EFFECT_MIN << " to" << BALL_EFFECT_MAX << endl;
            exit(-1313005);
        }
    }
    else {
        cerr << "Error in the dataframe: couldn't find 'effect:'" << endl;
        exit(-1313006);
    }

    // --- pan ---
    find = dataFrame.find("pan:") + 4;
    findEnd = dataFrame.find(',', findEnd+1);

    //cout << "test ball pan: '" << dataFrame.substr(find, findEnd-find) << "'" << endl;
    if(find != string::npos) {
        numInWork = stoi(dataFrame.substr(find, findEnd-find));
        if(numInWork >= BALL_PAN_MIN && numInWork <= BALL_PAN_MAX) {
            pan = numInWork;
        }
        else {
            string errStd = (numInWork < BALL_PAN_MIN) ? "too small" : "too big";
            cerr << "Error in the dataframe: the shoot pan is " << errStd << ", the limit are " << BALL_PAN_MIN << "° to " << BALL_PAN_MAX << "°" << endl;
            exit(-1313007);
        }
    }
    else {
        cerr << "Error in the dataframe: couldn't find 'pan:'" << endl;
        exit(-1313008);
    }

    // --- tilt ---
    find = dataFrame.find("tilt:") + 5;
    findEnd = dataFrame.find(',', findEnd+1);

    //cout << "test ball tilt: '" << dataFrame.substr(find, findEnd-find) << "'" << endl;
    if(find != string::npos) {
        numInWork = stoi(dataFrame.substr(find, findEnd-find));
        if(numInWork >= BALL_TILT_MIN && numInWork <= BALL_TILT_MAX) {
            tilt = numInWork;
        }
        else {
            string errStd = (numInWork < BALL_TILT_MIN) ? "too small" : "too big";
            cerr << "Error in the dataframe: the shoot tilt is " << errStd << ", the limit are " << BALL_TILT_MIN << "° to " << BALL_TILT_MAX << "°" << endl;
            exit(-1313009);
        }
    }
    else {
        cerr << "Error in the dataframe: couldn't find 'tilt:'" << endl;
        exit(-1313010);
    }

    // --- wait ---
    find = dataFrame.find("wait:") + 5;
    // no findEnd because we already have nextFound
    //cout << "test ball wait: '" << dataFrame.substr(find, nextFound-find) << "'" << endl;

    if(find != string::npos) {
        numInWork = stoi(dataFrame.substr(find, endFound-find));
        if(numInWork >= BALL_WAIT_MIN && numInWork <= BALL_WAIT_MAX) {
            wait = numInWork;
        }
        else {
            string errStd = (numInWork < BALL_WAIT_MIN) ? "too small" : "too big";
            cerr << "Error in the dataframe the waiting for the nextBall is " << errStd << ", the limit are " << BALL_WAIT_MIN << "ms to " << BALL_WAIT_MAX << "ms" << endl;
            exit(-1313011);
        }
    }
    else {
        cerr << "Error in the dataframe: couldn't find 'wait:'" << endl;
        exit(-1313012);
    }
}

// constrcutor using a dataframe as a string
ball::ball(string dataFrame)
{
    size_t found = dataFrame.find_first_of('!');
    size_t endFound = dataFrame.find('!', found+1);

    if(found != string::npos && endFound != string::npos) {
        setStdConfig(dataFrame);
    }
    else {
        cerr << "Error in creating ball using a dataFrame, couldn't find the begin or end: invalid dataFrame" << endl;
        exit(-1313000);
    }
}

// copy constructor
ball::ball(const ball& copy)
{
    ballType = copy.getBallType();
    velocity = copy.getBallVelocity();
    effect = copy.getBallEffect();
    pan = copy.getShootPan();
    tilt = copy.getShootTilt();
    wait = copy.getWait();
}

// constrcutor using separate arguments
ball::ball(BallType ball_type, unsigned short ballVelocity, short ballEffect, short shootPan, short shootTilt, unsigned short waitForNextBall)
{
    ballType = ball_type;

    // test velocity
    if(ballVelocity >= BALL_VELOCITY_MIN && ballVelocity <= BALL_VELOCITY_MAX) {
        velocity = ballVelocity;
    }
    else {
        string errStd = (ballVelocity < BALL_VELOCITY_MIN) ? "too small" : "too big";
        cerr << "Error the ball velocity is " << errStd << ", the limit are: " << BALL_VELOCITY_MIN << " to " << BALL_VELOCITY_MAX << endl;
        exit(-13130031);
    }

    // test effect
    if(ballEffect >= BALL_EFFECT_MIN && ballEffect <= BALL_EFFECT_MAX) {
        effect = ballEffect;
    }
    else {
        string errStd = (ballEffect < BALL_EFFECT_MIN) ? "too small" : "too big";
        cerr << "Error the ball effect is " << errStd << ", the limit are " << BALL_EFFECT_MIN << " to" << BALL_EFFECT_MAX << endl;
        exit(-13130051);
    }

    // test pan
    if(shootPan >= BALL_PAN_MIN && shootPan <= BALL_PAN_MAX) {
        pan = shootPan;
    }
    else {
        string errStd = (shootPan < BALL_PAN_MIN) ? "too small" : "too big";
        cerr << "Error the shoot pan is " << errStd << ", the limit are " << BALL_PAN_MIN << "° to " << BALL_PAN_MAX << "°" << endl;
        exit(-13130071);
    }

    // test tilt
    if(shootTilt >= BALL_TILT_MIN && shootTilt <= BALL_TILT_MAX) {
        tilt = shootTilt;
    }
    else {
        string errStd = (shootTilt < BALL_TILT_MIN) ? "too small" : "too big";
        cerr << "Error in the dataframe: the shoot tilt is " << errStd << ", the limit are " << BALL_TILT_MIN << "° to " << BALL_TILT_MAX << "°" << endl;
        exit(-13130091);
    }

    // test wait
    if(waitForNextBall >= BALL_WAIT_MIN && waitForNextBall <= BALL_WAIT_MAX) {
        wait = waitForNextBall;
    }
    else {
        string errStd = (waitForNextBall < BALL_WAIT_MIN) ? "too small" : "too big";
        cerr << "Error the waiting for the nextBall is " << errStd << ", the limit are " << BALL_WAIT_MIN << "ms to " << BALL_WAIT_MAX << "ms" << endl;
        exit(-13130111);
    }
}

// --- getting ---

BallType ball::getBallType() const {return ballType;}

unsigned short ball::getBallVelocity() const {return velocity;}

short ball::getBallEffect() const {return effect;}

short ball::getShootPan() const {return pan;}

short ball::getShootTilt() const {return tilt;}

unsigned short ball::getWait() const {return wait;}
