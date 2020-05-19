#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

struct Galaxy() {
    char coordinates[8][8][8][8];
    struct starbase starbases[10];
    struct klingon klingons[10];
};

struct Starbase() {
    int position[4];
    double damage;
    double energy;
    double shields;
};

struct Klingon() {
    int position[4];
    double damage;
    double energy;
    double shields;
};

struct Enterprise() {
    int position[4];
    double damage;
    double energy;
    double shields;
    int torpedoes;
};

struct Galaxy createGalaxy() {
    struct Galaxy _galaxy;

    //TODO: code to generate galaxy


    return _galaxy
};

struct Enterprise createEnterprise() {
    struct Enterprise _enterprise;

    //TODO: code to generate enterprise
    _enterprise = {{4, 6, 3, 5}, 0.0, 50.0, 15.0, 4};                     //TODO: delete temporary manual declaration once createEnterprise() is complete

    return _enterprise
};

void gameIntro(struct Galaxy *refGalaxy) {
    printf("                                    ,------*------,\n");
    printf("                    ,-------------   '---  ------'\n");
    printf("                     '-------- --'      / /\n");
    printf("                         ,---' '-------/ /--,\n");
    printf("                          '----------------'\n\n");
    printf("                    THE USS ENTERPRISE --- NCC-1701\n\n\n");

    struct Galaxy newGalaxy;
    newGalaxy = createGalaxy();
    *refGalaxy = newGalaxy;
    struct Enterprise outEnterprise;
    outEnterprise = createEnterprise();

    return outEnterprise;
}

int main()
{
    bool gameRunning = true;
    struct Galaxy theGalaxy;
    struct Enterprise theEnterprise;

    theEnterprise = gameIntro(theGalaxy);

    printf("Hello world!\n");
    printf("Editing this in atom!\n");


    return 0;                                                               // indicates normal return from main; no issues
}
