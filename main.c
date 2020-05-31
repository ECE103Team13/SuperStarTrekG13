#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_LENGTH 127

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
  bool userQuit;
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
void getCommand(struct Galaxy* refGalaxy);

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
    //struct gameVitals currentGame;

    _galaxy.gVitals.eDamage = 0;
    _galaxy.gVitals.eEnergy = 10;
    _galaxy.gVitals.numStarbases = 10;
    _galaxy.gVitals.numKlingons = 26;
    _galaxy.gVitals.stardate = 20;    // I made this number up
    _galaxy.gVitals.reputation = 2;
    _galaxy.gVitals.userQuit = false;

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

struct Galaxy gameIntro() {                                             // display splash ASCII, call new game generation functions
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
  newGalaxy.enterprise = createEnterprise();
  exeSRS(newGalaxy);

  return newGalaxy;
}

struct gameVitals getGameVitals(struct Galaxy *refGalaxy) {             //TODO: replace direct assignment of gVitals (see below)
    struct gameVitals GVout;

    GVout.eDamage = (*refGalaxy).gVitals.eDamage;
    GVout.eEnergy = (*refGalaxy).gVitals.eEnergy;
    GVout.numKlingons = (*refGalaxy).gVitals.numKlingons;               // TODO: count number of starbases remaining (getGameVitals)
    GVout.numStarbases = (*refGalaxy).gVitals.numStarbases;             // TODO: count number of klingons remaining (getGameVitals)
    GVout.reputation = (*refGalaxy).gVitals.reputation;                 // TODO: check if stardate limit has expired (getGameVitals)
    GVout.stardate = (*refGalaxy).gVitals.stardate;                     // TODO: check reputation??? (getGameVitals)
    GVout.userQuit = (*refGalaxy).gVitals.userQuit;

    return GVout;
}

