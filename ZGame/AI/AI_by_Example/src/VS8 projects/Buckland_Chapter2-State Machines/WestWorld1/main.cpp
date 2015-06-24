#include "Locations.h"
#include "Miner.h"
#include "misc/ConsoleUtils.h"
#include "EntityNames.h"

//z 2015-06-24 15:58:36 L.190'28884 T458312574 .K
int main()
{
    //create a miner
    Miner miner(ent_Miner_Bob);

    //simply run the miner through a few Update calls
    for (int i=0; i<20; ++i)
    {
        miner.Update();

        Sleep(800);
    }

    //wait for a keypress before exiting
    PressAnyKeyToContinue();

    return 0;
}
