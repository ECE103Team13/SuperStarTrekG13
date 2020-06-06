#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#define PI 3.14159265359
#define MAX_INPUT_LENGTH 127
#define GAME_NUM_KLINGONS 26
#define GAME_NUM_STARBASES 3
#define GAME_NUM_STARS 192
#define GAME_INITIAL_ENERGY 3000
#define GAME_INITIAL_STARDATE 2700
#define GAME_STARDATE_LIMIT 26

bool gblDEBUG = false;                                                      // global debug variable which enables non-game debug options; triggered by giving DBG command

// enum declarations:
typedef enum {GREEN, YELLOW, RED} Condition;

//struct declarations:
struct gameVitals {
  int eEnergy;
  int numStarbases;
  int numKlingons;
  double stardate;
  bool userQuit;
};

struct Starbase {
  int position[4];
  double HP;
  int energy;
  int shields;
};

struct Klingon {
  int position[4];
  double HP;
  int energy;
  int shields;
};

struct Enterprise {
  int position[4];
  int explored[9][9];
  double sysDamage[8];
  int energy;
  int shields;
  int torpedoes;
  Condition condition;
};

struct Galaxy {
  char coordinates[9][9][9][9];
  struct Starbase starbases[GAME_NUM_STARBASES];
  struct Klingon klingons[GAME_NUM_KLINGONS];
  struct Enterprise enterprise;
  struct gameVitals gVitals;
};

// function declarations:
struct Galaxy createGalaxy(void);
struct Enterprise createEnterprise(struct Galaxy *refGalaxy);
struct gameVitals getGameVitals(struct Galaxy *refGalaxy);
struct Galaxy gameIntro(void);
bool gameEnd(struct Galaxy* refGalaxy);
void getCommand(struct Galaxy* refGalaxy);
void exeHLP(struct Galaxy *refGalaxy);
void exeNAV(struct Galaxy *refGalaxy);
void exeSRS(struct Galaxy *refGalaxy);
void exeLRS(struct Galaxy *refGalaxy);
void exeDAM(struct Galaxy *refGalaxy);
void exeCOM(struct Galaxy *refGalaxy);
void exePHA(struct Galaxy *refGalaxy);
void exeTOR(struct Galaxy *refGalaxy);
void exeSHE(struct Galaxy *refGalaxy);
void exeXXX(struct Galaxy *refGalaxy);
void exeDBG(struct Galaxy *refGalaxy);
double getDist(struct Galaxy* refGalaxy, int* destination);
void setDest(int* _start, double dir, double dist, int* _destination);
double getDirection(struct Galaxy* refGalaxy, int* destination);
void KlingonsFire(struct Galaxy *refGalaxy);
void KlingonsFireMT(struct Galaxy* refGalaxy, double WS);
struct Klingon* getNthClosestKlingon(struct Galaxy* refGalaxy, int n);
void strtrim(char* string, int n);
void remNL(char* string, int n);
void strToLower(char* string, int n);
void strToUpper(char* string, int n);
void strinj(char* parstr, int parsiz, char* chldstr, int chldlen, int idx);
void displayManual(void);


////////////////////////////////////////////
//////////////// START MAIN ////////////////
int main() {
    bool gameRunning = true;
    struct Galaxy theGalaxy;
    theGalaxy = gameIntro();
    theGalaxy.gVitals = getGameVitals(&theGalaxy);

    while (gameRunning) {                                                           // Main game loop:
        getCommand(&theGalaxy);
        gameRunning = gameEnd(&theGalaxy);
    }
    return 0;                                                                       // indicates normal return from main; i.e. no errors
}
///////////////// END MAIN /////////////////
////////////////////////////////////////////

//function definitions:
struct Galaxy createGalaxy(void) {
    srand((int)time(0));                                        // Seed rand function with current time
    int numStarbases = GAME_NUM_STARBASES;
    int numKlingons = 0;
    int numStars = GAME_NUM_STARS;
    struct Galaxy _galaxy;
    struct Starbase starbases;
    struct Klingon klingons;

    // First initialize Galaxy with blank spaces to overwrite anything that was already there.
    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= 8; ++j) {
            for (int k = 1; k <= 8; ++k) {
                for (int l = 1; l <= 8; ++l) {
                    _galaxy.coordinates[i][j][k][l] = ' ';
                }
            }
        }
    }

    // Place starbases randomly in the galaxy:
    for (int i = 0; i<numStarbases; ++i) {
        struct Starbase newSB;                                  // create blank starbase struct
        bool notPlaced = true;                                  // Assign random position:
        while (notPlaced) {                                     // repeat until valid location found
            int w = rand()%8 + 1;                               // Generate and save random number coordinates:
            int x = rand()%8 + 1;
            int y = rand()%8 + 1;
            int z = rand()%8 + 1;
            if (_galaxy.coordinates[w][x][y][z] == ' ') {       // Check that the spot is empty
                newSB.position[0] = w;                          // Save location in starbase.position array
                newSB.position[1] = x;
                newSB.position[2] = y;
                newSB.position[3] = z;
                newSB.HP = 1.0;
                notPlaced = false;
            }
        }
        _galaxy.starbases[i] = newSB;                           // save completed starbase in galaxy.starbases array
    }

    // Place Klingons randomly in the galaxy:
    //for (int i = 0; i<numKlingons; ++i) {
    // TODO: I changed this to a while loop hopefully to continue iterating UNTIL we have 26 klingons. Then ideally at the end of this sequence, numKlingons should = 26.
    while (numKlingons <= GAME_NUM_KLINGONS) {

        struct Klingon newKlingon;
        bool notPlaced = true;
        while (notPlaced) {                                     // Assign random position:
            //int w = rand()%8 + 1;
            //int x = rand()%8 + 1;
            // TODO: I changed this to iterate through 8x8 quadrants to make sure k could be placed in each quadrant
            for (int w = 1; w < 9; w++) {                       // Iterate through every quadrant
                for (int x = 1; x < 9; x++) {

            float chance = rand()%100;          // Generate random percentage
            int klingonsInQuad = 0;           // Limit klingons in a given quadrant to 1, 2, or 3
            if (chance > 95) {
                klingonsInQuad = 3;
                numKlingons = numKlingons + 3;
            }
            else if (chance > 90) {
                klingonsInQuad = 2;
                numKlingons = numKlingons + 2;
            }
            else if (chance > 80) {
                klingonsInQuad = 1;
                numKlingons = numKlingons + 1;
            }
            else {
                klingonsInQuad = 0;
            }
                    // TODO: This does limit how many k are in a quadrant, BUT I can't tell if all 26 are being placed. I may have created new problems with this solution.
            for (int j = 0; j < klingonsInQuad; ++j) {
                int y = rand()%8 + 1;                           // Use rand() to choose a random sector in current quadrant
                int z = rand()%8 + 1;
                if (_galaxy.coordinates[w][x][y][z] == ' ') {       // Check that spot is empty
                    newKlingon.position[0] = w;                     // Save location in klingons.position array
                    newKlingon.position[1] = x;
                    newKlingon.position[2] = y;
                    newKlingon.position[3] = z;
                    newKlingon.HP = 1.0;
                    newKlingon.energy = 100;
                    newKlingon.shields = 100;
                    notPlaced = false;
                }
            }
                    int i = 0;
                    _galaxy.klingons[i] = newKlingon; // save completed klingon in galaxy.klingons array
                    i++;
                }
            }
        }
    }

    // Place stars randomly in the galaxy:
    for (int i = 0; i < numStars; ++i) {                        // Generate and save random number coordinates
        bool notPlaced = true;
        while (notPlaced) {
            int w = (rand()%8) + 1;
            int x = (rand()%8) + 1;
            int y = (rand()%8) + 1;
            int z = (rand()%8) + 1;
            if (_galaxy.coordinates[w][x][y][z] == ' ') {
                _galaxy.coordinates[w][x][y][z] = '*';          // '*' is a star
                notPlaced = false;
            }
        }
    }

// Set up starting gameVitals for currentGame
// All game vitals will count down to 0
// If any game vital reaches 0, game will end.
    _galaxy.gVitals.eEnergy = GAME_INITIAL_ENERGY;
    _galaxy.gVitals.numStarbases = numStarbases;
    _galaxy.gVitals.numKlingons = numKlingons;
    _galaxy.gVitals.stardate = GAME_INITIAL_STARDATE;
    _galaxy.gVitals.userQuit = false;
    return _galaxy;
};

