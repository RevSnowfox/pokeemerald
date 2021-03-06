#include "global.h"
#include "pokemon.h"
#include "sprite.h"
#include "pokeball.h"
#include "battle.h"
#include "battle_anim.h"
#include "task.h"
#include "sound.h"
#include "constants/songs.h"
#include "trig.h"
#include "main.h"
#include "m4a.h"
#include "decompress.h"
#include "constants/species.h"
#include "util.h"
#include "graphics.h"

extern bool8 gDoingBattleAnim;
extern u8 gActiveBank;
extern u8 gBankTarget;
extern u16 gBattlePartyID[];
extern u8 gBankSpriteIds[];
extern u8 gHealthBoxesIds[];
extern struct MusicPlayerInfo gMPlayInfo_BGM;

// this file's functions
static void Task_DoPokeballSendOutAnim(u8 taskId);
static void SpriteCB_TestBallThrow(struct Sprite *sprite);
static void SpriteCB_PlayerMonSendOut_1(struct Sprite *sprite);
static void SpriteCB_PlayerMonSendOut_2(struct Sprite *sprite);
static void SpriteCB_OpponentMonSendOut(struct Sprite *sprite);
static void sub_80756D4(struct Sprite *sprite);
static void sub_80756E0(struct Sprite *sprite);
static void sub_807574C(struct Sprite *sprite);
static void sub_80757E4(struct Sprite *sprite);
static void sub_8075838(struct Sprite *sprite);
static void sub_8075930(struct Sprite *sprite);
static void SpriteCB_ReleaseMonFromBall(struct Sprite *sprite);
static void SpriteCB_ReleaseMon2FromBall(struct Sprite *sprite);
static void sub_8075970(struct Sprite *sprite);
static void HandleBallAnimEnd(struct Sprite *sprite);
static void sub_8075FB4(struct Sprite *sprite);
static void sub_80760F8(struct Sprite *sprite);
static void sub_8076524(struct Sprite *sprite);
static void sub_80765E0(struct Sprite *sprite);
static void sub_80767D4(struct Sprite *sprite);
static void sub_807687C(struct Sprite *sprite);
static void sub_80768F0(struct Sprite *sprite);
static void sub_80769A8(struct Sprite *sprite);
static void sub_80769CC(struct Sprite *sprite);
static void SpriteCB_HitAnimHealthoxEffect(struct Sprite *sprite);
static u16 GetBankPokeballItemId(u8 bank);

// rom const data

#define GFX_TAG_POKEBALL    55000
#define GFX_TAG_GREATBALL   55001
#define GFX_TAG_SAFARIBALL  55002
#define GFX_TAG_ULTRABALL   55003
#define GFX_TAG_MASTERBALL  55004
#define GFX_TAG_NETBALL     55005
#define GFX_TAG_DIVEBALL    55006
#define GFX_TAG_NESTBALL    55007
#define GFX_TAG_REPEATBALL  55008
#define GFX_TAG_TIMERBALL   55009
#define GFX_TAG_LUXURYBALL  55010
#define GFX_TAG_PREMIERBALL 55011

const struct CompressedSpriteSheet gBallSpriteSheets[POKEBALL_COUNT] =
{
    {gInterfaceGfx_PokeBall,    384, GFX_TAG_POKEBALL},
    {gInterfaceGfx_GreatBall,   384, GFX_TAG_GREATBALL},
    {gInterfaceGfx_SafariBall,  384, GFX_TAG_SAFARIBALL},
    {gInterfaceGfx_UltraBall,   384, GFX_TAG_ULTRABALL},
    {gInterfaceGfx_MasterBall,  384, GFX_TAG_MASTERBALL},
    {gInterfaceGfx_NetBall,     384, GFX_TAG_NETBALL},
    {gInterfaceGfx_DiveBall,    384, GFX_TAG_DIVEBALL},
    {gInterfaceGfx_NestBall,    384, GFX_TAG_NESTBALL},
    {gInterfaceGfx_RepeatBall,  384, GFX_TAG_REPEATBALL},
    {gInterfaceGfx_TimerBall,   384, GFX_TAG_TIMERBALL},
    {gInterfaceGfx_LuxuryBall,  384, GFX_TAG_LUXURYBALL},
    {gInterfaceGfx_PremierBall, 384, GFX_TAG_PREMIERBALL},
};

const struct CompressedSpritePalette gBallSpritePalettes[POKEBALL_COUNT] =
{
    {gInterfacePal_PokeBall,    GFX_TAG_POKEBALL},
    {gInterfacePal_GreatBall,   GFX_TAG_GREATBALL},
    {gInterfacePal_SafariBall,  GFX_TAG_SAFARIBALL},
    {gInterfacePal_UltraBall,   GFX_TAG_ULTRABALL},
    {gInterfacePal_MasterBall,  GFX_TAG_MASTERBALL},
    {gInterfacePal_NetBall,     GFX_TAG_NETBALL},
    {gInterfacePal_DiveBall,    GFX_TAG_DIVEBALL},
    {gInterfacePal_NestBall,    GFX_TAG_NESTBALL},
    {gInterfacePal_RepeatBall,  GFX_TAG_REPEATBALL},
    {gInterfacePal_TimerBall,   GFX_TAG_TIMERBALL},
    {gInterfacePal_LuxuryBall,  GFX_TAG_LUXURYBALL},
    {gInterfacePal_PremierBall, GFX_TAG_PREMIERBALL},
};

