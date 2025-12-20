#include "global.h"
#include "pokemon.h"
#include "constants/species.h"

#include "data/pokemon_graphics/front_pic_anims.h"

bool8 HasTwoFramesAnimation(u16 species)
{
    return (species != SPECIES_CASTFORM
         && species != SPECIES_DEOXYS
         && species != SPECIES_SPINDA
         && species != SPECIES_UNOWN);
}
