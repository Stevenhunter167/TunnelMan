#include "World.h"
#include "Actor.h"
#include <iostream>
#include <string>
#include <sstream>

GameWorld* createStudentWorld(std::string assetDir)
{
	return new World(assetDir);
}

World::World(std::string assetDir)
: GameWorld(assetDir)
{
    field = new Earth**[64];
    for (int i = 0; i < 64; i++)
        field[i] = new Earth*[64];
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


int World::init(){
    lastTimeProtester = 0;
    totalProtester = 0;
    // initialize Oilfield
    
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 64; j++){
            if ((j > 29 && j < 34 && i > 3) || i > 59){
                field[i][j] = nullptr;
            }else{
                field[i][j] = new Earth(this, j, i);
                everything.push_back(field[i][j]);
            }
        }
    }
    
    
    // initialize items
    
    // boulder
    int boulderNum = min((getLevel() / 2 + 2), 9);
    for (int i = 0; i < boulderNum; i++){
        int x, y;
        initRandomLocation(x, y, 0, 60, 20, 56);
        everything.push_back(new Boulder(this, x, y));
    }
    
    // oil
    oilLeft() = min((getLevel() + 2), 21);
    for (int i = 0; i < oilLeft(); i++){
        int x, y;
        initRandomLocation(x, y, 0, 60, 0, 56);
        everything.push_back(new Barrel(this, x, y));
    }
    
    // gold
    int goldNum =
        ((5 - getLevel() / 2) > 2) ? (5 - getLevel() / 2) : 2;
    for (int i = 0; i < goldNum; i++){
        int x, y;
        initRandomLocation(x, y, 0, 60, 0, 56);
        everything.push_back(new PGold(this, x, y));
    }
    
    // test sonar
//    everything.push_back(new Sonar(this, 0, 60));
    
    // test protester
    //everything.push_back(new Regular(this, 60, 60));
    //everything.push_back(new HardCore(this, 40, 60));
    // test waterpool;
//    everything.push_back(new WaterPool(this, 30, 56));
//    everything.push_back(new WaterPool(this, 30, 52));
//    everything.push_back(new WaterPool(this, 30, 48));
    
    
    
    // initialize TunnelMan
    p = new TunnelMan(this);
    everything.push_back(p);
    
    return GWSTATUS_CONTINUE_GAME;
}

int World::move(){
    //  return constants:
    //    GWSTATUS_PLAYER_DIED;
    //    GWSTATUS_PLAYER_WON;
    //    GWSTATUS_LEVEL_ERROR;
    //    GWSTATUS_CONTINUE_GAME;
    //    GWSTATUS_FINISHED_LEVEL;
    
    // display game status
    
    //std::cout << GraphObject::Direction::right << std::endl;
    // up1, down2, left3, right4
    std::ostringstream header;
    
    header
    << "Lvl: "       << getLevel()
    << " Lives: "    << getLives()
    << " Hlth: "     << getPlayer()->getHP() * 10
    << "% Wtr: "     << getPlayer()->curWater()
    << " Gld: "      << getPlayer()->curGold()
    << " Oil Left: " << oilLeft()
    << " Sonar: "    << getPlayer()->curSonar()
    << " Scr: "      << getScore();
    
    setGameStatText(header.str());
    
    // player dead
    if (!p->isAlive()){
        playSound(SOUND_PLAYER_GIVE_UP);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    runTimeGoodie();
    runTimeProtester();
    
    // do something
    for (Actor* i: everything) i->doSomething();
    
    // remove dead objects
    for (auto it = everything.begin(); it != everything.end();){
        if (!(*it)->isAlive()){
            if ((*it)->isActor() && !(*it)->canDig())
                totalProtester--;
            delete (*it);
            it = everything.erase(it);
        }else it++;
    }
    
    // if no oil left, level finished
    if (oilLeft() == 0)
        return GWSTATUS_FINISHED_LEVEL;
    
    // else continue game
    return GWSTATUS_CONTINUE_GAME;
}

void World::cleanUp()
{
    // delete all Entities
    for (auto it = everything.begin(); it != everything.end();){
        delete *it;
        it = everything.erase(it);
    }
}

World::~World(){
    for (int i = 0; i < 64; i++){
        delete field[i];
    }delete field;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
void World::initRandomLocation(int &x, int &y, int xfrom, int xto, int yfrom, int yto){
    int tx;
    int ty;
    bool valid;
    while (true){
        valid = true;
        tx = rand() % (xto - xfrom + 1) + xfrom;
        // xfrom - xto
        ty = rand() % (yto - yfrom + 1) + yfrom;
        // yfrom - yto
        
        // in tunnel
        if (tx > 26 && tx < 34) valid = false;
        
        Dummy temp(this, tx, ty, GraphObject::none);
        for (Actor* i: everything){
            if (typeid(*i) != typeid(Earth)){
                // this object is less than 6 units from dummy
                if (temp.getDistance(i) <= 6){
                    valid = false; // find another random location
                    break;
                }
            }
        }
        
        if (valid) break;
    }
    x = tx; y = ty;
}

void World::runTimeGoodie(){
    int chanceDenom = getLevel() * 25 + 300;
    if (rand() % chanceDenom != 0) return;
    int addWhat = rand() & 5;
    if (addWhat == 0) everything.push_back(new Sonar(this, 0, 60));
    else{
        int x, y;
        while (true) {
            bool valid = true;
            x = rand() % 60;
            y = rand() % 60;
            Dummy temp(this, x, y, GraphObject::Direction::none);
            for (Actor* i: everything){
                if (temp.overlaps(i)){
                    valid = false;
                    break;
                }
            }
            if (valid) break;
        }
        everything.push_back(new WaterPool(this, x, y));
    }
}

void World::runTimeProtester(){
    if (lastTimeProtester > 0){
        lastTimeProtester--;
        return;
    }
    if (totalProtester >= min(15, 2 + getLevel() * 1.5)) return;
    
    int probHardCore = min(90, getLevel() * 10 + 30);
    if ((rand() % 100) < probHardCore){
        everything.push_back(new HardCore(this, 60, 60));
    }else{
        everything.push_back(new Regular(this, 60, 60));
    }
    
    lastTimeProtester = max(25, 200 - getLevel());
    totalProtester++;
}

int& World::oilLeft(){
    return oilCtr;
}

Earth*** World::getfield(){
    return field;
}

std::vector<Actor*>& World::getevery(){
    return everything;
}

TunnelMan* World::getPlayer(){
    return p;
}