static const struct OamData sBallOamData =
{
    .y = 0,
    .affineMode = 3,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sBallAnimSeq3[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq5[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq4[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq6[] =
{
    ANIMCMD_FRAME(12, 1),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sBallAnimSeq0[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sBallAnimSeq1[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sBallAnimSeq2[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sBallAnimSequences[] =
{
    sBallAnimSeq0,
    sBallAnimSeq1,
    sBallAnimSeq2,

    // unused?
    sBallAnimSeq3,
    sBallAnimSeq4,
    sBallAnimSeq5,
    sBallAnimSeq6,
};

static const union AffineAnimCmd sBallAffineAnimSeq0[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 0, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sBallAffineAnimSeq1[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -3, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sBallAffineAnimSeq2[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 3, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sBallAffineAnimSeq3[] =
{
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sBallAffineAnimSeq4[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 25, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd *const sBallAffineAnimSequences[] =
{
    sBallAffineAnimSeq0,
    sBallAffineAnimSeq1,
    sBallAffineAnimSeq2,
    sBallAffineAnimSeq3,
    sBallAffineAnimSeq4,
};

const struct SpriteTemplate gBallSpriteTemplates[POKEBALL_COUNT] =
{
    {
        .tileTag = GFX_TAG_POKEBALL,
        .paletteTag = GFX_TAG_POKEBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_GREATBALL,
        .paletteTag = GFX_TAG_GREATBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_SAFARIBALL,
        .paletteTag = GFX_TAG_SAFARIBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_ULTRABALL,
        .paletteTag = GFX_TAG_ULTRABALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_MASTERBALL,
        .paletteTag = GFX_TAG_MASTERBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_NETBALL,
        .paletteTag = GFX_TAG_NETBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_DIVEBALL,
        .paletteTag = GFX_TAG_DIVEBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_NESTBALL,
        .paletteTag = GFX_TAG_NESTBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_REPEATBALL,
        .paletteTag = GFX_TAG_REPEATBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_TIMERBALL,
        .paletteTag = GFX_TAG_TIMERBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_LUXURYBALL,
        .paletteTag = GFX_TAG_LUXURYBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
    {
        .tileTag = GFX_TAG_PREMIERBALL,
        .paletteTag = GFX_TAG_PREMIERBALL,
        .oam = &sBallOamData,
        .anims = sBallAnimSequences,
        .images = NULL,
        .affineAnims = sBallAffineAnimSequences,
        .callback = SpriteCB_TestBallThrow,
    },
};

#define tFrames         data[0]
#define tPan            data[1]
#define tThrowId        data[2]
#define tBank           data[3]
#define tOpponentBank   data[4]

u8 DoPokeballSendOutAnimation(s16 pan, u8 kindOfThrow)
{
    u8 taskId;

    gDoingBattleAnim = TRUE;
    gBattleSpritesDataPtr->healthBoxesData[gActiveBank].ballAnimActive = 1;

    taskId = CreateTask(Task_DoPokeballSendOutAnim, 5);
    gTasks[taskId].tPan = pan;
    gTasks[taskId].tThrowId = kindOfThrow;
    gTasks[taskId].tBank = gActiveBank;

    return 0;
}

#define sBank           data[6]

static void Task_DoPokeballSendOutAnim(u8 taskId)
{
    u16 throwCaseId;
    u8 bank;
    u16 itemId, ballId;
    u8 ballSpriteId;
    bool8 notSendOut = FALSE;

    if (gTasks[taskId].tFrames == 0)
    {
        gTasks[taskId].tFrames++;
        return;
    }

    throwCaseId = gTasks[taskId].tThrowId;
    bank = gTasks[taskId].tBank;

    if (GetBankSide(bank) != SIDE_PLAYER)
        itemId = GetMonData(&gEnemyParty[gBattlePartyID[bank]], MON_DATA_POKEBALL);
    else
        itemId = GetMonData(&gPlayerParty[gBattlePartyID[bank]], MON_DATA_POKEBALL);

    ballId = ItemIdToBallId(itemId);
    LoadBallGfx(ballId);
    ballSpriteId = CreateSprite(&gBallSpriteTemplates[ballId], 32, 80, 29);
    gSprites[ballSpriteId].data[0] = 0x80;
    gSprites[ballSpriteId].data[1] = 0;
    gSprites[ballSpriteId].data[7] = throwCaseId;

    switch (throwCaseId)
    {
    case POKEBALL_PLAYER_SENDOUT:
        gBankTarget = bank;
        gSprites[ballSpriteId].pos1.x = 24;
        gSprites[ballSpriteId].pos1.y = 68;
        gSprites[ballSpriteId].callback = SpriteCB_PlayerMonSendOut_1;
        break;
    case POKEBALL_OPPONENT_SENDOUT:
        gSprites[ballSpriteId].pos1.x = GetBankPosition(bank, BANK_X_POS);
        gSprites[ballSpriteId].pos1.y = GetBankPosition(bank, BANK_Y_POS) + 24;
        gBankTarget = bank;
        gSprites[ballSpriteId].data[0] = 0;
        gSprites[ballSpriteId].callback = SpriteCB_OpponentMonSendOut;
        break;
    default:
        gBankTarget = GetBankByIdentity(IDENTITY_OPPONENT_MON1);
        notSendOut = TRUE;
        break;
    }

    gSprites[ballSpriteId].sBank = gBankTarget;
    if (!notSendOut)
    {
        DestroyTask(taskId);
        return;
    }

    // this will perform an unused ball throw animation
    gSprites[ballSpriteId].data[0] = 0x22;
    gSprites[ballSpriteId].data[2] = GetBankPosition(gBankTarget, BANK_X_POS);
    gSprites[ballSpriteId].data[4] = GetBankPosition(gBankTarget, BANK_Y_POS) - 16;
    gSprites[ballSpriteId].data[5] = -40;
    sub_80A68D4(&gSprites[ballSpriteId]);
    gSprites[ballSpriteId].oam.affineParam = taskId;
    gTasks[taskId].tOpponentBank = gBankTarget;
    gTasks[taskId].func = TaskDummy;
    PlaySE(SE_NAGERU);
}

static void SpriteCB_TestBallThrow(struct Sprite *sprite)
{
    if (AnimateBallThrow(sprite))
    {
        u16 ballId;
        u8 taskId = sprite->oam.affineParam;
        u8 opponentBank = gTasks[taskId].tOpponentBank;
        u8 noOfShakes = gTasks[taskId].tThrowId;

        StartSpriteAnim(sprite, 1);
        sprite->affineAnimPaused = 1;
        sprite->pos1.x += sprite->pos2.x;
        sprite->pos1.y += sprite->pos2.y;
        sprite->pos2.x = 0;
        sprite->pos2.y = 0;
        sprite->data[5] = 0;
        ballId = ItemIdToBallId(GetBankPokeballItemId(opponentBank));
        LaunchBallStarsTask(sprite->pos1.x, sprite->pos1.y - 5, 1, 0x1C, ballId);
        sprite->data[0] = LaunchBallFadeMonTask(FALSE, opponentBank, 14, ballId);
        sprite->sBank = opponentBank;
        sprite->data[7] = noOfShakes;
        DestroyTask(taskId);
        sprite->callback = sub_80756D4;
    }
}

#undef tFrames
#undef tPan
#undef tThrowId
#undef tBank
#undef tOpponentBank

static void sub_80756D4(struct Sprite *sprite)
{
    sprite->callback = sub_80756E0;
}

static void sub_80756E0(struct Sprite *sprite)
{
    if (++sprite->data[5] == 10)
    {
        sprite->data[5] = 0;
        sprite->callback = sub_807574C;
        StartSpriteAffineAnim(&gSprites[gBankSpriteIds[sprite->sBank]], 2);
        AnimateSprite(&gSprites[gBankSpriteIds[sprite->sBank]]);
        gSprites[gBankSpriteIds[sprite->sBank]].data[1] = 0;
    }
}

static void sub_807574C(struct Sprite *sprite)
{
    sprite->data[5]++;
    if (sprite->data[5] == 11)
        PlaySE(SE_SUIKOMU);
    if (gSprites[gBankSpriteIds[sprite->sBank]].affineAnimEnded)
    {
        StartSpriteAnim(sprite, 2);
        gSprites[gBankSpriteIds[sprite->sBank]].invisible = TRUE;
        sprite->data[5] = 0;
        sprite->callback = sub_80757E4;
    }
    else
    {
        gSprites[gBankSpriteIds[sprite->sBank]].data[1] += 0x60;
        gSprites[gBankSpriteIds[sprite->sBank]].pos2.y = -gSprites[gBankSpriteIds[sprite->sBank]].data[1] >> 8;
    }
}

static void sub_80757E4(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        sprite->data[5]++;
        if (sprite->data[5] == 1)
        {
            sprite->data[3] = 0;
            sprite->data[4] = 32;
            sprite->data[5] = 0;
            sprite->pos1.y += Cos(0, 32);
            sprite->pos2.y = -Cos(0, sprite->data[4]);
            sprite->callback = sub_8075838;
        }
    }
}

static void sub_8075838(struct Sprite *sprite)
{
    bool8 r5 = FALSE;

    switch (sprite->data[3] & 0xFF)
    {
    case 0:
        sprite->pos2.y = -Cos(sprite->data[5], sprite->data[4]);
        sprite->data[5] += 4 + (sprite->data[3] >> 8);
        if (sprite->data[5] >= 64)
        {
            sprite->data[4] -= 10;
            sprite->data[3] += 0x101;
            if (sprite->data[3] >> 8 == 4)
                r5 = TRUE;
            switch (sprite->data[3] >> 8)
            {
            case 1:
                PlaySE(SE_KON);
                break;
            case 2:
                PlaySE(SE_KON2);
                break;
            case 3:
                PlaySE(SE_KON3);
                break;
            default:
                PlaySE(SE_KON4);
                break;
            }
        }
        break;
    case 1:
        sprite->pos2.y = -Cos(sprite->data[5], sprite->data[4]);
        sprite->data[5] -= 4 + (sprite->data[3] >> 8);
        if (sprite->data[5] <= 0)
        {
            sprite->data[5] = 0;
            sprite->data[3] &= 0xFF00;
        }
        break;
    }
    if (r5)
    {
        sprite->data[3] = 0;
        sprite->pos1.y += Cos(64, 32);
        sprite->pos2.y = 0;
        if (sprite->data[7] == 0)
        {
            sprite->callback = SpriteCB_ReleaseMonFromBall;
        }
        else
        {
            sprite->callback = sub_8075930;
            sprite->data[4] = 1;
            sprite->data[5] = 0;
        }
    }
}

static void sub_8075930(struct Sprite *sprite)
{
    sprite->data[3]++;
    if (sprite->data[3] == 31)
    {
        sprite->data[3] = 0;
        sprite->affineAnimPaused = TRUE;
        StartSpriteAffineAnim(sprite, 1);
        sprite->callback = sub_8075970;
        PlaySE(SE_BOWA);
    }
}

static void sub_8075970(struct Sprite *sprite)
{
    switch (sprite->data[3] & 0xFF)
    {
    case 0:
    case 2:
        sprite->pos2.x += sprite->data[4];
        sprite->data[5] += sprite->data[4];
        sprite->affineAnimPaused = FALSE;
        if (sprite->data[5] > 3 || sprite->data[5] < -3)
        {
            sprite->data[3]++;
            sprite->data[5] = 0;
        }
        break;
    case 1:
        sprite->data[5]++;
        if (sprite->data[5] == 1)
        {
            sprite->data[5] = 0;
            sprite->data[4] = -sprite->data[4];
            sprite->data[3]++;
            sprite->affineAnimPaused = FALSE;
            if (sprite->data[4] < 0)
                ChangeSpriteAffineAnim(sprite, 2);
            else
                ChangeSpriteAffineAnim(sprite, 1);
        }
        else
        {
            sprite->affineAnimPaused = TRUE;
        }
        break;
    case 3:
        sprite->data[3] += 0x100;
        if (sprite->data[3] >> 8 == sprite->data[7])
        {
            sprite->callback = SpriteCB_ReleaseMonFromBall;
        }
        else
        {
            if (sprite->data[7] == 4 && sprite->data[3] >> 8 == 3)
            {
                sprite->callback = sub_8075FB4;
                sprite->affineAnimPaused = TRUE;
            }
            else
            {
                sprite->data[3]++;
                sprite->affineAnimPaused = TRUE;
            }
        }
        break;
    case 4:
    default:
        sprite->data[5]++;
        if (sprite->data[5] == 31)
        {
            sprite->data[5] = 0;
            sprite->data[3] &= 0xFF00;
            StartSpriteAffineAnim(sprite, 3);
            if (sprite->data[4] < 0)
                StartSpriteAffineAnim(sprite, 2);
            else
                StartSpriteAffineAnim(sprite, 1);
            PlaySE(SE_BOWA);
        }
        break;
    }
}

#define tCryTaskSpecies         data[0]
#define tCryTaskPan             data[1]
#define tCryTaskWantedCry       data[2]
#define tCryTaskBank            data[3]
#define tCryTaskMonSpriteId     data[4]
#define tCryTaskMonPtr1         data[5]
#define tCryTaskMonPtr2         data[6]
#define tCryTaskFrames          data[10]
#define tCryTaskState           data[15]

static void Task_PlayCryWhenReleasedFromBall(u8 taskId)
{
    u8 wantedCry = gTasks[taskId].tCryTaskWantedCry;
    s8 pan = gTasks[taskId].tCryTaskPan;
    u16 species = gTasks[taskId].tCryTaskSpecies;
    u8 bank = gTasks[taskId].tCryTaskBank;
    u8 monSpriteId = gTasks[taskId].tCryTaskMonSpriteId;
    struct Pokemon *mon = (void*)(u32)((gTasks[taskId].tCryTaskMonPtr1 << 0x10) | (u16)(gTasks[taskId].tCryTaskMonPtr2));

    switch (gTasks[taskId].tCryTaskState)
    {
    case 0:
    default:
        if (gSprites[monSpriteId].affineAnimEnded)
            gTasks[taskId].tCryTaskState = wantedCry + 1;
        break;
    case 1:
        if (ShouldPlayNormalPokeCry(mon) == TRUE)
            PlayCry3(species, pan, 0);
        else
            PlayCry3(species, pan, 11);
        gBattleSpritesDataPtr->healthBoxesData[bank].field_1_x40 = 0;
        DestroyTask(taskId);
        break;
    case 2:
        StopCryAndClearCrySongs();
        gTasks[taskId].tCryTaskFrames = 3;
        gTasks[taskId].tCryTaskState = 20;
        break;
    case 20:
        if (gTasks[taskId].tCryTaskFrames == 0)
        {
            if (ShouldPlayNormalPokeCry(mon) == TRUE)
                PlayCry4(species, pan, 1);
            else
                PlayCry4(species, pan, 12);

            gBattleSpritesDataPtr->healthBoxesData[bank].field_1_x40 = 0;
            DestroyTask(taskId);
        }
        else
        {
            gTasks[taskId].tCryTaskFrames--;
        }
        break;
    case 3:
        gTasks[taskId].tCryTaskFrames = 6;
        gTasks[taskId].tCryTaskState = 30;
        break;
    case 30:
        if (gTasks[taskId].tCryTaskFrames != 0)
        {
            gTasks[taskId].tCryTaskFrames--;
            break;
        }
        gTasks[taskId].tCryTaskState++;
        // fall through
    case 31:
        if (!IsCryPlayingOrClearCrySongs())
        {
            StopCryAndClearCrySongs();
            gTasks[taskId].tCryTaskFrames = 3;
            gTasks[taskId].tCryTaskState++;
        }
        break;
    case 32:
        if (gTasks[taskId].tCryTaskFrames != 0)
        {
            gTasks[taskId].tCryTaskFrames--;
            break;
        }
        if (ShouldPlayNormalPokeCry(mon) == TRUE)
            PlayCry4(species, pan, 0);
        else
            PlayCry4(species, pan, 11);

        gBattleSpritesDataPtr->healthBoxesData[bank].field_1_x40 = 0;
        DestroyTask(taskId);
        break;
    }
}

static void SpriteCB_ReleaseMonFromBall(struct Sprite *sprite)
{
    u8 bank = sprite->sBank;
    u32 ballId;

    StartSpriteAnim(sprite, 1);
    ballId = ItemIdToBallId(GetBankPokeballItemId(bank));
    LaunchBallStarsTask(sprite->pos1.x, sprite->pos1.y - 5, 1, 0x1C, ballId);
    sprite->data[0] = LaunchBallFadeMonTask(1, sprite->sBank, 14, ballId);
    sprite->callback = HandleBallAnimEnd;

    if (gMain.inBattle)
    {
        struct Pokemon *mon;
        u16 species;
        s8 pan;
        u16 wantedCryCase;
        u8 taskId;

        if (GetBankSide(bank) != SIDE_PLAYER)
        {
            mon = &gEnemyParty[gBattlePartyID[bank]];
            pan = 25;
        }
        else
        {
            mon = &gPlayerParty[gBattlePartyID[bank]];
            pan = -25;
        }

        species = GetMonData(mon, MON_DATA_SPECIES);
        if ((bank == GetBankByIdentity(IDENTITY_PLAYER_MON1) || bank == GetBankByIdentity(IDENTITY_OPPONENT_MON1))
         && IsDoubleBattle() && gBattleSpritesDataPtr->animationData->field_9_x1)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI && gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                if (IsBGMPlaying())
                    m4aMPlayStop(&gMPlayInfo_BGM);
            }
            else
            {
                m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 128);
            }
        }

        if (!IsDoubleBattle() || !gBattleSpritesDataPtr->animationData->field_9_x1)
            wantedCryCase = 0;
        else if (bank == GetBankByIdentity(IDENTITY_PLAYER_MON1) || bank == GetBankByIdentity(IDENTITY_OPPONENT_MON1))
            wantedCryCase = 1;
        else
            wantedCryCase = 2;

        gBattleSpritesDataPtr->healthBoxesData[bank].field_1_x40 = 1;

        taskId = CreateTask(Task_PlayCryWhenReleasedFromBall, 3);
        gTasks[taskId].tCryTaskSpecies = species;
        gTasks[taskId].tCryTaskPan = pan;
        gTasks[taskId].tCryTaskWantedCry = wantedCryCase;
        gTasks[taskId].tCryTaskBank = bank;
        gTasks[taskId].tCryTaskMonSpriteId = gBankSpriteIds[sprite->sBank];
        gTasks[taskId].tCryTaskMonPtr1 = (u32)(mon) >> 0x10;
        gTasks[taskId].tCryTaskMonPtr2 = (u32)(mon);
        gTasks[taskId].tCryTaskState = 0;
    }

    StartSpriteAffineAnim(&gSprites[gBankSpriteIds[sprite->sBank]], 1);

    if (GetBankSide(sprite->sBank) == SIDE_OPPONENT)
        gSprites[gBankSpriteIds[sprite->sBank]].callback = sub_8039B58;
    else
        gSprites[gBankSpriteIds[sprite->sBank]].callback = sub_8039E44;

    AnimateSprite(&gSprites[gBankSpriteIds[sprite->sBank]]);
    gSprites[gBankSpriteIds[sprite->sBank]].data[1] = 0x1000;
}

#undef tCryTaskSpecies
#undef tCryTaskPan
#undef tCryTaskWantedCry
#undef tCryTaskBank
#undef tCryTaskMonSpriteId
#undef tCryTaskMonPtr1
#undef tCryTaskMonPtr2
#undef tCryTaskFrames
#undef tCryTaskState

static void sub_8075FB4(struct Sprite *sprite)
{
    sprite->animPaused = TRUE;
    sprite->callback = sub_80760F8;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    sprite->data[5] = 0;
}

static void HandleBallAnimEnd(struct Sprite *sprite)
{
    bool8 affineAnimEnded = FALSE;
    u8 bank = sprite->sBank;

    gSprites[gBankSpriteIds[bank]].invisible = FALSE;
    if (sprite->animEnded)
        sprite->invisible = TRUE;
    if (gSprites[gBankSpriteIds[bank]].affineAnimEnded)
    {
        StartSpriteAffineAnim(&gSprites[gBankSpriteIds[bank]], 0);
        affineAnimEnded = TRUE;
    }
    else
    {
        gSprites[gBankSpriteIds[bank]].data[1] -= 288;
        gSprites[gBankSpriteIds[bank]].pos2.y = gSprites[gBankSpriteIds[bank]].data[1] >> 8;
    }
    if (sprite->animEnded && affineAnimEnded)
    {
        s32 i, doneBanks;

        gSprites[gBankSpriteIds[bank]].pos2.y = 0;
        gDoingBattleAnim = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[bank].ballAnimActive = 0;
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);

        for (doneBanks = 0, i = 0; i < BATTLE_BANKS_COUNT; i++)
        {
            if (gBattleSpritesDataPtr->healthBoxesData[i].ballAnimActive == 0)
                doneBanks++;
        }
        if (doneBanks == BATTLE_BANKS_COUNT)
        {
            for (i = 0; i < POKEBALL_COUNT; i++)
                FreeBallGfx(i);
        }
    }
}

static void sub_80760F8(struct Sprite *sprite)
{
    u8 bank = sprite->sBank;

    sprite->data[4]++;
    if (sprite->data[4] == 40)
    {
        return;
    }
    else if (sprite->data[4] == 95)
    {
        gDoingBattleAnim = FALSE;
        m4aMPlayAllStop();
        PlaySE(MUS_FANFA5);
    }
    else if (sprite->data[4] == 315)
    {
        FreeOamMatrix(gSprites[gBankSpriteIds[sprite->sBank]].oam.matrixNum);
        DestroySprite(&gSprites[gBankSpriteIds[sprite->sBank]]);
        DestroySpriteAndFreeResources(sprite);
        if (gMain.inBattle)
            gBattleSpritesDataPtr->healthBoxesData[bank].ballAnimActive = 0;
    }
}

static void SpriteCB_PlayerMonSendOut_1(struct Sprite *sprite)
{
    sprite->data[0] = 25;
    sprite->data[2] = GetBankPosition(sprite->sBank, 2);
    sprite->data[4] = GetBankPosition(sprite->sBank, 3) + 24;
    sprite->data[5] = -30;
    sprite->oam.affineParam = sprite->sBank;
    sub_80A68D4(sprite);
    sprite->callback = SpriteCB_PlayerMonSendOut_2;
}

#define HIBYTE(x) (((x) >> 8) & 0xFF)

static void SpriteCB_PlayerMonSendOut_2(struct Sprite *sprite)
{
    u32 r6;
    u32 r7;

    if (HIBYTE(sprite->data[7]) >= 35 && HIBYTE(sprite->data[7]) < 80)
    {
        s16 r4;

        if ((sprite->oam.affineParam & 0xFF00) == 0)
        {
            r6 = sprite->data[1] & 1;
            r7 = sprite->data[2] & 1;
            sprite->data[1] = ((sprite->data[1] / 3) & ~1) | r6;
            sprite->data[2] = ((sprite->data[2] / 3) & ~1) | r7;
            StartSpriteAffineAnim(sprite, 4);
        }
        r4 = sprite->data[0];
        sub_80A6F3C(sprite);
        sprite->data[7] += sprite->sBank / 3;
        sprite->pos2.y += Sin(HIBYTE(sprite->data[7]), sprite->data[5]);
        sprite->oam.affineParam += 0x100;
        if ((sprite->oam.affineParam >> 8) % 3 != 0)
            sprite->data[0] = r4;
        else
            sprite->data[0] = r4 - 1;
        if (HIBYTE(sprite->data[7]) >= 80)
        {
            r6 = sprite->data[1] & 1;
            r7 = sprite->data[2] & 1;
            sprite->data[1] = ((sprite->data[1] * 3) & ~1) | r6;
            sprite->data[2] = ((sprite->data[2] * 3) & ~1) | r7;
        }
    }
    else
    {
        if (AnimateBallThrow(sprite))
        {
            sprite->pos1.x += sprite->pos2.x;
            sprite->pos1.y += sprite->pos2.y;
            sprite->pos2.y = 0;
            sprite->pos2.x = 0;
            sprite->sBank = sprite->oam.affineParam & 0xFF;
            sprite->data[0] = 0;

            if (IsDoubleBattle() && gBattleSpritesDataPtr->animationData->field_9_x1
             && sprite->sBank == GetBankByIdentity(IDENTITY_PLAYER_MON2))
                sprite->callback = SpriteCB_ReleaseMon2FromBall;
            else
                sprite->callback = SpriteCB_ReleaseMonFromBall;

            StartSpriteAffineAnim(sprite, 0);
        }
    }
}

static void SpriteCB_ReleaseMon2FromBall(struct Sprite *sprite)
{
    if (sprite->data[0]++ > 24)
    {
        sprite->data[0] = 0;
        sprite->callback = SpriteCB_ReleaseMonFromBall;
    }
}

static void SpriteCB_OpponentMonSendOut(struct Sprite *sprite)
{
    sprite->data[0]++;
    if (sprite->data[0] > 15)
    {
        sprite->data[0] = 0;
        if (IsDoubleBattle() && gBattleSpritesDataPtr->animationData->field_9_x1
         && sprite->sBank == GetBankByIdentity(IDENTITY_OPPONENT_MON2))
            sprite->callback = SpriteCB_ReleaseMon2FromBall;
        else
            sprite->callback = SpriteCB_ReleaseMonFromBall;
    }
}

#undef sBank

static u8 LaunchBallStarsTaskForPokeball(u8 x, u8 y, u8 kindOfStars, u8 d)
{
    return LaunchBallStarsTask(x, y, kindOfStars, d, BALL_POKE);
}

static u8 LaunchBallFadeMonTaskForPokeball(bool8 unFadeLater, u8 bank, u32 arg2)
{
    return LaunchBallFadeMonTask(unFadeLater, bank, arg2, BALL_POKE);
}

void CreatePokeballSpriteToReleaseMon(u8 monSpriteId, u8 bank, u8 x, u8 y, u8 oamPriority, u8 subpriortiy, u8 g, u32 h, u16 species)
{
    u8 spriteId;

    LoadCompressedObjectPicUsingHeap(&gBallSpriteSheets[0]);
    LoadCompressedObjectPaletteUsingHeap(&gBallSpritePalettes[0]);
    spriteId = CreateSprite(&gBallSpriteTemplates[0], x, y, subpriortiy);

    gSprites[spriteId].data[0] = monSpriteId;
    gSprites[spriteId].data[5] = gSprites[monSpriteId].pos1.x;
    gSprites[spriteId].data[6] = gSprites[monSpriteId].pos1.y;

    gSprites[monSpriteId].pos1.x = x;
    gSprites[monSpriteId].pos1.y = y;
    gSprites[monSpriteId].data[7] = species;

    gSprites[spriteId].data[1] = g;
    gSprites[spriteId].data[2] = bank;
    gSprites[spriteId].data[3] = h;
    gSprites[spriteId].data[4] = h >> 0x10;
    gSprites[spriteId].oam.priority = oamPriority;
    gSprites[spriteId].callback = sub_8076524;

    gSprites[monSpriteId].invisible = TRUE;
}

static void sub_8076524(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        u8 r5;
        u8 r7 = sprite->data[0];
        u8 bank = sprite->data[2];
        u32 r4 = (u16)sprite->data[3] | ((u16)sprite->data[4] << 16);

        if (sprite->subpriority != 0)
            r5 = sprite->subpriority - 1;
        else
            r5 = 0;

        StartSpriteAnim(sprite, 1);
        LaunchBallStarsTaskForPokeball(sprite->pos1.x, sprite->pos1.y - 5, sprite->oam.priority, r5);
        sprite->data[1] = LaunchBallFadeMonTaskForPokeball(1, bank, r4);
        sprite->callback = sub_80765E0;
        gSprites[r7].invisible = FALSE;
        StartSpriteAffineAnim(&gSprites[r7], 1);
        AnimateSprite(&gSprites[r7]);
        gSprites[r7].data[1] = 0x1000;
        sprite->data[7] = 0;
    }
    else
    {
        sprite->data[1]--;
    }
}

static void sub_80765E0(struct Sprite *sprite)
{
    bool8 r12 = FALSE;
    bool8 r6 = FALSE;
    u8 monSpriteId = sprite->data[0];
    u16 var1;
    u16 var2;

    if (sprite->animEnded)
        sprite->invisible = TRUE;
    if (gSprites[monSpriteId].affineAnimEnded)
    {
        StartSpriteAffineAnim(&gSprites[monSpriteId], 0);
        r12 = TRUE;
    }
    var1 = (sprite->data[5] - sprite->pos1.x) * sprite->data[7] / 128 + sprite->pos1.x;
    var2 = (sprite->data[6] - sprite->pos1.y) * sprite->data[7] / 128 + sprite->pos1.y;
    gSprites[monSpriteId].pos1.x = var1;
    gSprites[monSpriteId].pos1.y = var2;
    if (sprite->data[7] < 128)
    {
        s16 sine = -(gSineTable[(u8)sprite->data[7]] / 8);

        sprite->data[7] += 4;
        gSprites[monSpriteId].pos2.x = sine;
        gSprites[monSpriteId].pos2.y = sine;
    }
    else
    {
        gSprites[monSpriteId].pos1.x = sprite->data[5];
        gSprites[monSpriteId].pos1.y = sprite->data[6];
        gSprites[monSpriteId].pos2.x = 0;
        gSprites[monSpriteId].pos2.y = 0;
        r6 = TRUE;
    }
    if (sprite->animEnded && r12 && r6)
    {
        if (gSprites[monSpriteId].data[7] == SPECIES_EGG)
            DoMonFrontSpriteAnimation(&gSprites[monSpriteId], gSprites[monSpriteId].data[7], TRUE, 0);
        else
            DoMonFrontSpriteAnimation(&gSprites[monSpriteId], gSprites[monSpriteId].data[7], FALSE, 0);

        DestroySpriteAndFreeResources(sprite);
    }
}

u8 sub_807671C(u8 a, u8 b, u8 x, u8 y, u8 oamPriority, u8 subPriority, u8 g, u32 h)
{
    u8 spriteId;

    LoadCompressedObjectPicUsingHeap(&gBallSpriteSheets[0]);
    LoadCompressedObjectPaletteUsingHeap(&gBallSpritePalettes[0]);
    spriteId = CreateSprite(&gBallSpriteTemplates[0], x, y, subPriority);
    gSprites[spriteId].data[0] = a;
    gSprites[spriteId].data[1] = g;
    gSprites[spriteId].data[2] = b;
    gSprites[spriteId].data[3] = h;
    gSprites[spriteId].data[4] = h >> 16;
    gSprites[spriteId].oam.priority = oamPriority;
    gSprites[spriteId].callback = sub_80767D4;
    return spriteId;
}

static void sub_80767D4(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        u8 r6;
        u8 r7 = sprite->data[0];
        u8 r8 = sprite->data[2];
        u32 r5 = (u16)sprite->data[3] | ((u16)sprite->data[4] << 16);

        if (sprite->subpriority != 0)
            r6 = sprite->subpriority - 1;
        else
            r6 = 0;

        StartSpriteAnim(sprite, 1);
        LaunchBallStarsTaskForPokeball(sprite->pos1.x, sprite->pos1.y - 5, sprite->oam.priority, r6);
        sprite->data[1] = LaunchBallFadeMonTaskForPokeball(1, r8, r5);
        sprite->callback = sub_807687C;
        StartSpriteAffineAnim(&gSprites[r7], 2);
        AnimateSprite(&gSprites[r7]);
        gSprites[r7].data[1] = 0;
    }
    else
    {
        sprite->data[1]--;
    }
}

static void sub_807687C(struct Sprite *sprite)
{
    u8 r1;

    sprite->data[5]++;
    if (sprite->data[5] == 11)
        PlaySE(SE_SUIKOMU);
    r1 = sprite->data[0];
    if (gSprites[r1].affineAnimEnded)
    {
        StartSpriteAnim(sprite, 2);
        gSprites[r1].invisible = TRUE;
        sprite->data[5] = 0;
        sprite->callback = sub_80768F0;
    }
    else
    {
        gSprites[r1].data[1] += 96;
        gSprites[r1].pos2.y = -gSprites[r1].data[1] >> 8;
    }
}

static void sub_80768F0(struct Sprite *sprite)
{
    if (sprite->animEnded)
        sprite->callback = SpriteCallbackDummy;
}

static void DestroySpriteAndFreeResources_(struct Sprite *sprite)
{
    DestroySpriteAndFreeResources(sprite);
}

void sub_8076918(u8 bank)
{
    struct Sprite *healthboxSprite = &gSprites[gHealthBoxesIds[bank]];

    healthboxSprite->data[0] = 5;
    healthboxSprite->data[1] = 0;
    healthboxSprite->pos2.x = 0x73;
    healthboxSprite->pos2.y = 0;
    healthboxSprite->callback = sub_80769CC;
    if (GetBankSide(bank) != SIDE_PLAYER)
    {
        healthboxSprite->data[0] = -healthboxSprite->data[0];
        healthboxSprite->data[1] = -healthboxSprite->data[1];
        healthboxSprite->pos2.x = -healthboxSprite->pos2.x;
        healthboxSprite->pos2.y = -healthboxSprite->pos2.y;
    }
    gSprites[healthboxSprite->data[5]].callback(&gSprites[healthboxSprite->data[5]]);
    if (GetBankIdentity(bank) == IDENTITY_PLAYER_MON2)
        healthboxSprite->callback = sub_80769A8;
}

static void sub_80769A8(struct Sprite *sprite)
{
    sprite->data[1]++;
    if (sprite->data[1] == 20)
    {
        sprite->data[1] = 0;
        sprite->callback = sub_80769CC;
    }
}

static void sub_80769CC(struct Sprite *sprite)
{
    sprite->pos2.x -= sprite->data[0];
    sprite->pos2.y -= sprite->data[1];
    if (sprite->pos2.x == 0 && sprite->pos2.y == 0)
        sprite->callback = SpriteCallbackDummy;
}

void DoHitAnimHealthboxEffect(u8 bank)
{
    u8 spriteId;

    spriteId = CreateInvisibleSpriteWithCallback(SpriteCB_HitAnimHealthoxEffect);
    gSprites[spriteId].data[0] = 1;
    gSprites[spriteId].data[1] = gHealthBoxesIds[bank];
    gSprites[spriteId].callback = SpriteCB_HitAnimHealthoxEffect;
}

static void SpriteCB_HitAnimHealthoxEffect(struct Sprite *sprite)
{
    u8 r1 = sprite->data[1];

    gSprites[r1].pos2.y = sprite->data[0];
    sprite->data[0] = -sprite->data[0];
    sprite->data[2]++;
    if (sprite->data[2] == 21)
    {
        gSprites[r1].pos2.x = 0;
        gSprites[r1].pos2.y = 0;
        DestroySprite(sprite);
    }
}

void LoadBallGfx(u8 ballId)
{
    u16 var;

    if (GetSpriteTileStartByTag(gBallSpriteSheets[ballId].tag) == 0xFFFF)
    {
        LoadCompressedObjectPicUsingHeap(&gBallSpriteSheets[ballId]);
        LoadCompressedObjectPaletteUsingHeap(&gBallSpritePalettes[ballId]);
    }
    switch (ballId)
    {
    case BALL_DIVE:
    case BALL_LUXURY:
    case BALL_PREMIER:
        break;
    default:
        var = GetSpriteTileStartByTag(gBallSpriteSheets[ballId].tag);
        LZDecompressVram(gOpenPokeballGfx, (void *)(VRAM + 0x10100 + var * 32));
        break;
    }
}

void FreeBallGfx(u8 ballId)
{
    FreeSpriteTilesByTag(gBallSpriteSheets[ballId].tag);
    FreeSpritePaletteByTag(gBallSpritePalettes[ballId].tag);
}

static u16 GetBankPokeballItemId(u8 bank)
{
    if (GetBankSide(bank) == SIDE_PLAYER)
        return GetMonData(&gPlayerParty[gBattlePartyID[bank]], MON_DATA_POKEBALL);
    else
        return GetMonData(&gEnemyParty[gBattlePartyID[bank]], MON_DATA_POKEBALL);
}
