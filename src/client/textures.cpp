#include "Textures.hpp"

rl::Texture2D TEX_DESERT;

rl::Texture2D TEX_SS_RAT;
rl::Texture2D TEX_SS_RAT0;
rl::Texture2D TEX_SS_RAT1;
rl::Texture2D TEX_SS_RAT2;
rl::Texture2D TEX_SS_RAT3;
rl::Texture2D TEX_SS_RAT4;
rl::Texture2D TEX_SS_RAT5;
rl::Texture2D TEX_SS_RATDREAM;
rl::Texture2D TEX_RATS[7];

rl::Texture2D TEX_CARDS[14];
rl::Texture2D TEX_CARDS_SHEET;

void loadTextures() {
    TEX_DESERT = rl::LoadTexture("assets/desert.png");
    TEX_SS_RAT0 = rl::LoadTexture("assets/Dream Rats/Dream Rat 0.png");
    TEX_SS_RAT1 = rl::LoadTexture("assets/Dream Rats/Dream Rat 1.png");
    TEX_SS_RAT2 = rl::LoadTexture("assets/Dream Rats/Dream Rat 2.png");
    TEX_SS_RAT3 = rl::LoadTexture("assets/Dream Rats/Dream Rat 3.png");
    TEX_SS_RAT4 = rl::LoadTexture("assets/Dream Rats/Dream Rat 4.png");
    TEX_SS_RAT5 = rl::LoadTexture("assets/Dream Rats/Dream Rat 5.png");
    TEX_SS_RATDREAM = rl::LoadTexture("assets/Dream Rats/Dream Rat Rare.png");
    TEX_RATS[0] = TEX_SS_RAT0;
    TEX_RATS[1] = TEX_SS_RAT1;
    TEX_RATS[2] = TEX_SS_RAT2;
    TEX_RATS[3] = TEX_SS_RAT3;
    TEX_RATS[4] = TEX_SS_RAT4;
    TEX_RATS[5] = TEX_SS_RAT5;
    TEX_RATS[6] = TEX_SS_RATDREAM;

    TEX_CARDS_SHEET = rl::LoadTexture("assets/card_deck.png");
}
