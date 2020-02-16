#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

//    GraphObject(int imageID, int startX, int startY,
//                DIRECTION startDirection, float size = 1.0,
//                unsigned int depth = 0);
//    void setVisible(bool shouldIDisplay);
//    void getX() const;
//    void getY() const;
//    void moveTo(int x, int y);
//    DIRECTION getDirection() const; // Directions: up, down, left, right
//    void setDirection(DIRECTION d); // Directions: up, down, left, right

class World;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class Actor: public GraphObject{
public:
    // constructor
    Actor(World* world, int imageID, int startX, int startY, GraphObject::Direction startDir, float size, unsigned int depth);
    
    // destructor
    virtual ~Actor();
    
    // check current state
    bool isAlive() const;
    void setDead();
    
    // take in a direction, get the deltax and deltay needed
    void getNextStep(GraphObject::Direction dir, int& dx, int& dy) const;
    
    // get absolute distance between another Actor
    double getDistance(const Actor* other) const;
    
    // get a opposite direction
    GraphObject::Direction getOpposite(GraphObject::Direction dir);
    bool facingPlayer();
    
    // if this actor overlaps another actor
    bool overlaps(Actor* other) const;
    
    // annoy
    virtual void annoy(int howmuch);
    
    // become rich
    virtual void becomeRich();
    
    // can this Actor dig through earth?
    virtual bool canDig() const = 0;
    
    // distinguish items and actors
    virtual bool isActor() const = 0;
    
    // cango
    bool canGo(GraphObject::Direction dir);
    
    // everything doSomething every tick
    virtual void doSomething() = 0;
    
    // get size
    float entitySize() const;
    
    // getworld
    World* getWorld();
    
private:
    // worldptr
    World* worldptr;
    // current state
    bool m_alive;
    // position
    float m_size;
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class Earth: public Actor{
public:
    Earth(World* world, int x, int y);
    bool canDig() const;
    bool isActor() const;
    void doSomething();
    ~Earth();
};

class Dummy: public Actor{
public:
    Dummy(World* world, int x, int y, GraphObject::Direction startdir)
    : Actor(world, TID_GOLD, x, y, startdir, 1, 0)
    {
        setVisible(false);
    }
    void annoy(int howmuch){
         
    }
    bool canDig() const{
        return false;
    }
    bool isActor() const{
        return false;
    }
    
    void doSomething(){
        
    }
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class Item: public Actor{
public:
    Item(World* world, int imageID, int startX, int startY, GraphObject::Direction startDir, float size, unsigned int depth);
    void annoy(int howmuch);
    bool canDig() const;
    virtual void doSomething() = 0;
    bool isActor() const;
    virtual ~Item();
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class Pickupable: public Item{
public:
    Pickupable(World* world, int imageID, int startX, int startY);
    virtual void doSomething();
    virtual ~Pickupable();
};

class PGold: public Pickupable{
public:
    PGold(World* world, int x, int y);
    void doSomething();
    ~PGold();
};

class Sonar: public Pickupable{
public:
    Sonar(World* world, int x, int y);
    void doSomething();
    ~Sonar();
private:
    int ticksRemain;
};

class WaterPool: public Pickupable{
public:
    WaterPool(World* world, int x, int y);
    void doSomething();
    ~WaterPool();
private:
    int ticksRemain;
};

class Barrel: public Pickupable{
public:
    Barrel(World* world, int x, int y);
    void doSomething();
    ~Barrel();
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// other special items

class Boulder: public Item{
public:
    Boulder(World* world, int x, int y);
    void doSomething();
    ~Boulder();
private:
    bool falling;
    int waiting;
};

class DGold: public Item{ // gold for protesters
public:
    DGold(World* world, int x, int y);
    void doSomething();
    ~DGold();
private:
    int lifetime;
};

class Squirt: public Item{
public:
    Squirt(World* world, int x, int y, GraphObject::Direction startdir);
    
    void doSomething();
    ~Squirt();
private:
    int distanceRemain;
    void checkNextStep();
};

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

class Person: public Actor{
public:
    Person(World* world, int imageID, int startX, int startY,
          GraphObject::Direction startDirection, int hp);
    bool isActor() const;
    bool canDig() const = 0;
    // annoy and get hp
    void annoy(int howmuch);
    int getHP() const;
    // dosomething
    virtual void doSomething() = 0;
    virtual ~Person();
private:
    int hp;
};


class Protester: public Person{
public:
    Protester(World* world, int imageID, int startX, int startY, int hp);
    bool canDig() const;
    virtual void doSomething() = 0;
    void annoy(int howmuch);
    virtual void becomeRich() = 0;
    virtual ~Protester();
    
protected:
    // stun the protester
    void stun();
    
    // get the direction if a protester wants to go to this x,y point
    GraphObject::Direction dirTo(int x, int y, int& moves, int maxStep);
    
    // check on which direction can protester see player (none if can't see)
    GraphObject::Direction canSeePlayer();
    
    // get the leaving status of this protester
    bool& leavingStatus();
    // get the timers for its action
    int& restingTime();
    int& shoutResting();
    int& numSquares();
    int& mazeResting();
    
    // update this upcoming turn by changing the lastperpturn timer
    void upDateLastPerp(GraphObject::Direction d);
    
    // how long since last perpendicular turn
    int getLastPerp();
    
private:
    // state if it is leaving the field
    bool leaveField;
    
    // resting ticks
    int restingTimer;
    // maze resting
    int mazeRest;
    // shout resting ticks
    int shoutRestingTimer;
    // num Squares Remain Current Direction
    int numSquaresRemain;
    // havent turn perp
    int lastperpturn;
};

class Regular: public Protester{
public:
    Regular(World* world, int x, int y);
    ~Regular();
    void becomeRich();
    void doSomething();
    // 1 check alive
    // 2 if resting
    //   move and update
    // 3 elseif leave the field
    //   search way to exit, and setdead after
    // 4 shout to tunnelman
    // 5 move its position
};

class HardCore: public Protester{
public:
    HardCore(World* world, int x, int y);
    ~HardCore();
    void becomeRich();
    void doSomething();
};


class TunnelMan: public Person{
public:
    TunnelMan(World* world);
    void doSomething();
    void annoy(int howmuch);
    bool canDig() const;
    ~TunnelMan();
    
    // get item
    int& curGold();
    int& curWater();
    int& curSonar();
    
private:
    int gold;
    int water;
    int sonar;
};

#endif // ACTOR_H_
