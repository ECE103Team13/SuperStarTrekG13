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
};

// function delcarations:
struct Galaxy createGalaxy();
struct Enterprise createEnterprise();
struct Enterprise gameIntro(struct Galaxy refGalaxy);
void getGameVitals(double *refGameVitals);
void gameEnd(double *refGVitals);
void getCommand();

int main() {
  bool gameRunning = true;
  struct Galaxy theGalaxy;
  struct Enterprise theEnterprise;
  double gVitals[5];
  getGameVitals(gVitals);

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
    getGameVitals(gVitals);
    gameEnd(gVitals);
    getCommand();
    safetyCounter++;
  }

  return 0;                                                                     // indicates normal return from main; i.e. no errors
}

//function definitions:
struct Galaxy createGalaxy() {
    struct Galaxy _galaxy;

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

void getGameVitals(double *refGameVitals) {

}

void gameEnd(double *refGVitals) {

}

void getCommand() {

}
//test
