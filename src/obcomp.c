#include <stdio.h>
#include <string.h>

#include "obcomp.h"

#include "gfx.h"

/// Implements on-board computer messages


/////////////////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////////////////
const char *strNewShipMsg = "+[uKNwn]";


#define MESSAGE_LEN     (64)
#define NUM_MESSAGES    (15)
static char messages[NUM_MESSAGES][MESSAGE_LEN];
static int colours[NUM_MESSAGES];

static const char *strObcReset = "____________";

static int last_message = -1;
static int display_countdown = 0;


#define X_START         (15)
#define Y_START         (345)
#define Y_STEP          (15)
#define DURATION        (37)


/// see shipdata.h for #define's
static const char *ship_names[] = {
    "Sun",                      //"SUN",
    "Planet",                   //"PLANET",

    "-null-",                   //"NULL or UNKNWON",

    "LF Missile",               //"MISSILE",
    "Coriolis SS",              //"CORIOLIS",
    "EsCaps",                   //"ESCAPE_CAPSULE",
    "Freight",                  //"ALLOY",
    "Freight",                  //"CARGO",
    "Boulder",                  //"BOULDER",
    "Astroid",                  //"ASTEROID",
    "RocK",                     //"ROCK",
    "SK.AD Shuttle",            //"SHUTTLE",
    "MC15 Trnsprt",             //"TRANSPORTER",
    "Cobra MkIII",              //"COBRA3",
    "Python",                   //"PYTHON",
    "Boa CC",                   //"BOA",
    "Anaconda",                 //"ANACONDA",
    "rocK Hermit",              //"HERMIT",
    "Viper HK",                 //"VIPER",
    "Sidewndr",                 //"SIDEWINDER",
    "Mamba",                    //"MAMBA",
    "Krait",                    //"KRAIT",
    "Adder",                    //"ADDER",
    "Gecko",                    //"GECKO",
    "Cobra",                    //"COBRA1",
    "Worm Lndr",                //"WORM",
    "Cobra Mk3",                //"COBRA3_LONE",
    "Asp Mk.2",                 //"ASP2",
    "Python",                   //"PYTHON_LONE",
    "Fer deLnce",               //"FER_DE_LANCE",
    "Moray*bt",                 //"MORAY",
    "T#aRg0i.",                 //"THARGOID",
    "T#arg1Et",                 //"THARGLET",
    "Le Cnstrictr",             //"CONSTRICTOR",
    "Cougar?",                  //"COUGAR",
    "Dodo Station",             //"DODEC"
};


/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////
void obc_clear()
{
    for (int i = 0; i < NUM_MESSAGES; ++i)
    {
        messages[i][0] = 0;
        colours[i] = GFX_COL_BLACK;
    }
    last_message = -1;
    display_countdown = 0;
}

void obc_message(const char *msg, int col)
{
    int n = strlen(msg);
    if (strncmp(msg, messages[last_message], n) == 0)
    {
        if (strcmp(msg, strNewShipMsg) == 0)
            return;

        messages[last_message][n]     = '+';
        messages[last_message][n + 1] = 0;
    }
    else
    {
        last_message = (last_message + 1) % NUM_MESSAGES;

        strcpy(messages[last_message], msg);
        colours[last_message] = col;
    }
    display_countdown = DURATION;
}
void obc_reset(const char *planet)
{
    obc_message(strObcReset, GFX_COL_AA_0);
    obc_message(planet, GFX_COL_AA_0);
}

void obc_display()
{
    if (display_countdown)
    {
        int x = X_START;
        int y = Y_START;
        for (int i = 0; i < NUM_MESSAGES; ++i)
        {
            int message = last_message - i;
            if (message < 0)
                message += NUM_MESSAGES;
            if (colours[message])
                gfx_display_colour_text(x, y - (i * Y_STEP), messages[message], colours[message]);
        }
        --display_countdown;
    }
}

void obc_refresh()
{
    if (last_message != -1)
        display_countdown = DURATION;
}

const char *obc_ship_name(int type)
{
    if ((type < -2) || (type > 33))
        type = 0;

    return ship_names[type + 2];
}