bool gameEnd(struct Galaxy *refGalaxy) {
    if ((*refGalaxy).gVitals.userQuit) {
        char getInput[100] = {0};
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

    for(int i = 0; i < 5; ++i){
        if((*refGalaxy).gVitals.stardate == 0){
            char getInput[100] = {0};

            printf("IT IS STARDATE %d\n", (*refGalaxy).gVitals.stardate);
            printf("THERE WERE %d KLINGON BATTLE CRUISERS LEFT AT\n", (*refGalaxy).gVitals.numKlingons);
            printf("THE END OF YOUR MISSION.\n\n");

            if((*refGalaxy).gVitals.numStarbases == 0) {
                printf("NO STARBASES REMAINING!\n");                    // TODO: Fix placeholder game end message for no starbases remaining (gameEnd)
                exit(0);
            } else if ((*refGalaxy).gVitals.userQuit) {
                printf("THE FEDERATION IS IN NEED OF A NEW STARSHIP COMMANDER\n");
                printf("FOR A SIMILAR MISSION -- IF THERE IS A VOLUNTEER\n");
                printf("LET HIM STEP FORWARD AND ENTER 'AYE' ");

                fgets(getInput, 100, stdin);
                getInput[strlen(getInput) - 1] = '\0';

                if((strcmp(getInput, "AYE") == 0) || (strcmp(getInput, "Aye") == 0)|| (strcmp(getInput, "aye") == 0)) {
                    struct Galaxy newGalaxy;
                    newGalaxy = gameIntro();
                    refGalaxy = &newGalaxy;
                    (*refGalaxy).gVitals.userQuit = false;              // TODO: ensure proper setting of Galaxy.gVitals.userQuit after quitting and starting new game (gameEnd)
                    return false;
                } else {
                    printf("\nGame ended\n");
                    return true;
                }
            }
    }
    else{
      if(i == 4) {
        return true;
      }
    }
  }
}

void getCommand(struct Galaxy *refGalaxy) {
    char cmdString[MAX_INPUT_LENGTH];
    printf("Enter Command: ");                                                                          // Prompt user for command input
    fgets(cmdString, MAX_INPUT_LENGTH, stdin);
    strtrim(cmdString, strlen(strtrim));                                                                // Trim any leading spaces off the input
    if ((strchr(cmdString, '?') != NULL) || ((strstr(cmdString, "HELP") != NULL))) {                    // Check for a few basic commands that might be indicating a request for help
            exeHLP(refGalaxy);                                                                          // if detected, execute help command
    } else {
        cmdString[3] = '\0';                                                                            // Truncate the command string to 3 characters
        strToUpper(cmdString, strlen(cmdString));                                                       // convert to uppercase
        if ((strcmp(cmdString, "HEL") == 0) || (strcmp(cmdString, "HLP") == 0)) { exeHLP(refGalaxy);    // Check for some more possible intended help commands; execute help if detected
        } else if (strcmp(cmdString, "NAV") == 0) { exeNAV(refGalaxy);                                  // Otherwise, check for all other possible commands; execute detected command
        } else if (strcmp(cmdString, "SRS") == 0) { exeSRS(refGalaxy);
        } else if (strcmp(cmdString, "LRS") == 0) { exeLRS(refGalaxy);
        } else if (strcmp(cmdString, "DAM") == 0) { exeDAM(refGalaxy);
        } else if (strcmp(cmdString, "COM") == 0) { exeCOM(refGalaxy);
        } else if (strcmp(cmdString, "PHA") == 0) { exePHA(refGalaxy);
        } else if (strcmp(cmdString, "TOR") == 0) { exeTOR(refGalaxy);
        } else if (strcmp(cmdString, "SHE") == 0) { exeSHE(refGalaxy);
        } else if (strcmp(cmdString, "DBG") == 0) { exeDBG(refGalaxy);                                  // DEBUG: 'DBG' == a "secret" debug option showing game data
        } else if (strcmp(cmdString, "XXX") == 0) {
            (*refGalaxy).gVitals.userQuit = true;                                                       // If user chooses resign, trigger the 'userQuit' gameVital
            printf("\n\n('XXX' command executed)\n\n");
        } else {                                                                                        // If no other command recognized, print error message and re-enter main loop
            printf("Error. Command not recognized.\n\n");
        }
    }
}

void exeHLP(struct Galaxy *refGalaxy) {
    printf("HELP MENU:\nEnter one of the following commands (without quotes):\n");
    printf("\t'NAV'\t(Navigation)\n");
    printf("\t'SRS'\t(Short Range Sensors)\n");
    printf("\t'LRS'\t(Long Range Sensors)\n");
    printf("\t'DAM'\t(Damage Control)\n");
    printf("\t'COM'\t(Database Computer)\n");
    printf("\t'PHA'\t(Phasers)\n");
    printf("\t'TOR'\t(Torpedoes)\n");
    printf("\t'SHE'\t(Shields)\n");
    printf("\t'XXX'\t(Resign Command)\n\n");
    printf("'HELP' command executed.\n\n");
    return;
}

void exeNAV(struct Galaxy *refGalaxy) {                                 // TODO: implement NAV subroutine
    printf("'NAV' command executed.\n\n");
    return;
}

void exeSRS(struct Galaxy *refGalaxy) {                                 // TODO: implement SRS subroutine
    printf("'SRS' command executed.\n\n");
    return;
}

void exeLRS(struct Galaxy *refGalaxy) {                                 // TODO: implement LRS subroutine
    printf("'LRS' command executed.\n\n");
    return;
}

void exeDAM(struct Galaxy *refGalaxy) {                                 // TODO: implement DAM subroutine
    printf("'DAM' command executed.\n\n");
    return;
}

void exeCOM(struct Galaxy *refGalaxy) {                                 // TODO: implement COM subroutine
    printf("'COM' command executed.\n\n");
    return;
}

void exePHA(struct Galaxy *refGalaxy) {                                 // TODO: implement PHA subroutine
    printf("'PHA' command executed.\n\n");
    return;
}

void exeTOR(struct Galaxy *refGalaxy) {                                 // TODO: implement TOR subroutine
    printf("'TOR' command executed.\n\n");
    return;
}

void exeSHE(struct Galaxy *refGalaxy) {                                 // TODO: implement SHE subroutine
    printf("'SHE' command executed.\n\n");
    return;
}

void exeDBG(struct Galaxy *refGalaxy) {                                 // DEBUG: "secret" debug subroutine
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
    printf("Damage:\t\t%4.1f\n", (*refGalaxy).enterprise.damage);
    printf("Energy:\t\t%4.1f\n", (*refGalaxy).enterprise.energy);
    printf("Shields:\t%4.1f\n", (*refGalaxy).enterprise.shields);
    printf("Torpedoes:\t%4d\n\n", (*refGalaxy).enterprise.torpedoes);
    return;
}

void strtrim(char* string, int n) {                                     // Checks through string for leading whitespace, then copies characters to the string start, then ends string after n chars
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

void remNL(char* string, int n) {                                       // Checks the end of the string parameter for the newline character and, if found, removes it
    if (string[n-1] == '\n') { string[n-1] = '\0'; }
    return;
}

void strToLower(char* string, int n) {                                  // Loops through string parameter, changing each index to its lowercase version
    for (int i=0; i<n; i++) { string[i] = tolower(string[i]); }
    return;
}

void strToUpper(char* string, int n) {                                  // Loops through string parameter, changing each index to its uppercase version
    for (int i=0; i<n; i++) { string[i] = toupper(string[i]); }
    return;
}
