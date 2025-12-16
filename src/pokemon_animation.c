#include "global.h"
#include "pokemon.h"
#include "constants/species.h"

#include "data/pokemon_graphics/front_pic_anims.h"

bool8 HasTwoFramesAnimation(u16 species)
{
    const union AnimCmd *const *anims;
    u32 count;
    
    if (species >= NUM_SPECIES)
        return FALSE;
    
    anims = gMonFrontAnimsPtrTable[species];
    count = 0;
    
    // Count non-NULL pointers
    while (anims[count] != NULL)
        count++;
    
    // If there are 3 pointers (Frame 0, Frame 1, Frame 2), it's a two-frame animation
    return (count == 3);
}
