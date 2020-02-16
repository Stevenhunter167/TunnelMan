#include "Actor.h"
#include "World.h"
#include <queue>

//#define DEBUGMODEON

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// class Actor
// constructor
Actor::Actor(World* world, int imageID, int startX, int startY, GraphObject::Direction startDir, float size, unsigned int depth)
: GraphObject(imageID, startX, startY, startDir, size, depth), worldptr(world), m_alive(true), m_size(size){
    
}

// destructor
Actor::~Actor(){
    
}

// get current state
bool Actor::isAlive() const{
    return m_alive;
}

// set current state
void Actor::setDead(){
    m_alive = false;
}

// get possible next position
void Actor::getNextStep(GraphObject::Direction dir, int &dx, int &dy) const{
    switch (dir) {
        case GraphObject::up:
            dx = 0;
            dy = 1;
            break;
        case GraphObject::down:
            dx = 0;
            dy = -1;
            break;
        case GraphObject::left:
            dx = -1;
            dy = 0;
            break;
        case GraphObject::right:
            dx = 1;
            dy = 0;
            break;
        case GraphObject::none:
            dx = dy = 0;
            break;
    }
}

// get distance between Actor self and other
double Actor::getDistance(const Actor* other) const{
    double deltax = (getX() - other->getX());
    double deltay = (getY() - other->getY());
    return sqrt(pow(deltax, 2) + pow(deltay, 2));
}

GraphObject::Direction Actor::getOpposite(GraphObject::Direction dir){
    switch (dir) {
        case GraphObject::up:
            return GraphObject::Direction::down;
        case GraphObject::down:
            return GraphObject::Direction::up;
        case GraphObject::left:
            return GraphObject::Direction::right;
        case GraphObject::right:
            return GraphObject::Direction::left;
        default:
            return GraphObject::Direction::none;
    }
}

void Actor::annoy(int howmuch){
    
}

void Actor::becomeRich(){
    
}

bool Actor::facingPlayer(){
    int dx, dy;
    getNextStep(getDirection(), dx, dy);
    Dummy t(getWorld(), getX(), getY(), getDirection());
    if (t.canGo(t.getDirection())){
        t.moveTo(t.getX() + dx, t.getY() + dy);
    }else{
        return false;
    }
    while (t.canGo(t.getDirection())){
        if (t.overlaps(getWorld()->getPlayer())){
            //std::cout << t.overlaps(getWorld()->getPlayer()) << std::endl;
            return true;
        }
        else{
            t.moveTo(t.getX() + dx, t.getY() + dy);
        }
    }
    //std::cout << t.overlaps(getWorld()->getPlayer()) << std::endl;
    return false;
    
//    Dummy t(getWorld(), getX(), getY(), getDirection());
//    while (t.canGo(t.getDirection())){
//
//    }
}

// overlaps
bool Actor::overlaps(Actor* other) const{
    for (int thisx = getX(); thisx < getX() + entitySize(); thisx++)
    {
    for (int thisy = getY(); thisy < getY() + entitySize(); thisy++)
    {
    for (int ox = other->getX(); ox < other->getX() + other->entitySize(); ox++)
    {
    for (int oy = other->getY(); oy < other->getY() + other->entitySize(); oy++)
    {
        if (thisx == ox && thisy == oy) return true;
    }
    }
    }
    }
    return false;
}

