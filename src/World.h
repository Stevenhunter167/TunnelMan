#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

class Actor;
class TunnelMan;
class Earth;

//    GameWorld :
//    unsigned int getLives() const;
//    void decLives();
//    void incLives();

//    unsigned int getScore() const;
//    unsigned int getLevel() const;
//    void increaseScore(unsigned int howMuch);
//    void setGameStatText(string text);
//    bool getKey(int& value);
//    void playSound(int soundID);

class World : public GameWorld{
    
public:
    
    inline int max(int a, int b){
        return (a > b) ? a : b;
    }
    
    inline int min(int a, int b){
        return (a < b) ? a : b;
    }
    
    World(std::string assetDir);
    ~World();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    
    int&                  oilLeft();
    // get the tunnelman ptr
    TunnelMan*            getPlayer();
    // get everything in the game
    std::vector<Actor*>&  getevery();
    // get the earth field
    Earth***              getfield();

private:
    void initRandomLocation(int& x, int& y, int xfrom, int xto, int yfrom, int yto);
    void runTimeGoodie();
    void runTimeProtester();
    
    std::vector<Actor*> everything;
    Earth*** field;
    TunnelMan* p; // tunnel man
    int oilCtr;
    int lastTimeSonar;
    int lastTimeProtester;
    int totalProtester;
};

#endif // STUDENTWORLD_H_
