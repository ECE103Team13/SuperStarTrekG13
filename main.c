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
  struct gameVitals;
};

// function delcarations:
struct Galaxy createGalaxy();
struct Enterprise createEnterprise();
struct Enterprise gameIntro(struct Galaxy refGalaxy);
struct Galaxy getGameVitals(struct Galaxy theGalaxy);
int gameEnd(double *refGVitals);
void getCommand();

int main() {
  bool gameRunning = true;
  struct Galaxy theGalaxy;
  struct Enterprise theEnterprise;
    createGalaxy(theGalaxy);
  double gVitals[5];
  theGalaxy = getGameVitals(theGalaxy);
  theEnterprise = gameIntro(theGalaxy);

                                                                                //DEBUG:
                                                                                printf("ENTERPRISE STATUS:\n");
                                                                                printf("Position-\n  Quadrant:\t(%d,%d)\n  Sector:\t(%d,%d)\n", theEnterprise.position[0], theEnterprise.position[1], theEnterprise.position[2], theEnterprise.position[3]);
                                                                                printf("Damage:\t\t%4.1f\n", theEnterprise.damage);
                                                                                printf("Energy:\t\t%4.1f\n", theEnterprise.energy);
                                                                                printf("Shields:\t%4.1f\n", theEnterprise.shields);
                                                                                printf("Torpedoes:\t%4d\n", theEnterprise.torpedoes);

  int safetyCounter = 0;
  while (gameRunning && (safetyCounter <= 1000)) {
    getGameVitals(theGalaxy);
    if(gameEnd(gVitals) == 0){
    }
    else if(gameEnd(gVitals) == 1){
    //  restart game
    }
    else {
    //  gameRunning = false;
    }
    getCommand();
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

struct Enterprise gameIntro(struct Galaxy refGalaxy) {
  printf("\n                                    ,------*------,\n");
  printf("                    ,-------------   '---  ------'\n");
  printf("                     '-------- --'      / /\n");
  printf("                         ,---' '-------/ /--,\n");
  printf("                          '----------------'\n\n");
  printf("                    THE USS ENTERPRISE --- NCC-1701\n\n\n");

  struct Galaxy newGalaxy;
  newGalaxy = createGalaxy();
  refGalaxy = newGalaxy;
  struct Enterprise outEnterprise;
  outEnterprise = createEnterprise();

  return outEnterprise;
}

struct Galaxy getGameVitals(struct Galaxy theGalaxy) {

}

int gameEnd(double *refGVitals) {
  struct gameVitals currentGame;
  for(int i = 0; i < 5; ++i){
    if(refGVitals[i] == 0){
      char getInput[100] = {0};

      printf("IT IS STARDATE %d\n", currentGame.stardate);
      printf("THERE WERE %d KLINGON BATTLE CRUISERS LEFT AT\n", currentGame.numKlingons);
      printf("THE END OF YOUR MISSION.\n\n");

      if(currentGame.numStarbases == 0) {
        exit(0);
      }
      else {
        printf("THE FEDERATION IS IN NEED OF A NEW STARSHIP COMMANDER\n");
        printf("FOR A SIMILAR MISSION -- IF THERE IS A VOLUNTEER\n");
        printf("LET HIM STEP FORWARD AND ENTER 'AYE' ");

        fgets(getInput, 100, stdin);
        getInput[strlen(getInput) - 1] = '\0';

        if((strcmp(getInput, "AYE") == 0) || (strcmp(getInput, "Aye") == 0)|| (strcmp(getInput, "aye") == 0)) {
          return 1;
        }
        else {
          printf("\nGame ended\n");
          return 2;
        }
      }
    }
    else{
      if(i == 4) {
        return 0;
      }
    }
  }
}

void getCommand() {

}
