#include <iostream>
#include <fstream>

#ifndef ballClass
#define ballClass

#define BALL_VELOCITY_MIN 0
#define BALL_VELOCITY_MAX 175
#define BALL_EFFECT_MIN -5
#define BALL_EFFECT_MAX 5
#define BALL_PAN_MIN -2
#define BALL_PAN_MAX 2
#define BALL_TILT_MIN -4
#define BALL_TILT_MAX 4
#define BALL_WAIT_MIN 2000
#define BALL_WAIT_MAX 10000

//new type needed for the tenis ball
enum BallType {jaune, vert, orange};

class ball  {
    private:
        //needed settings
        BallType ballType = BallType::jaune; //type de balle
        unsigned short velocity = 127; // force de la balle (0 a 255)
        short effect = 0; //equivalent au potentiometre "spin", 0 = aucun effect
        short pan = 0; //pan en degree (-9° a 9°) (yaw)
        short tilt = 0; //tilt en degree (-25° a 25°) (pitch)
        unsigned short wait = 5000; //temp d'attende(ms) pour lancé la prochaine boule (2 000ms à 10 000ms)

    public:
        // main constructor using a dataframe
        ball(std::string dataFrame);
        // copy constructor
        ball(const ball& copy);
        // constructor without a dataframe
        ball(BallType ball_type,
             unsigned short ballVelocity,
             short ballEffect,
             short shootPan,
             short shootTilt,
             unsigned short waitForNextBall);

        // property get
        BallType getBallType() const;
        unsigned short getBallVelocity() const;
        short getBallEffect() const;
        short getShootPan() const;
        short getShootTilt() const;
        unsigned short getWait() const;

    private:
        void setStdConfig(std::string dataFrame);

};

#endif