struct Enterprise createEnterprise(struct Galaxy *refGalaxy) {
    struct Enterprise _enterprise;
    // Place Enterprise in the middle of the galaxy
        _enterprise.position[0] = 5;
        _enterprise.position[1] = 6;
        _enterprise.position[2] = 4;
        _enterprise.position[3] = 1;
    // Mark all quadrants as 0 (not explored yet) except for current quadrant
    for (int i = 1; i <= 8; i++) {
        for (int k = 1; k <= 8; k++) {
            _enterprise.explored[i][k] = 0;
        }
    }
    _enterprise.explored[_enterprise.position[0]][_enterprise.position[1]] = 1; // Mark current quadrant as Explored

    // Set up initial conditions for enterprise
    for (int i = 0; i < 8; ++i) { _enterprise.sysDamage[i] = 0; }
    _enterprise.energy = 3000;
    _enterprise.shields = 0;
    _enterprise.torpedoes = 10;
    _enterprise.condition = RED;
    return _enterprise;
};

struct Galaxy gameIntro(void) {                                             // display splash ASCII, call new game generation functions
  printf("\n                                    ,------*------,\n");
  printf("                    ,-------------   '---  ------'\n");
  printf("                     '-------- --'      / /\n");
  printf("                         ,---' '-------/ /--,\n");
  printf("                          '----------------'\n\n");
  printf("                    THE USS ENTERPRISE --- NCC-1701\n\n");
  printf("ENTER 'M' TO VIEW THE GAME MANUAL BEFORE STARTING GAME\nOTHERWISE, PRESS ENTER TO BEGIN GAME\n");

    // Ask user if they want to view game manual

    char input[MAX_INPUT_LENGTH];
    fgets(input, MAX_INPUT_LENGTH, stdin);
    strtrim(input, strlen(strtrim));                                        // Trim any leading spaces off the input
    input[1] = '\0';                                                        // Truncate the command string to 3 characters
    strToUpper(input, strlen(input));                                       // convert to uppercase
    if (input[0] == 'M') { displayManual(); }

  struct Galaxy newGalaxy;
  newGalaxy = createGalaxy();
  newGalaxy.enterprise = createEnterprise(&newGalaxy);
    printf("YOUR ORDERS ARE AS FOLLOWS :\n");
    printf("\tDESTROY THE %d KLINGON WARSHIPS WHICH HAVE INVADED\n", newGalaxy.gVitals.numKlingons);
    printf("\tTHE GALAXY BEFORE THEY CAN ATTACK FEDERATION HEADQUARTERS\n");
    printf("\tON STARDATE %4.0f. THIS GIVES YOU %d DAYS. THERE ARE\n", (newGalaxy.gVitals.stardate)+GAME_STARDATE_LIMIT, GAME_STARDATE_LIMIT);
    printf("\t%d STARBASES IN THE GALAXY FOR RESUPPLYING YOUR SHIP.\n", newGalaxy.gVitals.numStarbases);
  exeSRS(&newGalaxy);

  return newGalaxy;
}

struct gameVitals getGameVitals(struct Galaxy *refGalaxy) {                 //TODO: replace direct assignment of gVitals (see below)
    struct gameVitals GVout;
    GVout.eEnergy = (*refGalaxy).gVitals.eEnergy;
    GVout.numKlingons = (*refGalaxy).gVitals.numKlingons;                   // TODO: count number of starbases remaining (getGameVitals)
    GVout.numStarbases = (*refGalaxy).gVitals.numStarbases;                 // TODO: count number of klingons remaining (getGameVitals)
    GVout.stardate = (*refGalaxy).gVitals.stardate;                         // TODO: check if stardate limit has expired (getGameVitals)
    GVout.userQuit = (*refGalaxy).gVitals.userQuit;

    return GVout;
}

bool gameEnd(struct Galaxy *refGalaxy) {
    if ((*refGalaxy).gVitals.userQuit) {                                    // If user chooses Quit
        char getInput[100] = {0};
        printf("THE FEDERATION IS IN NEED OF A NEW STARSHIP COMMANDER\n");
        printf("FOR A SIMILAR MISSION -- IF THERE IS A VOLUNTEER\n");
        printf("LET HIM STEP FORWARD AND ENTER 'AYE' ");

        fgets(getInput, 100, stdin);
        getInput[strlen(getInput) - 1] = '\0';

// Check if user starts a new game
        if((strcmp(getInput, "AYE") == 0) || (strcmp(getInput, "Aye") == 0)|| (strcmp(getInput, "aye") == 0)) {
            (*refGalaxy).gVitals.userQuit = false;
            return true;
        } else {
            printf("\nGame ended\n");
            (*refGalaxy).gVitals.userQuit = true;
            return false;
        }
    }

        // Check each member of gVitals to see whether game ending conditions are met
        //for(int i = 0; i < 5; ++i){
        // If stardate is 0 (time is up)
        if((*refGalaxy).gVitals.stardate == 0){
            char getInput[100] = {0};

            printf("IT IS STARDATE %d\n", (*refGalaxy).gVitals.stardate);
            printf("THERE WERE %d KLINGON BATTLE CRUISERS LEFT AT\n", (*refGalaxy).gVitals.numKlingons);
            printf("THE END OF YOUR MISSION.\n\n");
            return false;
        }
        else if((*refGalaxy).gVitals.numStarbases == 0) {
                printf("NO STARBASES REMAINING!\n");                    //TODO: Fix placeholder game end message for no starbases remaining (gameEnd)
                return false;
            }
        else { return true; }                                           //TODO: Fix game end infinite loop after choosing to play again.
}

void getCommand(struct Galaxy *refGalaxy) {
    char cmdString[MAX_INPUT_LENGTH];
    printf("COMMAND: ");                                                                                // Prompt user for command input
    fgets(cmdString, MAX_INPUT_LENGTH, stdin);
    strtrim(cmdString, strlen(strtrim));                                                                // Trim any leading spaces off the input
    cmdString[3] = '\0';                                                                                // Truncate the command string to 3 characters
    strToUpper(cmdString, strlen(cmdString));                                                           // convert to uppercase
    if (strcmp(cmdString, "NAV") == 0) { exeNAV(refGalaxy);                                             // check for possible commands and execute if found
    } else if (strcmp(cmdString, "SRS") == 0) { exeSRS(refGalaxy);
    } else if (strcmp(cmdString, "LRS") == 0) { exeLRS(refGalaxy);
    } else if (strcmp(cmdString, "DAM") == 0) { exeDAM(refGalaxy);
    } else if (strcmp(cmdString, "COM") == 0) { exeCOM(refGalaxy);
    } else if (strcmp(cmdString, "PHA") == 0) { exePHA(refGalaxy);
    } else if (strcmp(cmdString, "TOR") == 0) { exeTOR(refGalaxy);
    } else if (strcmp(cmdString, "SHE") == 0) { exeSHE(refGalaxy);
    } else if (strcmp(cmdString, "MAN") == 0) { displayManual();
    } else if (strcmp(cmdString, "XXX") == 0) { (*refGalaxy).gVitals.userQuit = true;                   // If user chooses resign, trigger the 'userQuit' gameVital
    } else if (strcmp(cmdString, "DBG") == 0) { gblDEBUG = true; exeDBG(refGalaxy);                     // DEBUG: 'DBG' == a "secret" debug option showing game data and enabling '+' commands below:
    } else if ((strcmp(cmdString, "+LR") == 0)&&(gblDEBUG)) { exeSLR(refGalaxy);                        // DEBUG: '+LR' == hidden "super long range" sensors: calls LRS for all quadrants
    } else if ((strcmp(cmdString, "+EN") == 0)&&(gblDEBUG)) {                                           // DEBUG:
        (*refGalaxy).enterprise.energy += 1000;
        printf("ENTERPRISE ENERGY INCREASED BY 1000 UNITS.\n");
        printf("ENERGY LEVELS NOW AT %d UNITS.\n\n", (*refGalaxy).enterprise.energy);
    } else if ((strcmp(cmdString, "+TP") == 0)&&(gblDEBUG)) {                                           // DEBUG:
        (*refGalaxy).enterprise.torpedoes += 10;
        printf("10 TORPEDOES ADDED TO ENTERPRISE INVENTORY.\n");
        printf("%d TORPEDOES TOTAL NOW CARRIED.\n\n", (*refGalaxy).enterprise.torpedoes);
    } else if ((strcmp(cmdString, "+RP") == 0)&&(gblDEBUG)) {                                           // DEBUG:
        (*refGalaxy).enterprise.sysDamage[0] = 1.0;
        (*refGalaxy).enterprise.sysDamage[1] = 1.0;
        (*refGalaxy).enterprise.sysDamage[2] = 1.0;
        (*refGalaxy).enterprise.sysDamage[3] = 1.0;
        (*refGalaxy).enterprise.sysDamage[4] = 1.0;
        (*refGalaxy).enterprise.sysDamage[5] = 1.0;
        (*refGalaxy).enterprise.sysDamage[6] = 1.0;
        (*refGalaxy).enterprise.sysDamage[7] = 1.0;
        printf("ALL ENTERPRISE SYSTEMS RESTORED TO 100%.\n\n");
    } else if ((strcmp(cmdString, "+OV") == 0)&&(gblDEBUG)) {                                           // DEBUG:
        (*refGalaxy).enterprise.sysDamage[0] += 2.0;
        (*refGalaxy).enterprise.sysDamage[1] += 2.0;
        (*refGalaxy).enterprise.sysDamage[2] += 2.0;
        (*refGalaxy).enterprise.sysDamage[3] += 2.0;
        (*refGalaxy).enterprise.sysDamage[4] += 2.0;
        (*refGalaxy).enterprise.sysDamage[5] += 2.0;
        (*refGalaxy).enterprise.sysDamage[6] += 2.0;
        (*refGalaxy).enterprise.sysDamage[7] += 2.0;
        printf("POWER TO ALL ENTERPRISE SYSTEMS INCREASED BY 200% OF NORMAL.\n\n");
    } else { exeHLP(refGalaxy); }                                                                       // If no other command recognized, print help menu
}

