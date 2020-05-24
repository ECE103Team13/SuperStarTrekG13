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
void gameEnd(double *refGVitals);
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
    gameEnd(gVitals);
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
        // 'S' stands for starbase
        _galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] = 'S';
    }
    // Place klingons in random positions in the galaxy (where there aren't any starbases)
    //'K' stands for klingon
    for (int i = 0; i < numKlingons; ++i)
    {
        if (_galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] != 'S')
        {
            _galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] = 'K';
        }
    }
    // Place stars in random positions in the galaxy (check for starbases or klingons first)
    // '*' stands for star
    for (int i = 0; i < numStars; ++i)
    {
        if ((_galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] != 'S') || (_galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] != 'K'))
        {
            _galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] = '*';
        }
    }

    // Place Enterprise in an empty spot in the galaxy
    // 'E' stands for Enterprise
    if ((_galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] != 'S') || (_galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] != 'K') ||
        (_galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] != '*'))
        {
            _galaxy.coordinates[rand()%8][rand()%8][rand()%8][rand()%8] = 'E';
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
    //TODO: code to generate galaxy

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

void gameEnd(double *refGVitals) {

}

void getCommand() {

}
