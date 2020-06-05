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
#define GAME_NUM_STARS 100
#define GAME_INITIAL_ENERGY 3000
#define GAME_INITIAL_STARDATE 2700
#define GAME_STARDATE_LIMIT 26

// enum declarations:
typedef enum {GREEN, YELLOW, RED} Condition;

//struct declarations:
struct gameVitals {
  //NOTE: eDamage removed from gameVitals
  int eEnergy;
  int numStarbases;
  int numKlingons;
  double stardate;
  int reputation;
  bool userQuit;
};

struct Starbase {
  int position[4];
  int damage;
  int energy;
  int shields;
};

struct Klingon {
  int position[4];
  int damage; // TODO: Should these have initial values?
  int energy;
  int shields;
};

struct Enterprise {
  int position[4];
  int explored[8][8];
  int sysDamage[8];                                         //NOTE: enterprise.damage[] renamed to: enterprise.sysDamage[]
  int energy;
  int shields;
  int torpedoes;
  Condition condition;
};

struct Galaxy {
  char coordinates[8][8][8][8];
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
void setDest(int* _start, double dir, double dist, int* _destination);
void KlingonsFire(struct Galaxy *refGalaxy);
bool KlingonsInQuadrant(struct Galaxy* refGalaxy);
void strtrim(char* string, int n);
void remNL(char* string, int n);
void strToLower(char* string, int n);
void strToUpper(char* string, int n);

////////////////////////////////////////////
//////////////// START MAIN ////////////////
int main() {
    bool gameRunning = true;
    struct Galaxy theGalaxy;
    theGalaxy = gameIntro();
    theGalaxy.gVitals = getGameVitals(&theGalaxy);

    int safetyCounter = 0;
    while (gameRunning && (safetyCounter <= 1000)) {                // Main game loop:
        getCommand(&theGalaxy);
        gameRunning = gameEnd(&theGalaxy);
        safetyCounter++;
    }
    return 0;                                                                     // indicates normal return from main; i.e. no errors
}
///////////////// END MAIN /////////////////
////////////////////////////////////////////


//function definitions:
struct Galaxy createGalaxy(void) {
    srand((int)time(0));                                        // Seed rand function with current time
    int numStarbases = GAME_NUM_STARBASES;
    int numKlingons = GAME_NUM_KLINGONS;
    int numStars = GAME_NUM_STARS;
    struct Galaxy _galaxy;
    struct Starbase starbases;
    struct Klingon klingons;