// check if Actor can go to a direction
bool Actor::canGo(GraphObject::Direction dir){
    
    // 1 no Actor can go out of bound
    switch (dir) {
        case GraphObject::up:
        {
            if (!(getY() < 64 - entitySize()))
                //std::cout << "upbound" << std::endl;
                return false;
            if (typeid(*this) == typeid(Boulder)) break;
            int deltax, deltay;
            getNextStep(up, deltax, deltay);
            Dummy t(getWorld(), getX() + deltax, getY() + deltay, none);
            for (Actor* i: getWorld()->getevery()){
                if (typeid(*i) == typeid(Boulder)){
                    if (t.overlaps(i)){
                        return false;
                    }
                }
            }
            break;
        }
        case GraphObject::down:
        {
            if (!(getY() > 0)){
                //std::cout << "downbound" << std::endl;
                return false;
            }
            if (typeid(*this) == typeid(Boulder)) break;
            int deltax, deltay;
            getNextStep(down, deltax, deltay);
            Dummy t(getWorld(), getX() + deltax, getY() + deltay, none);
            for (Actor* i: getWorld()->getevery()){
                if (typeid(*i) == typeid(Boulder)){
                    if (t.overlaps(i)){
                        return false;
                    }
                }
            }
            break;
        }
        case GraphObject::left:
        {
            if(!(getX() > 0)){
                //std::cout << "leftbound" << std::endl;
                return false;
            }
            if (typeid(*this) == typeid(Boulder)) break;
            int deltax, deltay;
            getNextStep(left, deltax, deltay);
            Dummy t(getWorld(), getX() + deltax, getY() + deltay, none);
            for (Actor* i: getWorld()->getevery()){
                if (typeid(*i) == typeid(Boulder)){
                    if (t.overlaps(i)){
                        return false;
                    }
                }
            }
            break;
        }
        case GraphObject::right:
        {
            if (!(getX() < 64 - entitySize())){
                //std::cout << "rightbound" << std::endl;
                return false;
            }
            if (typeid(*this) == typeid(Boulder)) break;
            int deltax, deltay;
            getNextStep(right, deltax, deltay);
            Dummy t(getWorld(), getX() + deltax, getY() + deltay, none);
            for (Actor* i: getWorld()->getevery()){
                if (typeid(*i) == typeid(Boulder)){
                    if (t.overlaps(i)){
                        return false;
                    }
                }
            }
            break;
        }
        case GraphObject::none:
        {
            if (!(0 <= getX() && getX() <= 64 - entitySize()
                && 0 <= getY() && getY() <= 64 - entitySize()))
                return false;
            break;
        }
    }
    
    // 2 if this Actor can dig, no need to check the earth around it
    if (canDig()) return true;
    
    // 3 check the earth on the current direction
    
    int deltax, deltay;
    getNextStep(dir, deltax, deltay);
    
    if (deltax == 0){ // going up or down
        // if go up
        int dy = deltay;
        if (deltay > 0) deltay *= entitySize();
        int starty = getY() + deltay;
        
        Dummy t(getWorld(), getX(), getY() + dy, getDirection());
        if (typeid(*this) != typeid(Boulder)){
            for (Actor* i: getWorld()->getevery()){
                if (typeid(*i) == typeid(Boulder)){
                    if (t.getDistance(i) <= 3){
                        return false;
                    }
                }
            }
        }
        for (int x = getX(); x < (getX() + entitySize()); x++){
            if (getWorld()->getfield()[starty][x] == nullptr){
                // no earth on this direction
                if (x == getX() + entitySize() - 1)
                    return true;
            }else{
                break;
            }
        }
    }else if (deltay == 0){ // going left or right
        // if go right
        int dx = deltax;
        if (deltax > 0) deltax *= entitySize();
        int startx = getX() + deltax;
        Dummy t(getWorld(), getX() + dx, getY(), getDirection());
        if (typeid(*this) != typeid(Boulder)){
            for (Actor* i: getWorld()->getevery()){
                if (typeid(*i) == typeid(Boulder)){
                    if (t.getDistance(i) <= 3){
                        return false;
                    }
                }
            }
        }
        for (int y = getY(); y < (getY() + entitySize()); y++){
            if (getWorld()->getfield()[y][startx] == nullptr){
                // no earth on this direction
                if (y == getY() + entitySize() - 1)
                    return true;
            }else{
                break;
            }
        }
    }else if (deltax == 0 && deltay == 0){
        return true;
    }
    // there is earth on that direction
    return false;
}
// set/get size
float Actor::entitySize() const{
    return m_size * 4;
}

