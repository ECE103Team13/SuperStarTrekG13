#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_INPUT_LENGTH 127

// enum declarations:
typedef enum {GREEN, YELLOW, RED} Condition;

//struct declarations:
struct gameVitals {
  double eDamage[8];
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
  double damage[8];
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
    int numStarbases = 3;
    int numKlingons = 26;
    int numStars = 100;
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

    // Place the Enterprise in the galaxy:
    _galaxy.coordinates[5][6][4][1] = 'E';

    // Set up starbases randomly in the galaxy:
    for (int i = 0; i <numStarbases; ++i)
    {
        // Generate and save random number coordinates
        srand((int)time(0));            // Seed rand function with current time
        int w = rand()%8;
        int x = rand()%8;
        int y = rand()%8;
        int z = rand()%8;
        // Check that the spot is empty
        if (_galaxy.coordinates[w][x][y][z] != 'E') {
            // 'S' stands for starbase
            _galaxy.coordinates[w][x][y][z] = 'S';
            // Save location in starbases.position array
            starbases.position[0] = w;
            starbases.position[1] = x;
            starbases.position[2] = y;
            starbases.position[3] = z;
            }
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
        if ((_galaxy.coordinates[w][x][y][z] != 'S') && (_galaxy.coordinates[w][x][y][z] != 'E'))
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
        if ((_galaxy.coordinates[w][x][y][z] != 'S') && (_galaxy.coordinates[w][x][y][z] != 'K') && (_galaxy.coordinates[w][x][y][z] != 'E'))
        {
            _galaxy.coordinates[w][x][y][z] = '*';
            // I don't think we need to save locations of stars elsewhere?
        }
    }

                                                                    //Debug: Print map of galaxy, sector by sector:
                                                             /*       for (int i = 0; i < 8; ++i)
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
                                                                    }*/
                                                                    //END Debug printf code


    // Set up initial gameVitals for currentGame
    //struct gameVitals currentGame;

// All game vitals will count down to 0
// If any game vital reaches 0, game will end.
    // TODO: I made eDamage an array of 8 items because there are 8 separate things that can be damaged. I hope this doesn't mess up the gVitals function here. We can make 8 individual variables but that seemed silly.
    for (int i = 0; i < 8; ++i) {
    _galaxy.gVitals.eDamage[i] = 1;
    }
    _galaxy.gVitals.eEnergy = 10;
    _galaxy.gVitals.numStarbases = 3;
    _galaxy.gVitals.numKlingons = 26;
    _galaxy.gVitals.stardate = 26;
    _galaxy.gVitals.reputation = 2;
    _galaxy.gVitals.userQuit = false;

    return _galaxy;
};

struct Enterprise createEnterprise(struct Galaxy *refGalaxy) {
  struct Enterprise _enterprise;
    // Place Enterprise in the middle of the galaxy
        _enterprise.position[0] = 4;
        _enterprise.position[1] = 5;
        _enterprise.position[2] = 3;
        _enterprise.position[3] = 0;

  //TODO: code to generate enterprise
  //TODO: delete following temporary manual declaration once createEnterprise() is complete
    for (int i = 0; i < 8; ++i) {
    _enterprise.damage[i] = 1;
    }
  _enterprise.energy = 50.0;
  _enterprise.shields = 15.0;
  _enterprise.torpedoes = 4;
  _enterprise.condition = YELLOW;
  //END code to delete

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
  exeSRS(&newGalaxy);

  return newGalaxy;
}

struct gameVitals getGameVitals(struct Galaxy *refGalaxy) {             //TODO: replace direct assignment of gVitals (see below)
    struct gameVitals GVout;

// TODO: I'm sorry if I messed this up. I made eDamage an array of 8; I'm not sure what to do here to make an array work in this function. I commented it out here so the program can generally keep running until we fix this.
//    GVout.eDamage = (*refGalaxy).gVitals.eDamage;

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
    // TODO: I think this loop (check whether user input is Quit) already happens above? Do we need it twice?
       /*     else if ((*refGalaxy).gVitals.userQuit) {
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
                }
                else {
                    printf("\nGame ended\n");
                    return true;
                }
            }
    }
    else{
      if(i == 4) {
        return true;
      }
    }*/
//}
}