void exeHLP(struct Galaxy* refGalaxy) {
    printf("Enter one of the following:\n");
    printf("  NAV  (TO SET COURSE)\n");
    printf("  SRS  (FOR SHORT RANGE SENSOR SCAN)\n");
    printf("  LRS  (FOR LONG RANGE SENSOR SCAN)\n");
    printf("  PHA  (TO FIRE PHASERS)\n");
    printf("  TOR  (TO FIRE PHOTON TORPEDOS)\n");
    printf("  SHE  (TO RAISE OR LOWER SHIELDS)\n");
    printf("  DAM  (FOR DAMAGE CONTROL REPORTS)\n");
    printf("  COM  (TO CALL ON LIBRARY COMPUTER)\n");
    printf("  MAN  (TO VIEW GAME MANUAL)\n");
    printf("  XXX  (TO RESIGN YOUR COMMAND)\n\n");
    return;
}

void exeNAV(struct Galaxy* refGalaxy) {
    // -=NAV REFERENCE=-
    //
    // COURSE:
    //  4  3  2
    //   \ | /
    //    \|/
    //  5--*--1
    //    /|\
    //   / | \
    //  6  7  8
    // Course is a float number: decimals specify angles between integer directions. Direction 9.0===1.0
    //
    // WARP FACTOR:
    // 1.0 = 1 quadrant in the specified direction
    // 0.N = N/10ths of 1 quadrant (rounded)
    //
    // COORDINATES:
    // Game uses (row,column) coordinate format, incrementing from the "top" down, meaning (1,1) is the top-left-most coordinate pair

    char* cmdString[MAX_INPUT_LENGTH];
    double setCourse = 0.0;
    double setWarp = 0.0;
    printf("COURSE (1-9) ");                                                                                // Prompt user for course info
    fgets(cmdString, MAX_INPUT_LENGTH, stdin);
    remNL(cmdString, strlen(cmdString));
    strtrim(cmdString, strlen(strtrim));                                                                    // Trim any leading spaces off the input
    setCourse = atof(cmdString);
    if ((setCourse < 0.0) || (setCourse > 9.0)) {
        printf("LT. SULU REPORTS, 'INCORRECT COURSE DATA, SIR!'\n");
    } else {
        double maxWarp = 9.0;
        if ((*refGalaxy).enterprise.sysDamage[0] < 0) { maxWarp = -9.0 * (*refGalaxy).enterprise.sysDamage[0]; }
        printf("WARP FACTOR (0-%.1f) ", maxWarp);
        fgets(cmdString, MAX_INPUT_LENGTH, stdin);
        remNL(cmdString, strlen(cmdString));
        strtrim(cmdString, strlen(strtrim));                                                                // Trim any leading spaces off the input
        setWarp = atof(cmdString);
        if ((setWarp < 0) || (setWarp > 9.0)) {     printf("CHIEF ENGINEER SCOTT REPORTS, 'THE ENGINES WON'T TAKE WARP %.1f!'\n", setWarp);
        } else if (setWarp > maxWarp) {             printf("WARP ENGINES ARE DAMAGED. MAXIMUM SPEED = WARP %.1f!'\n", maxWarp);
        } else {
            if (KlingonsInQuadrant(refGalaxy, (*refGalaxy).enterprise.position) > 0) {
                KlingonsFireMT(refGalaxy, setWarp);
            }
            int curPos[4] = {(*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]};
            int newPos[4] = {(*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]};
            setDest(curPos, setCourse, setWarp, newPos);                                                    //TODO: check for obstacles when traveling (NAV)
            (*refGalaxy).enterprise.position[0] = newPos[0];
            (*refGalaxy).enterprise.position[1] = newPos[1];
            (*refGalaxy).enterprise.position[2] = newPos[2];
            (*refGalaxy).enterprise.position[3] = newPos[3];
            KlingonsFire(refGalaxy);
            (*refGalaxy).gVitals.stardate = round((*refGalaxy).gVitals.stardate + 1);                       //TODO: stardate increments maximum of 1; less with smaller moves
        }
    }                                                                                                       // TODO: Write code to output quadrant name upon entry (NAV)

    return;
}