World* Actor::getWorld(){
    return worldptr;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// class Earth

Earth::Earth(World* world, int x, int y)
: Actor(world, TID_EARTH, x, y, right, 0.25, 3)
{
    setVisible(true);
}

bool Earth::isActor() const{
    return false;
}

bool Earth::canDig() const{
    return false; // earth can't dig
}

void Earth::doSomething(){
//    if (overlap(getWorld()->getPlayer())){
//        std::cout << "overlap" << std::endl;
//        getWorld()->getfield()[getY()][getX()] = nullptr;
//        setDead();
//    }
}

Earth::~Earth(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// class Item: public Actor

Item::Item(World* world, int imageID, int startX, int startY, GraphObject::Direction startDir, float size, unsigned int depth)
: Actor(world, imageID, startX, startY, startDir, size, depth)
{
    
}

void Item::annoy(int howmuch){
    // items cannot be annoyed
}

bool Item::canDig() const{
    return false; // item cannot dig
}

bool Item::isActor() const{
    return false;
}

Item::~Item(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// class Pickupable: public Item

Pickupable::Pickupable(World* world, int imageID, int startX, int startY)
: Item(world, imageID, startX, startY, right, 1, 2)
{
    
}

void Pickupable::doSomething(){
    if (getDistance(getWorld()->getPlayer()) <= 3)
        setDead();
}

Pickupable::~Pickupable(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// Gold that only pickable to TunnelMan

PGold::PGold(World* world, int x, int y)
: Pickupable(world, TID_GOLD, x, y)
{
    // invisible
    setVisible(false);
}

void PGold::doSomething(){
    Pickupable::doSomething();
    if (!isAlive()){
        getWorld()->getPlayer()->curGold()++;
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(25);
    }
    if (getDistance(getWorld()->getPlayer()) <= 4){
        setVisible(true);
    }
}

PGold::~PGold(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Sonar::Sonar(World* world, int x, int y)
: Pickupable(world, TID_SONAR, x, y)
{
    setVisible(true);
    ticksRemain =
    getWorld()->max(100, 300 - 10 * getWorld()->getLevel());
}

void Sonar::doSomething(){
    Pickupable::doSomething();
    if (!isAlive()){
        getWorld()->getPlayer()->curSonar()++;
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(75);
    }
    ticksRemain--;
    if (ticksRemain <= 0) setDead();
}

Sonar::~Sonar(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(World* world, int x, int y)
: Pickupable(world, TID_WATER_POOL, x, y)
{
    setVisible(true);
    ticksRemain =
    getWorld()->max(100, 300 - 10 * getWorld()->getLevel());
}

void WaterPool::doSomething(){
    Pickupable::doSomething();
    if (!isAlive()){ // pick up by tunnelman
        getWorld()->getPlayer()->curWater() += 5;
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(100);
    }
    ticksRemain--;
    if (ticksRemain <= 0) setDead();
}

WaterPool::~WaterPool(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Barrel::Barrel(World* world, int x, int y)
: Pickupable(world, TID_BARREL, x, y)
{
    // invisible
    setVisible(false);
}

void Barrel::doSomething(){
    // check dead
    Pickupable::doSomething();
    if (!isAlive()){
        getWorld()->oilLeft()--;
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
    }
    // check visibility
    if (getDistance(getWorld()->getPlayer()) <= 4){
        setVisible(true);
    }
}

Barrel::~Barrel(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Boulder::Boulder(World* world, int x, int y)
: Item(world, TID_BOULDER, x, y, down, 1, 1), falling(false), waiting(30)
{
    // remove the earth for boulder
    for (int x = getX(); x < getX() + 4; x++){
        for (int y = getY(); y < getY() + 4; y++){
            if (getWorld()->getfield()[y][x] != nullptr){
                getWorld()->getfield()[y][x]->setDead();
                // modify oil field
                getWorld()->getfield()[y][x] = nullptr;
            }
        }
    }
    setVisible(true);
}


void Boulder::doSomething(){
    // if boulder is dead, do nothing
    if (!isAlive()) return;
    //std::cout << falling << std::endl;
    // if boulder is not falling
    if (!falling) {
        // if there is no earth below
        
        if (canGo(getDirection()))
            falling = true;
        
    }else{ // boulder is falling or prepare to fall
        waiting--; // counting down
        // if not falling, do nothing
        if (waiting > 0) return;
        
        // if start to fall, play sound
        if (waiting == 0)
            getWorld()->playSound(SOUND_FALLING_ROCK);
        
        // if can't go any further
        if (!canGo(getDirection())){
            setDead();
        }else{
            // to all actors in the game
            for (Actor* actor: getWorld()->getevery()){
                if (actor->isActor()){
                    // if within distance of 3
                    if (getDistance(actor) <= 3){
                        // annoy him with 100
                        actor->annoy(100);
                    }
                }
            }
            // go to the next tick
            moveTo(getX(), getY() - 1);
        }
    }
}

Boulder::~Boulder(){
    
}

DGold::DGold(World* world, int x, int y)
: Item(world, TID_GOLD, x, y, right, 1, 2)
{
    setVisible(true);
    lifetime = 100;
}

void DGold::doSomething(){
    //std::cout << lifetime << std::endl;
    for (Actor* i: getWorld()->getevery()){
        if (i->isActor() && !i->canDig()){
            if (getDistance(i) <= 3){
                i->becomeRich();
                setDead();
                return;
            }
            
        }
    }
    lifetime--;
    if (lifetime <= 0) setDead();
}

DGold::~DGold(){
    
}

Squirt::Squirt(World* world, int x, int y, GraphObject::Direction startdir)
: Item(world, TID_WATER_SPURT, x, y, startdir, 1, 1), distanceRemain(4)
{
    setVisible(true);
    checkNextStep();
}

void Squirt::checkNextStep(){
    if (distanceRemain == 4){
        int dx, dy;
        getNextStep(getDirection(), dx, dy);
        // put a dummy on the prev step
        Dummy dumdum(getWorld(), getX() - dx, getY() - dy, getDirection());
        // if this dummy can not go to that dir
        if (!dumdum.canGo(getDirection())){
            setDead();
            return;
        }
    }else if (!canGo(getDirection())){
        setDead();
        return;
    }
    // if this object is overlaping with boulder or earth
    for (Actor* i: getWorld()->getevery()){
        if (typeid(*i) == typeid(Boulder) || typeid(*i) == typeid(Earth)){
            if (overlaps(i)){
                setDead();
                return;
            }
        }
    }
}

void Squirt::doSomething(){
    // return if dead
    if (!isAlive()) return;
    
    
    // annoy protesters
    for (Actor* i: getWorld()->getevery()){
        if (i->isActor()){
            if (!i->canDig()){
                if (getDistance(i) <= 3){
                    i->annoy(2);
                    setDead();
                    break;
                }
            }
        }
    }
    
    // check distance
    if (distanceRemain <= 0){
        setDead();
    }
    
    // check status
    checkNextStep();
    //std::cout << isAlive() << std::endl;
    if (!isAlive()) return;
    
    // move
    int dx, dy;
    getNextStep(getDirection(), dx, dy);
    moveTo(getX() + dx, getY() + dy);
    distanceRemain--;
}

Squirt::~Squirt(){
    
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// class Actor: public Actor

Person::Person(World* world, int imageID, int startX, int startY,
             GraphObject::Direction startDirection, int hp)
: Actor(world, imageID, startX, startY, startDirection, 1, 0), hp(hp)
{
    // all actors are visible
    setVisible(true);
}

Person::~Person(){
    setVisible(false);
}

int Person::getHP() const{
    return hp;
}

void Person::annoy(int howmuch){
    hp -= howmuch;
}

bool Person::isActor() const{
    return true;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Protester::Protester(World* world, int imageID, int startX, int startY, int hp)
: Person(world, imageID, startX, startY, left, hp), leaveField(false), shoutRestingTimer(0), numSquaresRemain(15), lastperpturn(0), mazeRest(4)
{
    
    restingTimer =
    getWorld()->max(0, 3 - getWorld()->getLevel() / 4);
    setVisible(true);
}

bool Protester::canDig() const{
    return false; // protesters can not dig
}

void Protester::annoy(int howmuch){
    if (leaveField) return; // cannot be annoyed if leaving
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    if (howmuch > getHP()){
        leaveField = true;
        restingTimer = 0;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        if (typeid(*this) == typeid(Regular)){
            getWorld()->increaseScore(100);
        }else if (typeid(*this) == typeid(HardCore)){
            getWorld()->increaseScore(250);
        }
    }
    else{
        Person::annoy(howmuch);
        stun();
    }
}



GraphObject::Direction Protester::dirTo(int x, int y, int& moves, int maxStep){
    // the direction that this protester should go
    moves = 0;
    GraphObject::Direction res = none;
    
    std::queue<Dummy*> dummies;
    std::vector<Dummy*> dumptrs;
    std::queue<int> distance;
    // first dummy starts at finish point
    Dummy* firstdum = new Dummy(getWorld(), x, y, left);
    distance.push(0);
    dummies.push(firstdum);
    dumptrs.push_back(firstdum);
    
    // go from exit to protester
    while (!dummies.empty()) {
        
        if (distance.front() > maxStep) {
            for (Dummy* i: dumptrs) delete i;
            moves = --distance.front();
            return none;
        }
        
        // the change of coordinate
        int dx, dy;
        
        // if this guy is at the same place as this protester
        if (dummies.front()->getDistance(this) == 0){
            res = getOpposite(dummies.front()->getDirection());
            break; // break the loop
        }
        
        // go left?
        if (dummies.front()->canGo(left)){
            getNextStep(left, dx, dy);
            Dummy* newdum = new Dummy(getWorld(), dummies.front()->getX() + dx, dummies.front()->getY() + dy, left);
            // see if there are already dummies there
            for (Dummy* i: dumptrs){
                if (i->getDistance(newdum) == 0){
                    delete newdum;
                    newdum = nullptr;
                    break;
                }
            }
            // no one been there, place one there
            if (newdum != nullptr){
                //std::cout << newdum->getX() << ' ' << newdum->getY() << std::endl;
                dummies.push(newdum);
                dumptrs.push_back(newdum);
                distance.push(distance.front() + 1);
            }
            
        }
        if (dummies.front()->canGo(down)){
            getNextStep(down, dx, dy);
            Dummy* newdum = new Dummy(getWorld(), dummies.front()->getX() + dx, dummies.front()->getY() + dy, down);
            // see if there are already dummies there
            for (Dummy* i: dumptrs){
                if (i->getDistance(newdum) == 0){
                    delete newdum;
                    newdum = nullptr;
                    break;
                }
            }
            // no one been there, place one there
            if (newdum != nullptr){
                //std::cout << newdum->getX() << ' ' << newdum->getY() << std::endl;
                dummies.push(newdum);
                dumptrs.push_back(newdum);
                distance.push(distance.front() + 1);
            }
            
        }
        if (dummies.front()->canGo(right)){
            getNextStep(right, dx, dy);
            Dummy* newdum = new Dummy(getWorld(), dummies.front()->getX() + dx, dummies.front()->getY() + dy, right);
            // see if there are already dummies there
            for (Dummy* i: dumptrs){
                if (i->getDistance(newdum) == 0){
                    delete newdum;
                    newdum = nullptr;
                    break;
                }
            }
            // no one been there, place one there
            if (newdum != nullptr){
                //std::cout << newdum->getX() << ' ' << newdum->getY() << std::endl;
                dummies.push(newdum);
                dumptrs.push_back(newdum);
                distance.push(distance.front() + 1);
            }
        }
        if (dummies.front()->canGo(up)){
            getNextStep(up, dx, dy);
            Dummy* newdum = new Dummy(getWorld(), dummies.front()->getX() + dx, dummies.front()->getY() + dy, up);
            // see if there are already dummies there
            for (Dummy* i: dumptrs){
                if (i->getDistance(newdum) == 0){
                    delete newdum;
                    newdum = nullptr;
                    break;
                }
            }
            // no one been there, place one there
            if (newdum != nullptr){
                //std::cout << newdum->getX() << ' ' << newdum->getY() << std::endl;
                dummies.push(newdum);
                dumptrs.push_back(newdum);
                distance.push(distance.front() + 1);
            }
        }
        dummies.pop();
        distance.pop();
    }
    
    // clean everything up
    for (Dummy* i: dumptrs) delete i;
    moves = --distance.front();
    if (dummies.empty()) return none; // can't reach this location
    return res;
}

GraphObject::Direction Protester::canSeePlayer(){
    std::vector<Dummy*> fourDirs;
    Dummy t1(getWorld(), getX(), getY(), up);
    Dummy t2(getWorld(), getX(), getY(), down);
    Dummy t3(getWorld(), getX(), getY(), left);
    Dummy t4(getWorld(), getX(), getY(), right);
    fourDirs.push_back(&t1);
    fourDirs.push_back(&t2);
    fourDirs.push_back(&t3);
    fourDirs.push_back(&t4);
    GraphObject::Direction curr = none;
    for (auto i: fourDirs){
        if (i->facingPlayer()){
            curr = i->getDirection();
            break;
        }
    }
    
    return curr;
}

void Protester::stun(){
    // if it is leaving the field do nothing
    if (leaveField) return;
    restingTimer =
    getWorld()->max(50, (100 - 10 * getWorld()->getLevel()));
}


bool& Protester::leavingStatus(){
    return leaveField;
}

int& Protester::restingTime(){
    return restingTimer;
}

int& Protester::shoutResting(){
    return shoutRestingTimer;
}

int& Protester::numSquares(){
    return numSquaresRemain;
}

void Protester::upDateLastPerp(GraphObject::Direction d){
    if (d == up || d == down){
        if (getDirection() == left || getDirection() == right){
            lastperpturn = 0;
        }
        else lastperpturn++;
    }else if (d == left || d == right){
        if (getDirection() == up || getDirection() == down){
            lastperpturn = 0;
        }
        else lastperpturn++;
    }
    
}

int Protester::getLastPerp(){
    return lastperpturn;
}

Protester::~Protester(){
}



Regular::Regular(World* world, int x, int y)
: Protester(world, TID_PROTESTER, x, y, 5)
{
    
}

Regular::~Regular(){
    
}

void Regular::becomeRich(){
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(25);
    leavingStatus() = true;
}

void Regular::doSomething(){
    
    //std::cout << getLastPerp() << std::endl;
    //std::cout << facingPlayer() << std::endl;
    
    // alive ?
    if (!isAlive()) return;
    
    // resting ?
    if (restingTime() >= 0){
        restingTime()--;
        return;
    } restingTime() = 3;
    
    // leaving ?
    if (leavingStatus()){
        if (getX() == 60 && getY() == 60){
            setDead();
            return;
        }
        int dx, dy;
        int distanceToExit;
        GraphObject::Direction d = dirTo(60, 60, distanceToExit, INT_MAX);
        //std::cout << distanceToExit << std::endl;
        if (d == none) return;
        getNextStep(d, dx, dy);
        setDirection(d);
        moveTo(getX() + dx, getY() + dy);
        return;
    }
    
    // shout to tunnel man
    if (facingPlayer() && getDistance(getWorld()->getPlayer()) <= 4 && shoutResting() == 0){
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->getPlayer()->annoy(2);
        shoutResting() = 15;
        return;
    }else if (shoutResting() > 0){
        shoutResting()--;
        if (getDistance(getWorld()->getPlayer()) <= 4) return;
    }
    
    // see if protester can see tunnelman
    if (getDistance(getWorld()->getPlayer()) > 4){
        GraphObject::Direction curr = canSeePlayer();
        if (curr != none){
            int dx, dy;
            getNextStep(curr, dx, dy);
            upDateLastPerp(curr);
            setDirection(curr);
            moveTo(getX() + dx, getY() + dy);
            numSquares() = 0;
            return;
        }
    }
    // can't see tunnel man
    numSquares()--;
    if (numSquares() <= 0){
        GraphObject::Direction rdir = (GraphObject::Direction)(rand() % 4 + 1);
        while (!canGo(rdir)){
            rdir = (GraphObject::Direction)(rand() % 4 + 1);
        }
        upDateLastPerp(rdir);
        setDirection(rdir);
        numSquares() = rand() % 53 + 8;
        
    }else if (getLastPerp() >= 200){
        GraphObject::Direction thisd = getDirection();
        if (getDirection() == left || getDirection() == right){
            if (canGo(up) && canGo(down))
                thisd = ((GraphObject::Direction)(rand() % 2 + 1));
            else if (canGo(up))
                thisd = (up);
            else if (canGo(down))
                thisd = (down);
        }else if (getDirection() == up || getDirection() == down){
            if (canGo(left) && canGo(right))
                thisd = ((GraphObject::Direction)(rand() % 2 + 3));
            else if (canGo(left))
                thisd = (left);
            else if (canGo(right))
                thisd = (right);
        }
        numSquares() = rand() % 53 + 8;
        upDateLastPerp(thisd);
        setDirection(thisd);
    }
    if (!canGo(getDirection())){
        numSquares() = 0;
        return;
    }
    int dx, dy;
    getNextStep(getDirection(), dx, dy);
    upDateLastPerp(getDirection());
    moveTo(getX() + dx, getY() + dy);
    restingTime() = 3;
}

HardCore::HardCore(World* world, int x, int y)
: Protester(world, TID_HARD_CORE_PROTESTER, x, y, 20)
{
    
}

HardCore::~HardCore(){
    
}

void HardCore::becomeRich(){
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(50);
    restingTime() =
    getWorld()->max(50, (100 - getWorld()->getLevel() * 10));
}

void HardCore::doSomething(){
    // alive ?
    if (!isAlive()) return;
    
    // resting ?
    if (restingTime() >= 0){
        restingTime()--;
        return;
    } restingTime() = 3;
    
    // leaving ?
    if (leavingStatus()){
        if (getX() == 60 && getY() == 60){
            setDead();
            return;
        }
        int dx, dy;
        int distanceToExit;
        GraphObject::Direction d = dirTo(60, 60, distanceToExit, INT_MAX);
        //std::cout << distanceToExit << std::endl;
        if (d == none) return;
        getNextStep(d, dx, dy);
        setDirection(d);
        moveTo(getX() + dx, getY() + dy);
        return;
    }
    
    // shout to tunnel man
    if (facingPlayer() && getDistance(getWorld()->getPlayer()) <= 4 && shoutResting() == 0){
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->getPlayer()->annoy(2);
        shoutResting() = 15;
        return;
    }else if (shoutResting() > 0){
        shoutResting()--;
        if (getDistance(getWorld()->getPlayer()) <= 4) return;
    }

    // if hard core protester tracks tunnel man
    if (getDistance(getWorld()->getPlayer()) > 4 && getDistance(getWorld()->getPlayer()) < 23){
        // todo
        int m = 16 + getWorld()->getLevel() * 2;
        int actualSteps;
        GraphObject::Direction dirToTunnelMan = dirTo(getWorld()->getPlayer()->getX(), getWorld()->getPlayer()->getY(), actualSteps, m);
        if (dirToTunnelMan != none){
            if (actualSteps <= m){
                upDateLastPerp(dirToTunnelMan);
                setDirection(dirToTunnelMan);
                int dx, dy;
                getNextStep(getDirection(), dx, dy);
                moveTo(getX() + dx, getY() + dy);
                return;
            }
        }
    }

    // see if protester can see tunnelman
    if (getDistance(getWorld()->getPlayer()) > 4){
        GraphObject::Direction curr = canSeePlayer();
        if (curr != none){
            int dx, dy;
            getNextStep(curr, dx, dy);
            upDateLastPerp(curr);
            setDirection(curr);
            moveTo(getX() + dx, getY() + dy);
            numSquares() = 0;
            return;
        }
    }

    // can't see tunnel man
    numSquares()--;
    if (numSquares() <= 0){
        GraphObject::Direction rdir = (GraphObject::Direction)(rand() % 4 + 1);
        while (!canGo(rdir)){
            rdir = (GraphObject::Direction)(rand() % 4 + 1);
        }
        upDateLastPerp(rdir);
        setDirection(rdir);
        numSquares() = rand() % 53 + 8;

    }else if (getLastPerp() >= 200){
        GraphObject::Direction thisd = getDirection();
        if (getDirection() == left || getDirection() == right){
            if (canGo(up) && canGo(down))
                thisd = ((GraphObject::Direction)(rand() % 2 + 1));
            else if (canGo(up))
                thisd = (up);
            else if (canGo(down))
                thisd = (down);
        }else if (getDirection() == up || getDirection() == down){
            if (canGo(left) && canGo(right))
                thisd = ((GraphObject::Direction)(rand() % 2 + 3));
            else if (canGo(left))
                thisd = (left);
            else if (canGo(right))
                thisd = (right);
        }
        numSquares() = rand() % 53 + 8;
        upDateLastPerp(thisd);
        setDirection(thisd);
    }
    if (!canGo(getDirection())){
        numSquares() = 0;
        return;
    }
    int dx, dy;
    getNextStep(getDirection(), dx, dy);
    upDateLastPerp(getDirection());
    moveTo(getX() + dx, getY() + dy);
    restingTime() = 3;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// class TunnelMan: public Actor

TunnelMan::TunnelMan(World* world)
: Person(world, TID_PLAYER, 30, 60, right, 10),
gold(0), water(5), sonar(1)
{
    
}

bool TunnelMan::canDig() const{
    return true; // tunnelman can dig
}

TunnelMan::~TunnelMan(){
    
}

void TunnelMan::doSomething(){
    // 1 check self
    if (getHP() <= 0) setDead();
    if (isAlive() == false)
        return;
    
    // 2 get input
    int keypressed;
    if (getWorld()->getKey(keypressed)){
        switch (keypressed) {
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
            case KEY_PRESS_UP:
                //std::cout << "w" << std::endl;
                if (getDirection() == up){
                    if (canGo(up))
                        moveTo(getX(), getY() + 1);
                    else
                        moveTo(getX(), getY());
                }else{
                    setDirection(up);
                }
                break;
                
            case KEY_PRESS_DOWN:
                //std::cout << "s" << std::endl;
                if (getDirection() == down){
                    if (canGo(down))
                        moveTo(getX(), getY() - 1);
                    else
                        moveTo(getX(), getY());
                }else{
                    setDirection(down);
                }
                break;
                
            case KEY_PRESS_LEFT:
                //std::cout << "a" << std::endl;
                if (getDirection() == left){
                    if (canGo(left))
                        moveTo(getX() - 1, getY());
                    else
                        moveTo(getX(), getY());
                }else{
                    setDirection(left);
                }
                break;
                
            case KEY_PRESS_RIGHT:
                //std::cout << "d" << std::endl;
                if (getDirection() == right){
                    if (canGo(right))
                        moveTo(getX() + 1, getY());
                    else
                        moveTo(getX(), getY());
                }else{
                    setDirection(right);
                }
                break;
                
            case KEY_PRESS_SPACE:
                {
                    if (curWater() == 0) break;
                    curWater()--;
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    int dx = 0, dy = 0;
                    getNextStep(getDirection(), dx, dy);
                    dx *= entitySize();
                    dy *= entitySize();
                getWorld()->getevery().push_back(new Squirt(getWorld(), getX() + dx, getY() + dy, getDirection()));
                }
                break;
                
            // use sonar scan
            case 'z':
            case 'Z':
                if (curSonar() == 0) break;
                curSonar()--;
                getWorld()->playSound(SOUND_SONAR);
                for (Actor* i: getWorld()->getevery()){
                    if (!(i->isActor())){
                        if(getDistance(i) <= 12){
                            i->setVisible(true);
                        }
                    }
                }
                break;
                
            case KEY_PRESS_TAB:
            case 'T':
            case 't':
                if (curGold() == 0) break;
                curGold()--;
                getWorld()->getevery().push_back(new DGold(getWorld(), getX(), getY()));
                break;
            
            case '.': // cheat: finish current level
                getWorld()->oilLeft() = 0;
                break;
            case '/': // cheat: reveal all items in the oil mine
                for (Actor* i: getWorld()->getevery()){
                    i->setVisible(true);
                }
                break;
//
            default:
                break;
        }
    }
    bool dig = false;
    for (Actor* i: getWorld()->getevery()){
        if (typeid(*i) == typeid(Earth)){
            if (overlaps(i)){ getWorld()->getfield()[i->getY()][i->getX()]->setDead();
                dig = true;
                getWorld()->getfield()[i->getY()][i->getX()] = nullptr;
            }
        }
    }
    if (dig) getWorld()->playSound(SOUND_DIG);
}

void TunnelMan::annoy(int howmuch){
    if (howmuch > getHP()){
        setDead();
    }else{
        Person::annoy(howmuch);
    }
}

// attribute setter
int& TunnelMan::curGold()   {return gold;}
int& TunnelMan::curWater()  {return water;}
int& TunnelMan::curSonar()  {return sonar;}