void getCommand(struct Galaxy *refGalaxy) {
    char cmdString[MAX_INPUT_LENGTH];
    printf("Enter Command: ");                                                                          // Prompt user for command input
    fgets(cmdString, MAX_INPUT_LENGTH, stdin);
    strtrim(cmdString, strlen(strtrim));
    // Trim any leading spaces off the input
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

void exeLRS(struct Galaxy *refGalaxy) {
    printf("'LRS' command executed.\n\n");
    if((*refGalaxy).gVitals.eDamage[2] == 0) {
        printf("Long Range Sensors are Inoperable.\n");
        // TODO: Add other code here of what to do in this case (see Source Code line 4000)
    }
    else {
        printf("Long Range Scan for Quadrant %d, %d\n", (*refGalaxy).enterprise.position[0], (*refGalaxy).enterprise.position[1]);
        printf("---------------------------\n");
        for (int w = (*refGalaxy).enterprise.position[0]-1; w <= (*refGalaxy).enterprise.position[0]+1; w++) {
            for (int x = (*refGalaxy).enterprise.position[1]-1; x <= (*refGalaxy).enterprise.position[1]+1; x++) {
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

void exeDAM(struct Galaxy *refGalaxy) {                                 // TODO: implement DAM subroutine
    printf("'DAM' command executed.\n\n");
    return;
}

void exeCOM(struct Galaxy *refGalaxy) {
    // Initialize counter variables to be used in executing computer commands
    int starbaseCounter = 0;

    printf("Computer active and awaiting command: ");
    char command = getchar();
    getchar(); // Extra getchar to get rid of the newline

    // Switch branching statement to execute each possible computer command
    switch(command) {
        case ('0'):
            printf("case 0\n");
            // TODO: This prints a map of the galaxy quadrants, with names, but the trick is that it only shows information (quantity of starbases, klingons, stars) for quadrants that have been long range scanned or visited. So we may need a new variable to keep track of that?
            break;
        case ('1'):
            printf("\nStatus Report:\n");
            printf("Klingons Left: %d\n", (*refGalaxy).gVitals.numKlingons);
            printf("Mission must be completed in %d stardates\n", (*refGalaxy).gVitals.stardate);
            printf("The Federation is maintaining %d starbases in the galaxy\n", (*refGalaxy).gVitals.numStarbases);
            printf("\nDevice           State of Repair\n");
            printf("Warp Engines         %d\n", (*refGalaxy).gVitals.eDamage[0]);
            printf("Short Range Sensors  %d\n", (*refGalaxy).gVitals.eDamage[1]);
            printf("Long Range Sensors   %d\n", (*refGalaxy).gVitals.eDamage[2]);
            printf("Phaser Control       %d\n", (*refGalaxy).gVitals.eDamage[3]);
            printf("Photon Tubes         %d\n", (*refGalaxy).gVitals.eDamage[4]);
            printf("Damage Control       %d\n", (*refGalaxy).gVitals.eDamage[5]);
            printf("Shield Control       %d\n", (*refGalaxy).gVitals.eDamage[6]);
            printf("Library Computer     %d\n", (*refGalaxy).gVitals.eDamage[7]);
            printf("\n\n");
            break;
        case ('2'):
            printf("From Enterprise to Klingon Battle Cruser\n");
            printf("Direction = \n");
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

void exePHA(struct Galaxy *refGalaxy) {                                 // TODO: implement PHA subroutine
    printf("'PHA' command executed.\n\n");
    return;
}

void exeTOR(struct Galaxy *refGalaxy) {
    // If torpedoes are used up
    if ((*refGalaxy).enterprise.torpedoes < 1) {
        printf("All photon torpedoes expended.\n");
        return;
    }
    // If photon tubes are damaged
    if ((*refGalaxy).gVitals.eDamage[4] < 0) {
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

void exeSHE(struct Galaxy *refGalaxy) {
    int x;
    printf("'SHE' command executed.\n\n");
    if((*refGalaxy).gVitals.eDamage[7] < 0) {
      printf("SHIELD CONTROL INOPERABLE\n");
      return;
    }
    printf("ENERGY AVAILABLE = %d\n", ((*refGalaxy).gVitals.eEnergy + (*refGalaxy).enterprise.shields));
    printf("INPUT NUMBER OF UNITS TO SHIELDS? ");
    scanf("%d", &x);
    getchar();      // To get rid of the extra newline in input buffer
    if(x < 0 || (*refGalaxy).enterprise.shields == x) {
      printf("\n<SHIELDS UNCHANGED>\n");
      return;
    }
    if(x <= ((*refGalaxy).gVitals.eEnergy + (*refGalaxy).enterprise.shields)) {
      (*refGalaxy).gVitals.eEnergy = (*refGalaxy).gVitals.eEnergy + (*refGalaxy).enterprise.shields - x;
      (*refGalaxy).enterprise.shields = x;
      printf("DEFLECTOR CONTROL ROOM REPORT : \n");
      printf("SHIELDS NOW AT %d UNITS PER YOUR COMMAND\n", (*refGalaxy).enterprise.shields);
      return;
    }
    printf("SHIELD CONTROL REPORTS 'THIS IS NOT THE FEDEREATION TREASURY'\n");
    printf("<SHIELDS UNCHANGED>\n");
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