void exeSRS(struct Galaxy* refGalaxy) {
    // Check if SRS sensors are working
    if ((*refGalaxy).enterprise.sysDamage[1] < 0) {
        printf("SHORT RANGE SENSORS ARE OUT\n");
        return;
    /*
        // Set condition to RED, YELLOW, or GREEN, depending on presence of Klingons and shields
    if (numKlingons > 0) {
    if ((*refGalaxy).enterprise.shields == 0) { (*refGalaxy).enterprise.condition = RED; }
    else { (*refGalaxy).enterprise.condition = YELLOW; }
    }
    else { (*refGalaxy).enterprise.condition = GREEN; }
        //TODO: The above "set condition" code should also happen in the NAV function. In fact, it might not belong here at all because the only time Condition changes is if the Enterprise moves or if Klingons are killed. So actually PHA and TOR need it too. But the condition should already be set when the SRS is executed. I'm going to comment it out and then try to find where it belongs in those other functions.
       */
    } else {
        printf("\n");
        if (KlingonsInQuadrant(refGalaxy,(*refGalaxy).enterprise.position) > 0) { printf("\nCOMBAT AREA\tCONDITION RED\n"); }
        if ((*refGalaxy).enterprise.shields <= 10) { printf("   SHIELDS DANGEROUSLY LOW\n"); }
    }
    printf("---------------------------------------------\n");
    // Get current enterprise quadrant location from enterprise coordinates
    int w = (*refGalaxy).enterprise.position[0];
    int x = (*refGalaxy).enterprise.position[1];
    int numKlingons = 0;
    // Iterate through current quadrant and print whatever is in each sector
    char srsStr[10][45];                                                                                    // create empty 2D char vector to serve as a "canvas"
    for (int i=0; i<9; i++) {
        for (int j=0; j<=44; j++) {
            if (j<=43)  { srsStr[i][j] = ' ';   }                                                           // fill char vector with empty spaces
            else        { srsStr[i][j] = '\0';  }
        }
    }
    for (int y = 1; y<=8; y++) {                                                                            // iterate through quadrant line-by-line, checking for entities
        for (int z = 1; z<=8; z++) { srsStr[y][(z-1) * 5 + 2] = (*refGalaxy).coordinates[w][x][y][z]; }
        if (KlingonsInQuadrant(refGalaxy, (*refGalaxy).enterprise.position) > 0) {
            for (int i=0;i<GAME_NUM_KLINGONS; i++) {
                if (((*refGalaxy).klingons[i].position[0] == (*refGalaxy).enterprise.position[0]) &&
                    ((*refGalaxy).klingons[i].position[0] == (*refGalaxy).enterprise.position[0])) {
                    if ((*refGalaxy).klingons[i].HP > 0) {                                                  // if living Klingon found, inject it into its character space on the canvas
                        char klingStr[5] = " +K+";                                                          //TODO: find out why extra klingons are showing up in SRS
                        // TODO: Could the for loop a few lines up iterate 1 through 3 instead of 1 through 26? I think that would limit how many klingons show up in a quadrant, but if too many exist in the quadrant it doesn't solve that problem. I haven't found where certain numbers are assigned to certain quadrants yet.
                        strinj(srsStr[(*refGalaxy).klingons[i].position[2]], 49, klingStr, strlen(klingStr), ((*refGalaxy).klingons[i].position[3]-1) * 5);
                    }
                }
            }
        }
        if (StarbasesInQuadrant(refGalaxy, (*refGalaxy).enterprise.position) > 0) {
            for (int i=0;i<GAME_NUM_STARBASES; i++) {
                if (((*refGalaxy).starbases[i].position[0] == (*refGalaxy).enterprise.position[0]) &&
                    ((*refGalaxy).starbases[i].position[0] == (*refGalaxy).enterprise.position[0])) {
                    if ((*refGalaxy).starbases[i].HP > 0) {                                                  // if intact starbase found, inject it into its character space on the canvas
                        char sbStr[5] = " >B<";
                        strinj(srsStr[(*refGalaxy).starbases[i].position[2]], 49, sbStr, strlen(sbStr), ((*refGalaxy).starbases[i].position[3]-1) * 5);
                    }
                }
            }
        }
        char eStr[5] = " <E>";                                                                             // inject the enterprise into its location within the canvas
        strinj(srsStr[(*refGalaxy).enterprise.position[2]], 49, eStr, strlen(eStr), (((*refGalaxy).enterprise.position[3]-1) * 5));
        printf(srsStr[y]);                                                                                  // draw canvas line
        switch(y) {
            char cond[10];
            case 1:                                                                                         // print additional SRS readout info, according to current line
                printf("\tSTARDATE:\t\t%d\n", ((*refGalaxy).gVitals.stardate));
                break;
            case 2:
                if ((*refGalaxy).enterprise.condition == GREEN)         { strcpy(cond,"*GREEN*");   }
                else if ((*refGalaxy).enterprise.condition == YELLOW)   { strcpy(cond,"*YELLOW*");  }
                else                                                    { strcpy(cond,"*RED*");     }
                printf("\tCONDITION:\t\t%s\n", cond);
                break;
            case 3:
                printf("\tQUADRANT:\t\t%d,%d\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1]);
                break;
            case 4:
                printf("\tSECTOR:\t\t\t%d,%d\n", (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]);
                break;
            case 5:
                printf("\tPHOTON TORPEDOES:\t%d\n", (*refGalaxy).enterprise.torpedoes);
                break;
            case 6:
                printf("\tTOTAL ENERGY:\t\t%d\n", (*refGalaxy).enterprise.energy);
                break;
            case 7:
                printf("\tSHIELDS:\t\t%d\n", (*refGalaxy).enterprise.shields);
                break;
            case 8:
                printf("\tKLINGONS REMAINING:\t%d\n", (*refGalaxy).gVitals.numKlingons);
                break;
            default:
                break;
        }
    }
    printf("---------------------------------------------\n");

    return;
}

void exeLRS(struct Galaxy* refGalaxy) {                                             //
    if((*refGalaxy).enterprise.sysDamage[2] < 0) {
        printf("LONG RANGE SENSORS ARE INOPERABLE.\n");
        return;
    }
    else {
        printf("LONG RANGE SCAN FOR QUADRANT %d, %d\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1]);
        printf("  ---------------------    \n");
        for (int w = (*refGalaxy).enterprise.position[0]-1; w <= (*refGalaxy).enterprise.position[0]+1; w++) {
            for (int x = (*refGalaxy).enterprise.position[1]-1; x <= (*refGalaxy).enterprise.position[1]+1; x++) {
                // Mark scanned quadrants as Explored
                (*refGalaxy).enterprise.explored[w][x] = 1;
                int starCounter = 0;
                /*
                int klingonCounter = 0;
                int starbaseCounter = 0;
                int starCounter = 0;
                */
                for (int y = 0; y < 8; y++) {
                    for (int z = 0; z < 8; z++) {
                        if ((*refGalaxy).coordinates[w][x][y][z] == '*') { starCounter++; }
                    }
                }
                int thisQuad[2] = {w, x};
                printf(" :  %d%d%d", KlingonsInQuadrant(refGalaxy,thisQuad), StarbasesInQuadrant(refGalaxy,thisQuad), starCounter);
            }
            printf("  :\n  ---------------------    \n");
        }
    }
    return;
}

void exeSLR(struct Galaxy* refGalaxy) {                                             //
    //struct Enterprise ePr = (*refGalaxy).enterprise;
    printf("LONG RANGE SCAN FOR ALL QUADRANTS:\n");
    printf("       1      2      3      4      5      6      7      8\n");
    printf("    --------------------------------------------------------    \n");
    for (int w = 1; w <=8; w++) {
        printf(" %d", w);
        for (int x = 1; x <=8; x++) {
            int starCounter = 0;
            for (int y = 1; y <= 8; y++) {
                for (int z = 1; z <= 8; z++) {
                    if ((*refGalaxy).coordinates[w][x][y][z] == '*') { starCounter++; }
                }
            }
            int thisQuad[2] = {w, x};
            printf(" :  %d%d%d", KlingonsInQuadrant(refGalaxy,thisQuad), StarbasesInQuadrant(refGalaxy,thisQuad), starCounter);
        }
        printf("  :\n    --------------------------------------------------------    \n");
    }
    printf("       I     II     III    IV      I     II     III    IV\n\n");
    return;
}

void exeDAM(struct Galaxy* refGalaxy) {
    char input;
    int damageCounter = 0;
    if((*refGalaxy).enterprise.sysDamage[5] >= 0) {
        printf("\nDEVICE\t\t\t  STATE OF REPAIR\n");
        printf("WARP ENGINES\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[0]);
        printf("SHORT RANGE SENSORS\t\t%d\n", (*refGalaxy).enterprise.sysDamage[1]);
        printf("LONG RANGE SENSORS\t\t%d\n", (*refGalaxy).enterprise.sysDamage[2]);
        printf("PHASER CONTROL\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[3]);
        printf("PHOTON TUBES\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[4]);
        printf("DAMAGE CONTROL\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[5]);
        printf("SHIELD CONTROL\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[6]);
        printf("LIBRARY - COMPUTER\t\t%d\n", (*refGalaxy).enterprise.sysDamage[7]);
        printf("\n\n");
    }
    else {
        printf("DAMAGE CONTROL REPORT NOT AVAILABLE\n");
        return;
    }
    for (int i = 0; i < 8; i++) {
        // Check for damage on each of 8 items in enterprise.damage array
        if ((*refGalaxy).enterprise.sysDamage[i] < 0) {
            damageCounter++;    // Increment damageCounter if any part is damaged
        }
    }
    if (damageCounter <= 0) {       // if no parts of Enterprise are damaged
        return;
    }
    printf("\nTECHNICIANS STANDING BY TO EFFECT REPAIRS TO YOUR SHIP\n");
    printf("ESTIMATED TIME TO REPAIR : %d STARDATES\n", damageCounter);
    printf("WILL YOU AUTHORIZE THE REPAIR ORDER? (Y/N) ");
    scanf("%c", &input);
    getchar();
    if(input != 'Y' || input != 'y') {
        return;
    }
    // Cycle through each item in enterprise.damage array
    for(int i = 0; i < 8; ++i) {
        // if any member in the array < 0, set it equal to 0 and decrement stardate counter
        if((*refGalaxy).enterprise.sysDamage[i] < 0) {
            (*refGalaxy).enterprise.sysDamage[i] = 0;
            (*refGalaxy).gVitals.stardate--;
        }
    }
    return;
}

void exeCOM(struct Galaxy* refGalaxy) {
    int starbaseCounter;
    printf("Computer active and awaiting command: ");
    char command = getchar();
    getchar(); // Extra getchar to get rid of the newline

    // Switch branching statement to execute each possible computer command
    switch(command) {
        case ('0'):
            printf("\n   ----   ----   ----   ----   ----   ----   ----   ----\n");
            // Count klingons, starbases, and stars in Explored quadrants
            int klingonCounter = 0;
            int starbaseCounter = 0;
            int starCounter = 0;
            for (int w = 0; w < 8; w++) {
                for (int x = 0; x < 8; x++) {
                    if ((*refGalaxy).enterprise.explored[w][x] == 1) {
                        for (int y = 0; y < 8; y++) {
                        for (int z = 0; z < 8; z++) {
                            if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                                klingonCounter = klingonCounter + 1;
                            }
                            if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                                starbaseCounter = starbaseCounter + 1;
                            }
                            if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                                starCounter = starCounter + 1;
                            }
                        }
                        }
                        // Print quantities of elements in explored quadrants
                        printf("    %d%d%d", klingonCounter, starbaseCounter, starCounter);
                        klingonCounter = 0;         // Set counters back to 0 before counting next quadrant
                        starbaseCounter = 0;
                        starCounter = 0;
                        }
                    // Otherwise print *** for unexplored quadrants
                    else {
                        printf("    ***");
                    }
                }
                printf("\n   ----   ----   ----   ----   ----   ----   ----   ----\n");
            }
            break;
        case ('1'):
            printf("\STATUS REPORT:\n");
            printf("KLINGONS LEFT: %d\n", (*refGalaxy).gVitals.numKlingons);
            printf("MISSION MUST BE COMPLETED IN %d STARDATES\n", (*refGalaxy).gVitals.stardate);
            printf("THE FEDERATION IS MAINTAINING %d STARBASES IN THE GALAXY\n", (*refGalaxy).gVitals.numStarbases);
            printf("\nDEVICE           STATE OF REPAIR\n");
            printf("WARP ENGINES         %d\n", (*refGalaxy).enterprise.sysDamage[0]);
            printf("SHORT RANGE SENSORS  %d\n", (*refGalaxy).enterprise.sysDamage[1]);
            printf("LONG RANGE SENSORS   %d\n", (*refGalaxy).enterprise.sysDamage[2]);
            printf("PHASER CONTROL       %d\n", (*refGalaxy).enterprise.sysDamage[3]);
            printf("PHOTON TUBES         %d\n", (*refGalaxy).enterprise.sysDamage[4]);
            printf("DAMAGE CONTROL       %d\n", (*refGalaxy).enterprise.sysDamage[5]);
            printf("SHIELD CONTROL       %d\n", (*refGalaxy).enterprise.sysDamage[6]);
            printf("LIBRARY COMPUTER     %d\n", (*refGalaxy).enterprise.sysDamage[7]);
            printf("\n\n");
            break;
        case ('2'):
            printf("FROM ENTERPRISE TO KLINGON BATTLE CRUISER\n");
            printf("DIRECTION = \n");                                                                                       //TODO: check for INT direction values in COM, change to double
            printf("DISTANCE = \n");
            // TODO: Add code here to calculate distance. I'm not sure how they decide which klingon to calculate the distance to. Maybe the nearest one?
            break;
        case ('3'):
            // Initialize starbaseCounter to check current quadrant for starbases
            starbaseCounter = 0;
            // Check enterprise position quadrant against the other items in this sector
            int w = (*refGalaxy).enterprise.position[0];
            int x = (*refGalaxy).enterprise.position[1];
            for (int y = 0; y < 8; y++) {
                for (int z = 0; z < 8; z++) {
                    if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                    starbaseCounter = starbaseCounter + 1;
                    }
                }
            }
            printf("MR. SPOCK REPORTS, ""SENSORS SHOW %d STARBASES IN THIS QUADRANT.""\n", starbaseCounter);
            break;
        case ('4'):
            printf("DIRECTION/DISTANCE CALCULATOR\n");
            // Print current location
            printf("YOU ARE AT QUADRANT %d, %d, SECTOR %d, %d\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]);
            // Ask user for destination coordinates
            printf("PLEASE ENTER \n\tINITIAL COORDINATES(X,Y): \n");
            int destination[4];
            scanf("%d,%d", &destination[0], &destination[1]);
            printf("\tFINAL COORDINATES(X,Y): \n");
            scanf("%d,%d", &destination[2], &destination[3]);
            getchar();   // Extra getchar to get rid of newline in input buffer
            // Call functions to calculate distance and direction
            double distance = getDist(refGalaxy, destination);
            double direction = getDirection(refGalaxy, destination);
                    // TODO: I messed these up. It's not getting directions from the Enterprise to somewhere else, but from one random spot to another random spot. WHY?!
            // Print distance and direction to screen
            printf("DIRECTION = %f\n", direction);
            printf("DISTANCE = %f\n", distance);

            break;
        case ('5'):
            printf("                        THE GALAXY\n");
            printf("    1      2      3      4      5      6      7      8\n");
            printf("1           ANTARES          |           SIRIUS\n");
            printf("2           RIGEL            |           DENEB\n");
            printf("3           PROCYON          |           CAPELLA\n");
            printf("4           VEGA             |           BETELGEUSE\n");
            printf("5           CANOPUS          |           ALDEBARAN\n");
            printf("6           ALTAIR           |           REGULUS\n");
            printf("7           SAGITTARIUS      |           ARCTURUS\n");
            printf("8           POLLUX           |           SPICA\n");
            break;
        default:
            printf("UNRECOGNIZED COMMAND. ENTER 0 - 5.\n");
            break;
    }
    return;
}

void exePHA(struct Galaxy* refGalaxy) {                                             // TODO: REVIEW PHA BLOCK
  int klingonCounter = 0;
  int unitsToFire, H, H1;
    if((*refGalaxy).enterprise.sysDamage[3] < 0) {
      printf("PHASERS INOPERATIVE\n\n");
      return;
    }
    // Check whether there are klingons in this quadrant
    int KlingonCount = KlingonsInQuadrant(refGalaxy, (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1]);
    if (KlingonCount <= 0) {      // If there are no klingons in quadrant, print message and return
        printf("SCIENCE OFFICER SPOCK REPORTS 'SENSORS SHOW NO ENEMY SHIPS\n IN THIS QUADRANT'\n");
        return;
    }
    // Otherwise, find klingons and fire phasers
    if((*refGalaxy).enterprise.sysDamage[7] < 0) {     // Check if computer is damaged
        printf("COMPUTER FAILURE HAMPERS ACCURACY\n");
        }
        printf("PHASERS LOCKED ON TARGET\n");
          do{
            printf("ENERGY AVAILABLE = %d UNITS\n", (*refGalaxy).gVitals.eEnergy);
            printf("NUMBER OF UNITS TO FIRE? ");
            scanf("%d", &unitsToFire);
              // Check unitsToFire is greater than 0
            if (unitsToFire <= 0) {
                return;
            }
              // Check that there is enough energy available to fire requested unitsToFire
          } while((*refGalaxy).gVitals.eEnergy - unitsToFire > 0);
          (*refGalaxy).gVitals.eEnergy = (*refGalaxy).gVitals.eEnergy - unitsToFire;
    // TODO: I need some help with this next part. I think what happens next depends on klingonCounter, unitsToFire, shields (I think because if shields are low and there are 2 klingons then one fires at the enterprise?). But I don't understand the Source Code math and I'm not sure how to figure it out on my own. Also sometimes the variable x shows up below - I changed x above to unitsToFire just to have more descriptive variable names and because I liked using [w][x][y][z] for position arrays.
    /*if((*refGalaxy).enterprise.damage[6] < 0) {
            x = x * rand()%1;
          }
          else {
            H1 = x / klingonCounter;
            for(int i = 0; i < 8; ++i) {
              for(int j; j < 8; ++j) {
                if((*refGalaxy).coordinates[a][b][i][j] != 'K') {
                  continue;
                }
                else if((*refGalaxy).coordinates[a][b][i][j] == 'K') {
                  //H = (H1 / FND(0) * (rand() + 2));
                  if(H > (.15 * (*refGalaxy).klingons.position[0])) { //Check line
                    k[i][3] = k[i][3] - H;                                                                                                                //Fix lines below
                    printf("UNIT HIT ON KLINGON AT SECTOR %d,%d\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1]));
                    if(k[i][3] <= 0) {
                      printf("*** KLINGON DESTROYED ***");
                      klingonCounter = klingonCounter - 1;
                      (*refGalaxy).gVitals.numKlingons = (*refGalaxy).gVitals.numKlingons - 1;
                      Z1 = k[i][1];
                      Z2 = k[i][2];
                      k[i][3] = 0;
                      G(Q1,Q2) = G(Q1,Q2) - 100;
                      Z(Q1,Q2) = G(Q1,Q2);
                    }
                    printf("SENSORS SHOW %d UNITS REMAINING", k[i][3]);
                  }
                  else {
                    printf("SENSORS SHOW NO DAMAGE TO ENEMY AT SECTOR %d, %d",(*refGalaxy).klingons.position[0], (*refGalaxy).enterprise.position[1]);
                  }
                }
            }
        }
      return;
    }
*/
    return;

}

void exeTOR(struct Galaxy* refGalaxy) {
    // If torpedoes are used up
    if ((*refGalaxy).enterprise.torpedoes < 1) {
        printf("ALL PHOTON TORPEDOES EXPENDED.\n");
        return;
    }
    // If photon tubes are damaged
    if ((*refGalaxy).enterprise.sysDamage[4] < 0) {
        printf("PHOTON TUBES ARE NOT OPERATIONAL.\n");
        return;
    }
    printf("INPUT PHOTON TORPEDO COURSE (1-9): ");
    float course = 0.0;
    scanf("%f", &course);
    getchar();   // Get extra newline character from input buffer
    if ((course < 1) || (course > 9)) {
        printf("ENSIGN CHEKOV REPORTS, ""INCORRECT COURSE DATA, SIR!""\n");
        return;
    }
    // Decrement numberTorpedos:
    (*refGalaxy).enterprise.torpedoes--;
    // TODO: Change torpedo function here. Direction is a float, not an int, so case/switch statement is inappropriate.
    // Assign coordinates to current position of enterprise
    int w = (*refGalaxy).enterprise.position[0];
    int x = (*refGalaxy).enterprise.position[1];
    int y = (*refGalaxy).enterprise.position[2];
    int z = (*refGalaxy).enterprise.position[3];
    /*switch (course) {
        case (1):           // Direction 1: EAST
            z++;
            if (z < 8) {
            printf("TORPEDO TRACK:\n");
            }
            while (z < 8) {
                printf("            %d, %d\n", y+1, z+1);
                if (z == 7) {
                    printf("TORPEDO MISSED!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("KLINGON DESTROYED\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("STAR AT %d, %d ABSORBED TORPEDO ENERGY\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("STARBASE DESTROYED\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    z++;
                }
            }
            break;
        case (2):               // Direction 2: NORTHEAST
            z++;
            y--;
            if ((z < 8) && (y > 0)) {
            printf("Torpedo Track:\n");
            }
            while ((z < 8) && (y >= 0)) {
                printf("            %d, %d\n", y+1, z+1);
                if ((z == 7) || (y == 0))  {
                    printf("Torpedo missed!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("Klingon destroyed\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("Star at %d, %d absorbed torpedo energy\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("Starbase destroyed\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    z++;
                    y--;
                }
            }
            break;
        case (3):               // DIRECTION 3: NORTH
            y--;
            if (y > 0) {
            printf("Torpedo Track:\n");
            }
            while (y >= 0) {
                printf("            %d, %d\n", y+1, z+1);
                if (y == 0) {
                    printf("Torpedo missed!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("Klingon destroyed\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("Star at %d, %d absorbed torpedo energy\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("Starbase destroyed\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    y--;
                }
            }
            break;
        case (4):               // DIRECTION 4: NORTHWEST
            y--;
            z--;
            if ((z > 0) && (y > 0)) {
            printf("Torpedo Track:\n");
            }
            while ((y >= 0) && (z >= 0)) {
                printf("            %d, %d\n", y+1, z+1);
                if ((z == 0) || (y == 0)) {
                    printf("Torpedo missed!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("Klingon destroyed\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("Star at %d, %d absorbed torpedo energy\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("Starbase destroyed\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    z--;
                    y--;
                }
            }
            break;
        case (5):               // DIRECTION 5: WEST
            z--;
            if (z > 0) {
            printf("Torpedo Track:\n");
            }
            while (z >= 0) {
                printf("            %d, %d\n", y+1, z+1);
                if (z == 0) {
                    printf("Torpedo missed!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("Klingon destroyed\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("Star at %d, %d absorbed torpedo energy\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("Starbase destroyed\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    z--;
                }
            }
            break;
        case (6):               // DIRECTION 6: SOUTHWEST
            z--;
            y++;
                if ((y < 8) && (z > 0)) {
                printf("Torpedo Track:\n");
                }
            while ((z >= 0) && (y < 8)) {
                printf("            %d, %d\n", y+1, z+1);
                if ((z == 0) || (y == 7)) {
                    printf("Torpedo missed!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("Klingon destroyed\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("Star at %d, %d absorbed torpedo energy\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("Starbase destroyed\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    z--;
                    y++;
                }
            }
            break;
        case (7):               // DIRECTION 7: SOUTH
            y++;
            if (y < 8) {
            printf("Torpedo Track:\n");
            }
            while (y < 8) {
                printf("            %d, %d\n", y+1, z+1);
                if (y == 7) {
                    printf("Torpedo missed!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("Klingon destroyed\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("Star at %d, %d absorbed torpedo energy\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("Starbase destroyed\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    y++;
                }
            }
            break;
        case (8):               // DIRECTION 8: SOUTHEAST
            y++;
            z++;
            if ((z < 8) && (y < 8)) {
            printf("Torpedo Track:\n");
            }
            while ((z < 8) && (z < 8)) {
                printf("            %d, %d\n", y+1, z+1);
                if ((y == 7) || (z == 7)) {
                    printf("Torpedo missed!\n");
                    return;
                }
                if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                    printf("Klingon destroyed\n");
                    // Turn Klingon symbol into a blank symbol
                    (*refGalaxy).coordinates[w][x][y][z] = ' ';
                    // Decrement numKlingons
                    (*refGalaxy).gVitals.numKlingons--;
                    return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == '*') {
                printf("Star at %d, %d absorbed torpedo energy\n", y+1, z+1);
                return;
                }
                else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                printf("Starbase destroyed\n");
                (*refGalaxy).coordinates[w][x][y][z] = ' ';
                // Decrement numStarbases
                (*refGalaxy).gVitals.numStarbases--;
                // TODO: So far this only decrements numStarbases, but if player destroys all 3 starbases some other stuff happens and game ends. Should that go in gameEnd or here? See Source Code 5360.
                return;
                }
                else {
                    y++;
                    z++;
                }
            }
            break;
        default:               // If none of these cases
            printf("Error. Unrecognized command.\n");
            break;
    }
     */
    return;
}

void exeSHE(struct Galaxy* refGalaxy) {
  //User input for amount of energy to allocate to shields
    int x;
    if((*refGalaxy).enterprise.sysDamage[6] < 0) {
      printf("SHIELD CONTROL INOPERABLE\n\n");
      return;
    }
    printf("ENERGY AVAILABLE = %d\n", ((*refGalaxy).enterprise.energy + (*refGalaxy).enterprise.shields));
    printf("INPUT NUMBER OF UNITS TO SHIELDS? ");
    scanf("%d", &x);
    getchar();      // To get rid of the extra newline in input buffer
    if(x < 0 || (*refGalaxy).enterprise.shields == x) {
      printf("\n<SHIELDS UNCHANGED>\n\n");
      return;
    }
    if(x <= ((*refGalaxy).enterprise.energy + (*refGalaxy).enterprise.shields)) {
      (*refGalaxy).enterprise.energy = (*refGalaxy).enterprise.energy + (*refGalaxy).enterprise.shields - x;
      (*refGalaxy).enterprise.shields = x;
      printf("DEFLECTOR CONTROL ROOM REPORT : \n");
      printf("SHIELDS NOW AT %d UNITS PER YOUR COMMAND\n\n", (*refGalaxy).enterprise.shields);
      return;
    }
    printf("SHIELD CONTROL REPORTS 'THIS IS NOT THE FEDEREATION TREASURY'\n");
    printf("<SHIELDS UNCHANGED>\n\n");
    return;
}

void exeDBG(struct Galaxy* refGalaxy) {                                             // DEBUG: "secret" debug subroutine
    printf("\nACCESSING DEBUG SUBROUTINES...\n");
    printf("CREDENTIALS AUTHENTICATED...\n");
    printf("UNIVERSE ADMIN ACCESS GRANTED!\n\n");
    printf("GOD COMMANDS UNLOCKED:\n");
    printf("\t+EN: ADD ENTERPRISE ENERGY\n");
    printf("\t+TP: ADD ENTERPRISE TORPEDOES\n");
    printf("\t+RP: REPAIR ALL SYSTEMS\n");
    printf("\t+OV: OVERPOWER ALL SYSTEMS\n");
    printf("\t+LR: SUPER-LONG-RANGE SENSORS\n\n");
    printf("GAME VITALS STATUS:\n");
    printf("STARBASES REMAINING:\t%d\n", (*refGalaxy).gVitals.numStarbases);
    printf("KLINGONS REMAINING:\t%d\n", (*refGalaxy).gVitals.numKlingons);
    printf("KLINGONS IN QUADRANT:\t%d\n", KlingonsInQuadrant(refGalaxy,(*refGalaxy).enterprise.position));
    printf("STARDATE\t\t%d\n", (*refGalaxy).gVitals.stardate);
    printf("USER HAS QUIT:\t\t");
    if ((*refGalaxy).gVitals.userQuit) { printf("TRUE\n\n");
    } else { printf("FALSE\n\n"); }
    printf("\nENTERPRISE STATUS:\n");
    printf("POSITION-\n  QUADRANT:\t(%d,%d)\n  SECTOR:\t(%d,%d)\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]);
    printf("ENERGY:\t\t%4.1f\n", (*refGalaxy).enterprise.energy);
    printf("SHIELDS:\t%4.1f\n", (*refGalaxy).enterprise.shields);
    printf("TORPEDOES:\t%4d\n\n", (*refGalaxy).enterprise.torpedoes);
    printf("(N-TH CLOSEST TEST:)\n");
    if (KlingonsInQuadrant(refGalaxy,(*refGalaxy).enterprise.position) > 0) { getNthClosestKlingon(refGalaxy,1); }
    if (KlingonsInQuadrant(refGalaxy,(*refGalaxy).enterprise.position) > 0) { getNthClosestKlingon(refGalaxy,2); }
    if (KlingonsInQuadrant(refGalaxy,(*refGalaxy).enterprise.position) > 0) { getNthClosestKlingon(refGalaxy,KlingonsInQuadrant(refGalaxy,(*refGalaxy).enterprise.position)); }
    printf("\n");
    return;
}


void setDest(int* _start, double dir, double dist, int* _destination) {             // Using starting coordinates, direction (NAV number), and distance, it sets destination coordinates accordingly
    int sStart[2] = {(_start[0] * 8 + _start[2]), (_start[1] * 8 + _start[3])};     // For simplicity of calculation, convert (Qx,Qy,Sx,Sy) format to an equivalent (Sx,Sy)
    int sEnd[2] = {sStart[0], sStart[1]};
    int sDiff[2] = {0, 0};
    int lclDiff[4] = {0, 0, 0, 0};

    double theta = ((dir - 1.0) * -2 * PI) / 8.0;
    sDiff[0] = round(dist * 8 * sin(theta));
    sDiff[1] = round(dist * 8 * cos(theta));

    if (((sStart[0] + sDiff[0]) > 64) || ((sStart[0] + sDiff[0]) < 1) || ((sStart[1] + sDiff[1]) > 64) || ((sStart[1] + sDiff[1]) < 1)) {
        printf("LT. UHURA REPORTS MESSAGE FROM STARFLEET COMMAND: \n");
        printf("'PERMISSION TO ATTEMPT CROSSING OF GALACTIC PERIMETER\n");
        printf("IS HEREBY *DENIED*. SHUT DOWN YOUR ENGINES.' CHIEF\n");
        printf("ENGINEER SCOTT REPORTS 'WARP ENGINES SHUT DOWN AT\n");
        printf("SECTOR %d,%d OF QUADRANT %d,%d'\n\n", _start[2],_start[3], _start[0],_start[1]);
        return;
    }

    sEnd[0] = sStart[0] + sDiff[0];
    sEnd[1] = sStart[1] + sDiff[1];
    lclDiff[0] = floor(sEnd[0] / 8) - floor(sStart[0] / 8);
    lclDiff[1] = floor(sEnd[1] / 8) - floor(sStart[1] / 8);
    lclDiff[2] = (sEnd[0] % 8) - (sStart[0] % 8);
    lclDiff[3] = (sEnd[1] % 8) - (sStart[1] % 8);

    _destination[0] = _start[0] + lclDiff[0];
    _destination[1] = _start[1] + lclDiff[1];
    _destination[2] = _start[2] + lclDiff[2];
    _destination[3] = _start[3] + lclDiff[3];
    return;
}

double getDirection(struct Galaxy* refGalaxy, int* destination) {             // Using starting coordinates, direction, and destination, calculates direction
    int y = ((destination[0]*8)+destination[2]) - (((*refGalaxy).enterprise.position[0]*8)+(*refGalaxy).enterprise.position[2]);
    int x = ((destination[1]*8)+destination[3]) - (((*refGalaxy).enterprise.position[1]*8)+(*refGalaxy).enterprise.position[3]);
    double theta = 0;       // Initialize theta to hold angle of direction
    // Figure out what quadrant theta falls into and calculate angle
    if (x >= 0) {
        if (y >= 0) {
            theta = asin((double)y/x);
        }
        else if (y < 0) {
            y = abs(y);
            theta = asin((double)y/x);
            theta = 360 - theta;
        }
    }
    else if (x < 0) {
        if (y >= 0) {
            x = abs(x);
            theta = asin((double)y/x);
            theta = 180 - theta;
        }
        else {
            x = abs(x);
            y = abs(y);
            theta = asin((double)y/x);
            theta = 180 + theta;
        }
    }
    // Convert angle into 1-9 game notation (1 = East = positive x-axis)
    double direction = (theta / 45) + 1;
    return direction;
}


double checkObstacles(int* _start, double dir, double dist, struct Galaxy* refGalaxy) {         // iterates through parameters' travel path to check for obstacles, returns furthest good coordinate if found
    for (double i=0.1; i<=dist; i+=0.1) {
        int chkCoords[4] = {0, 0, 0, 0};
        setDest(_start, dir, i, chkCoords);
        if ((*refGalaxy).coordinates[chkCoords[0]][chkCoords[1]][chkCoords[2]][chkCoords[3]] != ' ') { return (i - 0.1); }
    }
    return dist;
}

double getDist(struct Galaxy* refGalaxy, int* destination) {
    double yDist = abs(((*refGalaxy).enterprise.position[0]*8)+(*refGalaxy).enterprise.position[2]) - ((destination[0]*8)+destination[2]);
    double xDist = abs(((*refGalaxy).enterprise.position[1]*8)+(*refGalaxy).enterprise.position[3]) - ((destination[1]*8)+destination[3]);
    return (sqrt(pow(xDist,2) + pow(yDist,2)));
}

// CLUES:
//
// � N=INT(W1*8+.5) : IF E-N>=0 THEN 2590                                                       (Where E=energy, W1=warp speed)
//
// � H=INT((K(I,3)/FND(1))*(2+RND(1))) : S=S-H : K(I,3)=K(I,3)/(3+RND(0))                       (Klingon firing equation)

void KlingonsFireMT(struct Galaxy* refGalaxy, double WS) {                                      //TODO: Klingon firing behavior when player flies through their occupied quadrant
    int klingonsHere = KlingonsInQuadrant(refGalaxy, (*refGalaxy).enterprise.position);

    return;
}

struct Klingon* getNthClosestKlingon(struct Galaxy* refGalaxy, int n) {                         // returns a pointer to the nth-closest klingon to the enterprise within its quadrant
    struct Enterprise* ePr = &((*refGalaxy).enterprise);
    if (KlingonsInQuadrant(refGalaxy, (*ePr).position) < 1) { return NULL; } // Nothing to return!
    int kCount = 0;
    double distIs[GAME_NUM_KLINGONS][2];                                                        // make a 2-column list: each contains the distance of a klingon and its index in the galaxy array
    double tmpcrry[2];                                                                          // temp space for sorting
    for (int i=0; i<GAME_NUM_KLINGONS; i++) {                                                   // clear array values
        distIs[i][0] = 0.0;
        distIs[i][1] = 0.0;
    }
    for (int i=0; i<GAME_NUM_KLINGONS; i++) {                                                   // check for valid klingons in quadrant
        if (((*refGalaxy).klingons[i].position[0] == (*ePr).position[0]) &&
            ((*refGalaxy).klingons[i].position[1] == (*ePr).position[1]) &&
            ((*refGalaxy).klingons[i].HP > 0))  {
            double thisDist = getDist(refGalaxy,(*refGalaxy).klingons[i].position);             // get desired info from them
            double thisI = (double)i;
            for (int j=0;j<kCount; j++) {                                                       // TODO: Fix last element in array not being sorted properly
                if (thisDist > distIs[j][0]) {                                                  // Sort!!
                    tmpcrry[0] = distIs[j][0];
                    tmpcrry[1] = distIs[j][1];
                    distIs[j][0] = thisDist;
                    distIs[j][1] = thisI;
                    thisDist = tmpcrry[0];
                    thisI = tmpcrry[1];
                    tmpcrry[0] = -1;
                    tmpcrry[1] = -1;
                } //else if (((j+1) == kCount) && tmpcrry > -1) {
                    //distIs[j][0] = tmpcrry[0];
                    //distIs[j][1] = tmpcrry[1];
                //}
            }
            kCount++;                                                                           // keep track of how many have qualified
        }
    }
    for (int i=0; i<kCount; i++) {
                                                                                                printf("list row %d: %f,%f\n", i, distIs[i][1], distIs[i][0]); //DEBUG
    }
    return &((*refGalaxy).klingons[(int)distIs[n-1][1]]);                                       // return the pointer to the klingon that matches the specified precedence
}

int qsortByDist(int coordinates[2]) {
    return coordinates[2];
}

void KlingonsFire(struct Galaxy* refGalaxy) {                                                   //TODO: Klingon firing behavior when player fires on them or performs other actions
    return;
}

int StarbasesInQuadrant(struct Galaxy* refGalaxy, int* Q) {                         // checks enterprise's current quadrant and returns number of living starbases within
    int SBCount = 0;
    for (int i=0; i<GAME_NUM_STARBASES; i++) {
        if (((*refGalaxy).starbases[i].position[0] == Q[0]) &&
            ((*refGalaxy).starbases[i].position[1] == Q[1]) &&
            ((*refGalaxy).starbases[i].HP > 0))  { SBCount++; }
    }
    return SBCount;
}

int KlingonsInQuadrant(struct Galaxy* refGalaxy, int* Q) {                          // checks enterprise's current quadrant and returns number of living Klingons within
    int KlingonCount = 0;
    for (int i=0; i<GAME_NUM_KLINGONS; i++) {
        if (((*refGalaxy).klingons[i].position[0] == Q[0]) &&
            ((*refGalaxy).klingons[i].position[1] == Q[1]) &&
            ((*refGalaxy).klingons[i].HP > 0))  { KlingonCount++; }
    }
    return KlingonCount;
}

void strtrim(char* string, int n) {                                                 // Checks through string for leading whitespace, then copies characters to the string start, then ends string after n chars
    for (int i=0; i<n; i++) {
        if (string[i] == '\0') {
            return;
        } else if (string[i] != ' ') {
            for (int j=0; j<(n-i); j++) { string[j] = string[j+i]; }
            string[n-i] = '\0';
            return;
        }
    }
    return;
}

void strinj(char* parstr, int parsiz, char* chldstr, int chldlen, int idx) {        // injects child string into parent string at index idx, overwriting any characters from the parent (Checks first against overwriting end of parent string)
    if ((idx + chldlen) >= parsiz) { return; }
    else {
        for (int i=0; i<chldlen; i++) {
            parstr[idx+i] = chldstr[i];
        }
    }
    return;
}

void remNL(char* string, int n) {                                                   // Checks the end of the string parameter for the newline character and, if found, removes it
    if (string[n-1] == '\n') { string[n-1] = '\0'; }
    return;
}

void strToLower(char* string, int n) {                                              // Loops through string parameter, changing each index to its lowercase version
    for (int i=0; i<n; i++) { string[i] = tolower(string[i]); }
    return;
}

void strToUpper(char* string, int n) {                                              // Loops through string parameter, changing each index to its uppercase version
    for (int i=0; i<n; i++) { string[i] = toupper(string[i]); }
    return;
}

void displayManual() {
    printf("\n\n1. You are the captain of the starship ''Enterprise'' with a mission to seek and destroy a fleet of Klingon warships (usually about 17) which are menacing the United Federation of Planets. You have a specified number of stardates in which to complete your mission. You also have two or three Federation Starbases for resupplying your ship.\n\n");
    printf("2. You will be assigned a starting position somewhere in the galaxy. The galaxy is divided into an 8 x 8 quadrant grid. The astronomical name of a quadrant is called out upon entry into a new region. (See ''Quadrant Nomenclature.'') Each quadrant is further divided into an 8 x 8 section grid.\n\n");
    printf("3. On a section diagram, the following symbols are used:\n\n");
    printf("\t<E>\tEnterprise\t\t>!<\tStarbase\n\t+K+\tKlingon\t\t\t*\tStar\n\n");
    printf("4. You have eight commands available to you. (A detailed description of each command is given in the program instructions.)\n\n");
    printf("\tNAV\tNavigate the Starship by setting course and warp engine speed.\n\tSRS\tShort-range sensor scan (one quadrant)\n\tLRS\tLong-range sensor scan (9 quadrants)\n\tPHA\tPhaser control (energy gun)\n\tTOR\tPhoton torpedo control\n\tSHE\tShield control (protects against phaser fire)\n\tDAM\tDamage and state-of-repair report\n\tCOM\tCall library computer\n\n");
    printf("When setting a course with the NAV command, direction is indicated by a floating point number from 1.0 to 9.0, according to the following convention:\n");
    printf("\t  4  3  2\n");
    printf("\t   \ | / \n");
    printf("\t    \|/  \n");
    printf("\t  5--*--1\n");
    printf("\t    /|\  \n");
    printf("\t   / | \ \n");
    printf("\t  6  7  8\n");
    printf("\t(1.0 == 9.0)\n");
    printf("Non-whole numbers interpolate between directions. E.g. '1.5' would direct halfway between the 1.0 and 2.0 directions.(1.0 == 9.0)\n");
    printf("5. Library computer options are as follows (more complete descriptions are in program instructions):\n\n");
    printf("\t0\tCumulative galactic record\n\t1\tStatus report\n\t2\tPhoton torpedo course data\n\t3\tStarbase navigation data\n\t4\tDirection/distance calculator\n\t5\tQuadrant nomenclature map\n\n");
    printf("6. Certain reports on the ship's status are made by officers of the Enterprise who appeared on the original TV show - Spock, Scott, Uhura, Chekov, etc.\n\n");
    printf("7. Klingons are non-stationary within their quadrants. If you try to maneuver on then, they will move and fire on you.\n\n");
    printf("8. Firing and damage notes:\n\n\tA. Phaser fire diminishes with increased distance between combatants.\n\tB. If a Klingon zaps you hard enough (relative to your shield strength) he will generally cause damage to some part of your ship with an appropriate ''Damage Control'' report resulting.\n\tC. If you don't zap a Klingon hard enough (relative to his shield strength) you won't damage him at all. Your sensors will tell the story.\n\tD. Damage control will let you know when out-of-commission devices have been completely repaired.\n\n");
    printf("9. Your engines will automatically shut down if you should attempt to leave the galaxy, or if you should try to maneuver through a star, a Starbase, or - heaven help you - a Klingon warship.\n\n");
    printf("10. In a pinch, or if you should miscalculate slightly, some shield control energy will be automatically diverted to warp engine control (if your shields are operational!)./n/n");
    printf("11. While you're docked at a Starbase, a team of technicians can repair your ship (if you're willing for the mto spend the time required - and the repairmen always underestimate...)\n\n");
    printf("12. If, to save maneuvering time toward the end of the gane, you should cold-bloodedly destroy a Starbase, you get a nasty note from Starfleet Command. If you destroy your last Starbase, you lose the game!\n\n");
    printf("13. End game logic has been ''cleaned up'' in several spots, and it is possible to get a new command after successfully completing your mission (or, after resigning your old one).\n\n\n");
    printf("\t(Enter game command 'MAN' at any time to see this manual again)\n\n");
            // NOTE: "Usually about 17 Klingons! So there's a MAX of 26 but not necessarily always 26?!!!
            // NOTE: Please see note 10.
            // NOTE: Please see note 11 about decrementing Stardate if repairs happen
            // NOTE: Please see note 12. I think this explains the "reputation" factor. Which makes me think it's not a game vital but just a nicer option than losing the game?
            // NOTE: I deleted notes 14 and 15 because they were hardware-specific (alarm bells on a Westinghouse 1600 and information about chain instructions. Interesting but not relevant to us.
    return;
}
