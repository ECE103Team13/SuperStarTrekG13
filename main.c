#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// enum declarations:
typedef enum {GREEN, YELLOW, RED} Condition;

//struct declarations:
struct gameVitals {
  double eDamage;
  double eEnergy;
  int numStarbases;
  int numKlingons;
  int stardate;
  double reputation;
};

struct Starbase {
  int position[4];
  double damage;
  double energy;
  double shields;
};

struct Klingon {
  int position[4];
  double damage;
  double energy;
  double shields;
};

struct Enterprise {
  int position[4];
  double damage;
  double energy;
  double shields;
  int torpedoes;
  Condition condition;
};

struct Galaxy {
  char coordinates[8][8][8][8];
  struct Starbase starbases[10];
  struct Klingon klingons[10];
  struct Enterprise enterprise;
  struct gameVitals gVitals;
};

// function declarations:
struct Galaxy createGalaxy();
struct Enterprise createEnterprise();
struct gameVitals getGameVitals(struct Galaxy *refGalaxy);
struct Galaxy gameIntro();
bool gameEnd(struct Galaxy* refGalaxy);
void getCommand();

int main() {
  bool gameRunning = true;
  struct Galaxy theGalaxy;
  theGalaxy = gameIntro();
  theGalaxy.gVitals = getGameVitals(&theGalaxy);

                                        //DEBUG:
                                        printf("ENTERPRISE STATUS:\n");
                                        printf("Position-\n  Quadrant:\t(%d,%d)\n  Sector:\t(%d,%d)\n", theGalaxy.enterprise.position[0], theGalaxy.enterprise.position[1], theGalaxy.enterprise.position[2], theGalaxy.enterprise.position[3]);
                                        printf("Damage:\t\t%4.1f\n", theGalaxy.enterprise.damage);
                                        printf("Energy:\t\t%4.1f\n", theGalaxy.enterprise.energy);
                                        printf("Shields:\t%4.1f\n", theGalaxy.enterprise.shields);
                                        printf("Torpedoes:\t%4d\n", theGalaxy.enterprise.torpedoes);
                                        //END DEBUG

    int safetyCounter = 0;
    while (gameRunning && (safetyCounter <= 1000)) {

        /*
        theEnd = gameEnd(theGalaxy.gVitals);
        if(theEnd == 0){
            // Nothing happens. (Probably? For now, at least?)
        }
        else if(theEnd == 1){
            // restart game
        }
        else {
            // gameRunning = false;
        }
        */
        getCommand(&theGalaxy);
        gameRunning = gameEnd(&theGalaxy);
        safetyCounter++;
    }
    return 0;                                                                     // indicates normal return from main; i.e. no errors
}

//function definitions:
struct Galaxy createGalaxy() {
    // Does the game always start with the same number of starbases? I picked 10 arbitrarily.
    int numStarbases = 10;
    int numKlingons = 26;
    int numStars = 26;
    struct Galaxy _galaxy;
    struct Enterprise theEnterprise;
    struct Starbase starbases;
    struct Klingon klingons;

    // First initialize Galaxy with blank spaces to overwrite anything that was already there.
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            for (int k = 0; k <8; ++k)
            {
                for (int l = 0; l <8; ++l)
                {
                    _galaxy.coordinates[i][j][k][l] = ' ';
                }
            }
        }
    }

    // Set up starbases randomly in the galaxy:
    for (int i = 0; i <numStarbases; ++i)
    {
            // Generate and save random number coordinates
        int w = rand()%8;
        int x = rand()%8;
        int y = rand()%8;
        int z = rand()%8;
        // 'S' stands for starbase
        _galaxy.coordinates[w][x][y][z] = 'S';
        // Save location in starbases.position array
        starbases.position[0] = w;
        starbases.position[1] = x;
        starbases.position[2] = y;
        starbases.position[3] = z;
    }
    // Place klingons in random positions in the galaxy (where there aren't any starbases)
    //'K' stands for klingon
    for (int i = 0; i < numKlingons; ++i)
    {
            // Generate and save random number coordinates
        int w = rand()%8;
        int x = rand()%8;
        int y = rand()%8;
        int z = rand()%8;
        if (_galaxy.coordinates[w][x][y][z] != 'S')
        {
            _galaxy.coordinates[w][x][y][z] = 'K';
            // Save location in klingons.position array
            klingons.position[0] = w;
            klingons.position[1] = x;
            klingons.position[2] = y;
            klingons.position[3] = z;
        }
    }
    // Place stars in random positions in the galaxy (check for starbases or klingons first)
    // '*' stands for star
    for (int i = 0; i < numStars; ++i)
    {
            // Generate and save random number coordinates
        int w = rand()%8;
        int x = rand()%8;
        int y = rand()%8;
        int z = rand()%8;
        if ((_galaxy.coordinates[w][x][y][z] != 'S') || (_galaxy.coordinates[w][x][y][z] != 'K'))
        {
            _galaxy.coordinates[w][x][y][z] = '*';
            // I don't think we need to save locations of stars elsewhere?
        }
    }

    // Place Enterprise in an empty spot in the galaxy
    // 'E' stands for Enterprise
    // Generate and save random number coordinates
    int w = rand()%8;
    int x = rand()%8;
    int y = rand()%8;
    int z = rand()%8;
    if ((_galaxy.coordinates[w][x][y][z] != 'S') || (_galaxy.coordinates[w][x][y][z] != 'K') ||
        (_galaxy.coordinates[w][x][y][z] != '*'))
        {
            _galaxy.coordinates[w][x][y][z] = 'E';
            // Save this location in theEnterprise.position array
            theEnterprise.position[0] = w;
            theEnterprise.position[1] = x;
            theEnterprise.position[2] = y;
            theEnterprise.position[3] = z;
        }

                                                                    /*
                                                                    //Debug: Print map of galaxy, sector by sector:
                                                                    for (int i = 0; i < 8; ++i)
                                                                    {
                                                                        for (int j = 0; j < 8; ++j)
                                                                        {
                                                                            printf("Sector %d, %d:\n", i, j);
                                                                            for (int k = 0; k <8; ++k)
                                                                            {
                                                                                for (int l = 0; l <8; ++l)
                                                                                {
                                                                                    printf("%c ", _galaxy.coordinates[i][j][k][l]);
                                                                                }
                                                                                printf("\n");
                                                                            }
                                                                        }
                                                                    }
                                                                    //END Debug printf code
                                                                    */

    // Set up initial gameVitals for currentGame
    struct gameVitals currentGame;
    currentGame.eDamage = 0;
    currentGame.eEnergy = 10;
    currentGame.numStarbases = 10;
    currentGame.numKlingons = 26;
    currentGame.stardate = 5004;    // I made this number up
    currentGame.reputation = 2;

    return _galaxy;
};

