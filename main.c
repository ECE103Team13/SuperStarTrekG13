#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// enum declarations:
typedef enum {GREEN, YELLOW, RED} Condition;

//struct declarations:
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
} theGalaxy;

// function delcarations:
struct Galaxy createGalaxy();
struct Enterprise createEnterprise();
struct Enterprise gameIntro(struct Galaxy refGalaxy);

int main()
{
    bool gameRunning = true;
    struct Galaxy theGalaxy;
    struct Enterprise theEnterprise;

    theEnterprise = gameIntro(theGalaxy);

    return 0;                                                               // indicates normal return from main; no issues
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

    //TODO: delete temporary manual declaration once createEnterprise() is complete
    _enterprise.position[0] = 4;
    _enterprise.position[1] = 6;
    _enterprise.position[2] = 3;
    _enterprise.position[3] = 5;
    _enterprise.damage = 0.0;
    _enterprise.energy = 50.0;
    _enterprise.shields = 15.0;
    _enterprise.torpedoes = 4;

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