    // First initialize Galaxy with blank spaces to overwrite anything that was already there.
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < 8; ++k) {
                for (int l = 0; l < 8; ++l) {
                    _galaxy.coordinates[i][j][k][l] = ' ';
                }
            }
        }
    }

    // Place the Enterprise in the galaxy:
    _galaxy.coordinates[5][6][4][1] = 'E';

    // Place starbases randomly in the galaxy:
    for (int i = 0; i<numStarbases; ++i) {
        struct Starbase newSB;                                  // create blank starbase struct
        bool notPlaced = true;                                  // Assign random position:
        while (notPlaced) {                                     // repeat until valid location found
            int w = rand()%8;                                   // Generate and save random number coordinates:
            int x = rand()%8;
            int y = rand()%8;
            int z = rand()%8;
            if (_galaxy.coordinates[w][x][y][z] == ' ') {       // Check that the spot is empty
                newSB.position[0] = w;                          // Save location in starbase.position array
                newSB.position[1] = x;
                newSB.position[2] = y;
                newSB.position[3] = z;
                _galaxy.coordinates[w][x][y][z] = 'S';          // 'S' stands for starbase
                notPlaced = false;
            }
        }
        _galaxy.starbases[i] = newSB;                           // save completed starbase in galaxy.starbases array
    }

    // Place Klingons randomly in the galaxy:
    for (int i = 0; i<numKlingons; ++i) {
        struct Klingon newKlingon;
        bool notPlaced = true;
        while (notPlaced) {                                     // Assign random position:
            int w = rand()%8;
            int x = rand()%8;
            int y = rand()%8;
            int z = rand()%8;
            if (_galaxy.coordinates[w][x][y][z] == ' ') {       // Check that spot is empty
                newKlingon.position[0] = w;                     // Save location in klingons.position array
                newKlingon.position[1] = x;
                newKlingon.position[2] = y;
                newKlingon.position[3] = z;
                _galaxy.coordinates[w][x][y][z] = 'K';          // 'K' stands for Klingon
                notPlaced = false;
            }
        }
    }

    // Place stars randomly in the galaxy:
    for (int i = 0; i < numStars; ++i) {                        // Generate and save random number coordinates
        bool notPlaced = true;
        while (notPlaced) {
            int w = rand()%8;
            int x = rand()%8;
            int y = rand()%8;
            int z = rand()%8;
            if (_galaxy.coordinates[w][x][y][z] == ' ') {
                _galaxy.coordinates[w][x][y][z] = '*';              // '*' stands for star
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
    _galaxy.gVitals.reputation = 2;
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
    for (int i = 0; i < 8; i++) {
        for (int k = 0; k < 8; k++) {
            _enterprise.explored[i][k] = 0;
        }
    }
    _enterprise.explored[4][5] = 1; // Mark current quadrant as Explored

    // Set up initial conditions for enterprise
    for (int i = 0; i < 8; ++i) {
    _enterprise.sysDamage[i] = 0;
    }
    _enterprise.energy = 3000;
    _enterprise.shields = 0;
    _enterprise.torpedoes = 10;
    // TODO: Condition is RED if enterprise is in a sector with a klingon and no shields, GREEN if enterprise is in a sector with no klingons. Not sure YELLOW - but I would assume it's if enterprise is in a sector with klingon with shields? Is this the right place for Condition variable?
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
  printf("                        (Press Enter to begin)\n\n");
  getchar();                                                            // this empty getchar() simply holds the screen until user presses Enter

  struct Galaxy newGalaxy;
  newGalaxy = createGalaxy();
  newGalaxy.enterprise = createEnterprise(&newGalaxy);
    printf("Your orders are as follows :\n");
    printf("\tDestroy the %d Klingon warships which have invaded\n", newGalaxy.gVitals.numKlingons);
    printf("\tthe galaxy before they can attack Federation Headquarters\n");
    printf("\ton Stardate %4.0f. This gives you %d days. There are\n", (newGalaxy.gVitals.stardate)+GAME_STARDATE_LIMIT, GAME_STARDATE_LIMIT);
    printf("\t%d starbases in the galaxy for resupplying your ship.\n", newGalaxy.gVitals.numStarbases);
    // TODO: Write the text that says which quadrant it is
  exeSRS(&newGalaxy);

  return newGalaxy;
}

struct gameVitals getGameVitals(struct Galaxy *refGalaxy) {                 //TODO: replace direct assignment of gVitals (see below)
    struct gameVitals GVout;
    //POSSIBLY NOT NEEDED!GVout.eDamage = (*refGalaxy).gVitals.eDamage;
    GVout.eEnergy = (*refGalaxy).gVitals.eEnergy;
    GVout.numKlingons = (*refGalaxy).gVitals.numKlingons;               // TODO: count number of starbases remaining (getGameVitals)
    GVout.numStarbases = (*refGalaxy).gVitals.numStarbases;             // TODO: count number of klingons remaining (getGameVitals)
    GVout.reputation = (*refGalaxy).gVitals.reputation;                 // TODO: check if stardate limit has expired (getGameVitals)
    GVout.stardate = (*refGalaxy).gVitals.stardate;                     // TODO: check reputation??? (getGameVitals)
    GVout.userQuit = (*refGalaxy).gVitals.userQuit;

    return GVout;
}

bool gameEnd(struct Galaxy *refGalaxy) {
    if ((*refGalaxy).gVitals.userQuit) {                    // If user chooses Quit
        char getInput[100] = {0};
        printf("THE FEDERATION IS IN NEED OF A NEW STARSHIP COMMANDER\n");
        printf("FOR A SIMILAR MISSION -- IF THERE IS A VOLUNTEER\n");
        printf("LET HIM STEP FORWARD AND ENTER 'AYE' ");

        fgets(getInput, 100, stdin);
        getInput[strlen(getInput) - 1] = '\0';

// Check if user starts a new game
        if((strcmp(getInput, "AYE") == 0) || (strcmp(getInput, "Aye") == 0)||
           (strcmp(getInput, "aye") == 0)) {
            return true;
        } else {
            printf("\nGame ended\n");
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
                printf("NO STARBASES REMAINING!\n");                    // TODO: Fix placeholder game end message for no starbases remaining (gameEnd)
                //exit(0);                      // TODO: I changed this exit to return false
                return false;
            }
        else
            {
                return true;
            }
}

void getCommand(struct Galaxy *refGalaxy) {
    char cmdString[MAX_INPUT_LENGTH];
    printf("COMMAND: ");                                                                          // Prompt user for command input
    fgets(cmdString, MAX_INPUT_LENGTH, stdin);
    strtrim(cmdString, strlen(strtrim));                                                                // Trim any leading spaces off the input
    if ((strchr(cmdString, '?') != NULL) || ((strstr(cmdString, "HELP") != NULL))) {                    // Check for a few basic commands that might be indicating a request for help
            exeHLP(refGalaxy);                                                                          // if detected, execute help command
    } else {
        cmdString[3] = '\0';                                                                            // Truncate the command string to 3 characters
        strToUpper(cmdString, strlen(cmdString));                                                       // convert to uppercase
        if ((strcmp(cmdString, "HEL") == 0) || (strcmp(cmdString, "HLP") == 0)) { exeHLP(refGalaxy);    // Check for some more possible intended help commands; execute help if detected
        } else if (strcmp(cmdString, "NAV") == 0) { exeNAV(refGalaxy);                                  // Otherwise, check for all other possible commands and execute if found
        } else if (strcmp(cmdString, "SRS") == 0) { exeSRS(refGalaxy);
        } else if (strcmp(cmdString, "LRS") == 0) { exeLRS(refGalaxy);
        } else if (strcmp(cmdString, "DAM") == 0) { exeDAM(refGalaxy);
        } else if (strcmp(cmdString, "COM") == 0) { exeCOM(refGalaxy);
        } else if (strcmp(cmdString, "PHA") == 0) { exePHA(refGalaxy);
        } else if (strcmp(cmdString, "TOR") == 0) { exeTOR(refGalaxy);
        } else if (strcmp(cmdString, "SHE") == 0) { exeSHE(refGalaxy);
        } else if (strcmp(cmdString, "DBG") == 0) { exeDBG(refGalaxy);                                  // DEBUG: 'DBG' == a "secret" debug option showing game data
        } else if (strcmp(cmdString, "XXX") == 0) { (*refGalaxy).gVitals.userQuit = true;               // If user chooses resign, trigger the 'userQuit' gameVital
        } else { exeHLP(refGalaxy); }                                                                    // If no other command recognized, print help menu
    }
}

void exeHLP(struct Galaxy* refGalaxy) {
    printf("Enter one of the following:\n");
    printf("  NAV  (To set course)\n");
    printf("  SRS  (For short range sensor scan)\n");
    printf("  LRS  (For long range sensor scan)\n");
    printf("  PHA  (To fire phasers)\n");
    printf("  TOR  (To fire photon torpedoes)\n");
    printf("  SHE  (To raise or lower shields)\n");
    printf("  DAM  (For damage control reports)\n");
    printf("  COM  (To call on library-computer)\n");
    printf("  XXX  (To resign your command)\n\n");
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

    char* cmdString[MAX_INPUT_LENGTH];
    double setCourse = 0.0;
    double setWarp = 0.0;
    printf("COURSE (1-9) ");                                                                                // Prompt user for course info
    fgets(cmdString, MAX_INPUT_LENGTH, stdin);
    remNL(cmdString, strlen(cmdString));
    strtrim(cmdString, strlen(strtrim));                                                                    // Trim any leading spaces off the input
    setCourse = atof(cmdString);
    if ((setCourse < 0.0) || (setCourse > 9.0)) {
        printf("Lt. Sulu reports, 'Incorrect course data, sir!'\n");
    } else {
        double maxWarp = 9.0;
        if ((*refGalaxy).enterprise.sysDamage[0] < 0) { maxWarp = -9.0 * (*refGalaxy).enterprise.sysDamage[0]; }
        printf("WARP FACTOR (0-%.1f) ", maxWarp);
        fgets(cmdString, MAX_INPUT_LENGTH, stdin);
        remNL(cmdString, strlen(cmdString));
        strtrim(cmdString, strlen(strtrim));                                                                // Trim any leading spaces off the input
        setWarp = atof(cmdString);
        //scanf("%.1f", setWarp);
        if ((setWarp < 0) || (setWarp > 9.0)) {     printf("Chief engineer Scott reports, 'The engines won't take warp %.1f!'\n", setWarp);
        } else if (setWarp > maxWarp) {            printf("Warp engines are damaged. Maximum speed = Warp %.1f!'\n", maxWarp);
        } else {
            int curPos[4] = {(*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]};
            int newPos[4] = {(*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]};
            setDest(curPos, setCourse, setWarp, newPos);                                                    //TODO: check for obstacles when travelling (NAV)
            (*refGalaxy).enterprise.position[0] = newPos[0];
            (*refGalaxy).enterprise.position[1] = newPos[1];
            (*refGalaxy).enterprise.position[2] = newPos[2];
            (*refGalaxy).enterprise.position[3] = newPos[3];
            KlingonsFire(refGalaxy);
            (*refGalaxy).gVitals.stardate = round((*refGalaxy).gVitals.stardate + 1);
        }
    }

    return;
}

void exeSRS(struct Galaxy* refGalaxy) {
      // Check if SRS sensors are working
      if ((*refGalaxy).enterprise.sysDamage[1] < 0) {
          printf("Short range sensors are out\n");
          return;
      } else if (KlingonsInQuadrant(refGalaxy)) { printf("\nCOMBAT AREA\tCONDITION RED\n"); }
      if ((*refGalaxy).enterprise.shields <= 10) { printf("   SHIELDS DANGEROUSLY LOW\n"); }
      printf("---------------------------------------------\n");
      // Get current enterprise quadrant location from enterprise coordinates
      int w = (*refGalaxy).enterprise.position[0];
      int x = (*refGalaxy).enterprise.position[1];
      int numKlingons = 0;
      // Iterate through current quadrant and print whatever is in each sector
      for (int y = 0; y<8; y++) {
          for (int z = 0; z<8; z++) {
              if (((*refGalaxy).enterprise.position[2] == y) && ((*refGalaxy).enterprise.position[3] == z)) {
                    printf(" <E> ");
              }
              if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
                  printf(" +K+ ");
              }
              else if ((*refGalaxy).coordinates[w][x][y][z] == 'S') {
                  printf(" >B< ");
              }
              else {
              printf("  %c  ", (*refGalaxy).coordinates[w][x][y][z]);
              }
              // Increment klingon counter to count how many klingons are in current sector
              if ((*refGalaxy).coordinates[w][x][y][z] == 'K') { numKlingons++; }
          }
          switch(y) {
                char cond[10];
                case 0:
                    printf("\tStardate:\t\t%d\n", ((*refGalaxy).gVitals.stardate));
                    break;
                case 1:
                    if ((*refGalaxy).enterprise.condition == GREEN) { strcpy(cond,"*GREEN*");
                    } else if ((*refGalaxy).enterprise.condition == YELLOW) { strcpy(cond,"*YELLOW*");
                    } else { strcpy(cond,"*RED*"); }
                    printf("\tCondition:\t\t%s\n", cond);
                    break;
                case 2:
                    printf("\tQuadrant:\t\t%d,%d\n", (*refGalaxy).enterprise.position[0]+1, (*refGalaxy).enterprise.position[1]+1);
                    break;
                case 3:
                    printf("\tSector:\t\t\t%d,%d\n", (*refGalaxy).enterprise.position[2]+1, (*refGalaxy).enterprise.position[3]+1);
                    break;
                case 4:
                    printf("\tPhoton Torpedoes:\t%d\n", (*refGalaxy).enterprise.torpedoes);
                    break;
                case 5:
                    printf("\tTotal Energy:\t\t%d\n", (*refGalaxy).enterprise.energy);
                    break;
                case 6:
                    printf("\tShields:\t\t%d\n", (*refGalaxy).enterprise.shields);
                    break;
                case 7:
                    printf("\tKlingons Remaining:\t%d\n\n", (*refGalaxy).gVitals.numKlingons);
                    break;
                default:
                    break;
          }
      }
      printf("---------------------------------------------\n");
      // Set condition to RED, YELLOW, or GREEN, depending on presence of Klingons and shields
      if (numKlingons > 0) {
          if ((*refGalaxy).enterprise.shields == 0) { (*refGalaxy).enterprise.condition = RED; }
          else { (*refGalaxy).enterprise.condition = YELLOW; }
      }
      else { (*refGalaxy).enterprise.condition = GREEN; }
      return;
  }

void exeLRS(struct Galaxy* refGalaxy) {
    printf("'LRS' command executed.\n\n");
    if((*refGalaxy).enterprise.sysDamage[2] < 0) {
        printf("Long Range Sensors are Inoperable.\n");
        return;
    }
    else {
        printf("Long Range Scan for Quadrant %d, %d\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1]);
        printf("---------------------------\n");
        for (int w = (*refGalaxy).enterprise.position[0]-1; w <= (*refGalaxy).enterprise.position[0]+1; w++) {
            for (int x = (*refGalaxy).enterprise.position[1]-1; x <= (*refGalaxy).enterprise.position[1]+1; x++) {
                // Mark scanned quadrants as Explored
                (*refGalaxy).enterprise.explored[w][x] = 1;
                int klingonCounter = 0;
                int starbaseCounter = 0;
                int starCounter = 0;
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
                printf("  :  %d%d%d", klingonCounter, starbaseCounter, starCounter);
            }
            printf("  :\n---------------------------\n");
        }
    }
    return;
}

void exeDAM(struct Galaxy* refGalaxy) {
    char input;
    int damageCounter = 0;
    if((*refGalaxy).enterprise.sysDamage[5] >= 0) {
        printf("\nDEVICE\t\t\t  STATE OF REPAIR\n");
        printf("Warp Engines\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[0]);
        printf("Short Range Sensors\t\t%d\n", (*refGalaxy).enterprise.sysDamage[1]);
        printf("Long Range Sensors\t\t%d\n", (*refGalaxy).enterprise.sysDamage[2]);
        printf("Phaser Control\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[3]);
        printf("Photon Tubes\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[4]);
        printf("Damage Control\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[5]);
        printf("Shield Control\t\t\t%d\n", (*refGalaxy).enterprise.sysDamage[6]);
        printf("Library-Computer\t\t%d\n", (*refGalaxy).enterprise.sysDamage[7]);
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
            printf("\nStatus Report:\n");
            printf("Klingons Left: %d\n", (*refGalaxy).gVitals.numKlingons);
            printf("Mission must be completed in %d stardates\n", (*refGalaxy).gVitals.stardate);
            printf("The Federation is maintaining %d starbases in the galaxy\n", (*refGalaxy).gVitals.numStarbases);
            printf("\nDevice           State of Repair\n");
            printf("Warp Engines         %d\n", (*refGalaxy).enterprise.sysDamage[0]);
            printf("Short Range Sensors  %d\n", (*refGalaxy).enterprise.sysDamage[1]);
            printf("Long Range Sensors   %d\n", (*refGalaxy).enterprise.sysDamage[2]);
            printf("Phaser Control       %d\n", (*refGalaxy).enterprise.sysDamage[3]);
            printf("Photon Tubes         %d\n", (*refGalaxy).enterprise.sysDamage[4]);
            printf("Damage Control       %d\n", (*refGalaxy).enterprise.sysDamage[5]);
            printf("Shield Control       %d\n", (*refGalaxy).enterprise.sysDamage[6]);
            printf("Library Computer     %d\n", (*refGalaxy).enterprise.sysDamage[7]);
            printf("\n\n");
            break;
        case ('2'):
            printf("From Enterprise to Klingon Battle Cruiser\n");
            printf("Direction = \n");                                                                                       //TODO: check for INT direction values in COM, change to double
            printf("Distance = \n");
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
            printf("Mr. Spock reports, ""sensors show %d starbases in this quadrant.""\n", starbaseCounter);
            break;
        case ('4'):
            printf("Direction/Distance Calculator\n");
            printf("You are at quadrant %d, %d, sector %d, %d\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]);
            // Ask user for destination coordinates
            printf("Please enter \n    initial coordinates(x,y): \n");
            float Q0;
            float Q1;
            float S0;
            float S1;
            scanf("%f,%f", &Q0, &Q1);
            printf("    final coordinates(x,y): \n");
            scanf("%f,%f", &S0, &S1);
            getchar();   // Extra getchar to get rid of newline in input buffer
            int Direction = 0;
            // Calculate direction
            // TODO: These directions aren't correct, I need to get a big piece of paper and make a map to figure out how to calculate them. 1 is East, 3 is North, etc.
            if ((*refGalaxy).enterprise.position[0] < Q0) {
                if ((*refGalaxy).enterprise.position[1] < Q1) {
                    Direction = 1;
                }
                else if ((*refGalaxy).enterprise.position[1] == Q1) {
                    Direction = 2;
                }
                else {
                    Direction = 3;
                }
            }
            else if ((*refGalaxy).enterprise.position[0] == Q0) {
                if ((*refGalaxy).enterprise.position[1] < Q1) {
                    Direction = 4;
                }
                else if ((*refGalaxy).enterprise.position[1] == Q1) {
                    Direction = 5;
                }
                else {
                    Direction = 6;
                }
            }
            else {
                if ((*refGalaxy).enterprise.position[1] < Q1) {
                    Direction = 7;
                }
                else if ((*refGalaxy).enterprise.position[1] < Q1) {
                    Direction = 8;
                }
                else {
                        Direction = 9;
                    }
                }
            // Calculate distance between current location and destination
            // TODO: I used the Distance Formula to calculate the distance between 2 quadrants. But we actually need to calculate distance between specific sector in 1 quadrant and specific sector in another quadrant. This is more math than I can face right now - I'll come back to this.
            float EQ0 = (*refGalaxy).enterprise.position[0];
            float EQ1 = (*refGalaxy).enterprise.position[1];
            float ES0 = (*refGalaxy).enterprise.position[2];
            float ES1 = (*refGalaxy).enterprise.position[3];
            float Distance = sqrt(pow((EQ0 - Q0),2) + pow((EQ1 - Q1), 2));
            printf("Direction = %d\n", Direction);
            printf("Distance = %f\n", Distance);
            break;
        case ('5'):
            printf("                        The Galaxy\n");
            printf("    1      2      3      4      5      6      7      8\n");
            printf("1           Antares          |           Sirius\n");
            printf("2           Rigel            |           Deneb\n");
            printf("3           Procyon          |           Capella\n");
            printf("4           Vega             |           Betelgeuse\n");
            printf("5           Canopus          |           Aldebaran\n");
            printf("6           Altair           |           Regulus\n");
            printf("7           Sagittarius      |           Arcturus\n");
            printf("8           Pollux           |           Spica\n");
            break;
        default:
            printf("Unrecognized command. Enter 0 - 5.\n");
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
    int w = (*refGalaxy).enterprise.position[0];
    int x = (*refGalaxy).enterprise.position[1];
    for (int y = 0; y < 8; y++) {
        for (int z = 0; z < 8; z++) {
            if ((*refGalaxy).coordinates[w][x][y][z] == 'K') {
              klingonCounter = klingonCounter + 1;
            }
        }
    }
    if (klingonCounter <= 0) {      // If there are no klingons in quadrant, print message and return
        printf("Science Officer Spock reports, ""Sensors show no enemy ships in this quadrant""");
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
        printf("All photon torpedoes expended.\n");
        return;
    }
    // If photon tubes are damaged
    if ((*refGalaxy).enterprise.sysDamage[4] < 0) {
        printf("Photon tubes are not operational.\n");
        return;
    }
    printf("Input photon torpedo course (1-8): ");
    int course = 0;
    scanf("%d", &course);
    getchar();   // Get extra newline character from input buffer
    if ((course < 1) || (course > 8)) {
        printf("Ensign Chekov reports, ""Incorrect course data, sir!""\n");
        return;
    }
    // Decrement numberTorpedos:
    (*refGalaxy).enterprise.torpedoes--;
    // Assign coordinates to current position of enterprise
    int w = (*refGalaxy).enterprise.position[0];
    int x = (*refGalaxy).enterprise.position[1];
    int y = (*refGalaxy).enterprise.position[2];
    int z = (*refGalaxy).enterprise.position[3];
    switch (course) {
        case (1):           // Direction 1: EAST
            z++;
            if (z < 8) {
            printf("Torpedo Track:\n");
            }
            while (z < 8) {
                printf("            %d, %d\n", y+1, z+1);
                if (z == 7) {
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
    return;
}

void exeSHE(struct Galaxy* refGalaxy) {
  //User input for amount of energy to allocate to shields
    int x;
    printf("'SHE' command executed.\n\n");
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
    printf("\n\tDEBUG MODE ACCESSED.\n\n");
    printf("GAME VITALS STATUS:\n");
    printf("Starbases remaining:\t%d\n", (*refGalaxy).gVitals.numStarbases);
    printf("Klingons remaining:\t%d\n", (*refGalaxy).gVitals.numKlingons);
    printf("Stardate\t\t%d\n", (*refGalaxy).gVitals.stardate);
    printf("Reputation:\t\t%3.1f\n", (*refGalaxy).gVitals.reputation);
    printf("User has quit:\t\t");
    if ((*refGalaxy).gVitals.userQuit) { printf("True\n\n");
    } else { printf("False\n\n"); }
    printf("\nENTERPRISE STATUS:\n");
    printf("Position-\n  Quadrant:\t(%d,%d)\n  Sector:\t(%d,%d)\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1], (*refGalaxy).enterprise.position[2], (*refGalaxy).enterprise.position[3]);
    printf("Energy:\t\t%4.1f\n", (*refGalaxy).enterprise.energy);
    printf("Shields:\t%4.1f\n", (*refGalaxy).enterprise.shields);
    printf("Torpedoes:\t%4d\n\n", (*refGalaxy).enterprise.torpedoes);
    return;
}

void setDest(int* _start, double dir, double dist, int* _destination) {             // Using starting coordinates, direction (NAV number), and distance, it sets destination coordinates accordingly
    int sStart[2] = {(_start[0] * 8 + _start[2]), (_start[1] * 8 + _start[3])};     // For simplicty of calculation, convert (Qx,Qy,Sx,Sy) format to an equivalent (Sx,Sy)
    int sEnd[2] = {sStart[0], sStart[1]};
    int sDiff[2] = {0, 0};
    int lclDiff[4] = {0, 0, 0, 0};

    double theta = ((dir - 1.0) * 2 * PI) / 8.0;                                    //DEBUG:printf("\t%.1f units from Q(%d,%d)S(%d,%d) in dir %.1f = theta: %.2f\n", dist, _start[0]+1, _start[1]+1, _start[2]+1, _start[3]+1, dir, theta);  //DEBUG values test
    sDiff[0] = round(dist * 8 * cos(theta));
    sDiff[1] = round(dist * 8 * sin(theta));

    if (((sStart[0] + sDiff[0]) > 64) || ((sStart[0] + sDiff[0]) < 1) || ((sStart[1] + sDiff[1]) > 64) || ((sStart[1] + sDiff[1]) < 1)) {
        printf("Can't leave the galaxy!\n");                                        //TODO: find actual "leaving galaxy" message
        return;
    }

    sEnd[0] = sStart[0] + sDiff[0];
    sEnd[1] = sStart[1] + sDiff[1];
    lclDiff[0] = floor(sEnd[0] / 8) - floor(sStart[0] / 8);
    lclDiff[1] = floor(sEnd[1] / 8) - floor(sStart[1] / 8);
    lclDiff[2] = (sEnd[0] % 8) - (sStart[0] % 8);
    lclDiff[3] = (sEnd[1] % 8) - (sStart[1] % 8);
                                                                                    //DEBUG:printf("\tOLD Dest. value: Q(%d,%d)S(%d,%d)\n", _destination[0]+1, _destination[1]+1, _destination[2]+1, _destination[3]+1);  //DEBUG values test
                                                                                    //DEBUG:printf("\tShift value:     Q(%d,%d)S(%d,%d)\n", lclDiff[0], lclDiff[1], lclDiff[2], lclDiff[3]);  //DEBUG values test
    _destination[0] = _start[0] + lclDiff[0];
    _destination[1] = _start[1] + lclDiff[1];
    _destination[2] = _start[2] + lclDiff[2];
    _destination[3] = _start[3] + lclDiff[3];
                                                                                    //DEBUG:printf("\tNEW Dest. value: Q(%d,%d)S(%d,%d)\n", _destination[0]+1, _destination[1]+1, _destination[2]+1, _destination[3]+1);  //DEBUG values test
    return;
}

double checkObstacles(int* _start, double dir, double dist, struct Galaxy* refGalaxy) { // iterates through parameters' travel path to check for obstacles, returns furthest good coordinate if found
    for (double i=0.1; i<=dist; i+=0.1) {
        int chkCoords[4] = {0, 0, 0, 0};
        setDest(_start, dir, i, chkCoords);
        if ((*refGalaxy).coordinates[chkCoords[0]][chkCoords[1]][chkCoords[2]][chkCoords[3]] != ' ') { return (i - 0.1); }
    }
    return dist;
}

void KlingonsFire(struct Galaxy* refGalaxy) {                                       //TODO: Implement KlingonsFire() (called by NAV, TOR, and PHA functions)
}

bool KlingonsInQuadrant(struct Galaxy* refGalaxy) {                                 //TODO: Implement KlingonsInQuadrant()
    return true;
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