struct Enterprise createEnterprise() {
  struct Enterprise _enterprise;

  //TODO: code to generate enterprise

  //TODO: delete following temporary manual declaration once createEnterprise() is complete
  _enterprise.position[0] = 4;
  _enterprise.position[1] = 6;
  _enterprise.position[2] = 3;
  _enterprise.position[3] = 5;
  _enterprise.damage = 0.0;
  _enterprise.energy = 50.0;
  _enterprise.shields = 15.0;
  _enterprise.torpedoes = 4;
  _enterprise.condition = YELLOW;
  //END code to delete

  return _enterprise;
};

struct Galaxy gameIntro(struct Galaxy *refGalaxy) {
  printf("\n                                    ,------*------,\n");
  printf("                    ,-------------   '---  ------'\n");
  printf("                     '-------- --'      / /\n");
  printf("                         ,---' '-------/ /--,\n");
  printf("                          '----------------'\n\n");
  printf("                    THE USS ENTERPRISE --- NCC-1701\n\n");
  printf("                        (Press Enter to begin)\n\n");
  getchar();

  struct Galaxy newGalaxy;
  newGalaxy = createGalaxy();
  newGalaxy.enterprise = createEnterprise();

  return newGalaxy;
}

struct gameVitals getGameVitals(struct Galaxy *refGalaxy) {     //TODO: replace manual assignment of gVitals (see below)
    struct gameVitals GVout;

    GVout.eDamage = (*refGalaxy).enterprise.damage;
    GVout.eEnergy = (*refGalaxy).enterprise.energy;
    GVout.numKlingons = 10;                                     // TODO: count number of starbases remaining (getGameVitals)
    GVout.numStarbases = 10;                                    // TODO: count number of klingons remaining (getGameVitals)
    GVout.reputation = 25;                                      // TODO: check if stardate limit has expired (getGameVitals)
    GVout.stardate = 1.0;                                       // TODO: check reputation??? (getGameVitals)

    return GVout;
}

bool gameEnd(struct Galaxy *refGalaxy) {
    (*refGalaxy).gVitals = getGameVitals(&refGalaxy);
    struct gameVitals currentGame = {(*refGalaxy).gVitals.eDamage, (*refGalaxy).gVitals.eDamage, (*refGalaxy).gVitals.numKlingons, (*refGalaxy).gVitals.numStarbases, (*refGalaxy).gVitals.reputation, (*refGalaxy).gVitals.stardate};

    for(int i = 0; i < 5; ++i){
        if(currentGame.stardate == 0){
            char getInput[100] = {0};

            printf("IT IS STARDATE %d\n", currentGame.stardate);
            printf("THERE WERE %d KLINGON BATTLE CRUISERS LEFT AT\n", currentGame.numKlingons);
            printf("THE END OF YOUR MISSION.\n\n");

            if(currentGame.numStarbases == 0) {
                printf("NO STARBASES REMAINING!\n");        // TODO: Fix placeholder game end message for no starbases remaining
                exit(0);
            } else {
                printf("THE FEDERATION IS IN NEED OF A NEW STARSHIP COMMANDER\n");
                printf("FOR A SIMILAR MISSION -- IF THERE IS A VOLUNTEER\n");
                printf("LET HIM STEP FORWARD AND ENTER 'AYE' ");

                fgets(getInput, 100, stdin);
                getInput[strlen(getInput) - 1] = '\0';

                if((strcmp(getInput, "AYE") == 0) || (strcmp(getInput, "Aye") == 0)|| (strcmp(getInput, "aye") == 0)) {
                    return true;
                } else {
                    printf("\nGame ended\n");
                    return false;
                }
            }
    }
    else{
      if(i == 4) {
        return false;
      }
    }
  }
}

void getCommand() {

}
