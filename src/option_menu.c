#include "global.h"
#include "option_menu.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "string_util.h"
#include "strings2.h"
#include "task.h"

extern void SetPokemonCryStereo(u32 val);

// Menu items
enum
{
    MENUITEM_TEXTSPEED,
    MENUITEM_BATTLESCENE,
    MENUITEM_BATTLESTYLE,
    MENUITEM_SOUND,
    MENUITEM_BUTTONMODE,
    MENUITEM_FRAMETYPE,
    MENUITEM_VERSION,
    MENUITEM_TRAINER,
    MENUITEM_CANCEL,
};

// Task data
#define tMenuSelection  data[0]
#define tOptTextSpeed   data[1]
#define tOptBattleScene data[2]
#define tOptBattleStyle data[3]
#define tOptSound       data[4]
#define tOptButtonMode  data[5]
#define tOptFrameType   data[6]
#define tOptVersion     data[7]
#define tOptTrainer     data[8]
#define tScrollOffset   data[9]

#if ENGLISH
#define TEXTSPEED_SLOW_LEFT (120)
#define TEXTSPEED_MIX_LEFT (155)
#define TEXTSPEED_FAST_LEFT (184)
#define BATTLESTYLE_SHIFT (120)
#define BATTLESTYLE_SET (190)
#elif GERMAN
#define TEXTSPEED_SLOW_LEFT (120)
#define TEXTSPEED_MIX_LEFT (161)
#define TEXTSPEED_FAST_LEFT (202)
#define BATTLESTYLE_SHIFT (120)
#define BATTLESTYLE_SET (178)
#endif

const u16 gUnknown_0839F5FC[] = INCBIN_U16("graphics/misc/option_menu_text.gbapal");
// note: this is only used in the Japanese release
const u8 gUnknown_0839F63C[] = INCBIN_U8("graphics/misc/option_menu_equals_sign.4bpp");

static void Task_OptionMenuFadeIn(u8 taskId);
static void Task_OptionMenuProcessInput(u8 taskId);
static void Task_OptionMenuSave(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static void HighlightOptionMenuItem(u8 taskId, u8 index);
static void RedrawOptionMenuItems(u8 taskId);
static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style);
static void FrameType_DrawChoicesAtRow(u8 selection, u8 row);
static u8   TextSpeed_ProcessInput(u8 selection);
static u8   BattleScene_ProcessInput(u8 selection);
static u8   BattleStyle_ProcessInput(u8 selection);
static u8   Sound_ProcessInput(u8 selection);
static u8   FrameType_ProcessInput(u8 selection);
static u8   ButtonMode_ProcessInput(u8 selection);
static u8   Version_ProcessInput(u8 selection);
static u8   Trainer_ProcessInput(u8 selection);

static void MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_InitOptionMenu(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        REG_BG2CNT = 0;
        REG_BG1CNT = 0;
        REG_BG0CNT = 0;
        REG_BG2HOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        DmaFill16Large(3, 0, (u8 *)VRAM, 0x18000, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        gMain.state++;
        break;
    case 1:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 2:
        Text_LoadWindowTemplate(&gWindowTemplate_81E71B4);
        gMain.state++;
        break;
    case 3:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E71B4);
        gMain.state++;
        break;
    case 4:
        if (!MultistepInitMenuWindowContinue())
            return;
        gMain.state++;
        break;
    case 5:
        LoadPalette(gUnknown_0839F5FC, 0x80, 0x40);
        CpuCopy16(gUnknown_0839F63C, (void *)(VRAM + 0xBEE0), 0x40);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gMain.state++;
        break;
    case 7:
    {
        u16 savedIme;

        REG_WIN0H = 0;
        REG_WIN0V = 0;
        REG_WIN1H = 0;
        REG_WIN1V = 0;
        REG_WININ = 0x1111;
        REG_WINOUT = 0x31;
        REG_BLDCNT = 0xE1;
        REG_BLDALPHA = 0;
        REG_BLDY = 7;
        savedIme = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = savedIme;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
        SetVBlankCallback(VBlankCB);
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_OBJ_ON |
          DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
        gMain.state++;
        break;
    }
    case 8:
    {
        u8 taskId = CreateTask(Task_OptionMenuFadeIn, 0);

        gTasks[taskId].tMenuSelection = 0;
        gTasks[taskId].tOptTextSpeed   = gSaveBlock2.optionsTextSpeed;
        gTasks[taskId].tOptBattleScene = gSaveBlock2.optionsBattleSceneOff;
        gTasks[taskId].tOptBattleStyle = gSaveBlock2.optionsBattleStyle;
        gTasks[taskId].tOptSound       = gSaveBlock2.optionsSound;
        gTasks[taskId].tOptButtonMode  = gSaveBlock2.optionsButtonMode;
        gTasks[taskId].tOptFrameType   = gSaveBlock2.optionsWindowFrameType;
        gTasks[taskId].tOptVersion      = gSaveBlock2.optionsVersion;
        gTasks[taskId].tOptTrainer      = gSaveBlock2.optionsTrainer;
        gTasks[taskId].tScrollOffset    = 0;

        Menu_DrawStdWindowFrame(2, 0, 27, 3);   // title box
        Menu_DrawStdWindowFrame(2, 4, 27, 19);  // options list box

        Menu_PrintText(gSystemText_OptionMenu,  4,  1);

        RedrawOptionMenuItems(taskId);

        REG_WIN0H = WIN_RANGE(17, 223);
        REG_WIN0V = WIN_RANGE(1, 31);

        HighlightOptionMenuItem(taskId, gTasks[taskId].tMenuSelection);
        gMain.state++;
        break;
    }
    case 9:
        SetMainCallback2(MainCB);
        return;
    }
}

static void Task_OptionMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_OptionMenuProcessInput;
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
    if (gMain.newKeys & A_BUTTON)
    {
        if (gTasks[taskId].tMenuSelection == MENUITEM_CANCEL)
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (gMain.newKeys & B_BUTTON)
    {
        gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (gMain.newKeys & DPAD_UP)
    {
        u8 oldScroll = gTasks[taskId].tScrollOffset;
        if (gTasks[taskId].tMenuSelection > 0)
        {
            gTasks[taskId].tMenuSelection--;
            // Scroll up if selection is above visible area
            if (gTasks[taskId].tMenuSelection < gTasks[taskId].tScrollOffset)
            {
                gTasks[taskId].tScrollOffset = gTasks[taskId].tMenuSelection;
            }
        }
        else
        {
            // Wrap to last item and scroll to show it
            gTasks[taskId].tMenuSelection = 7;
            gTasks[taskId].tScrollOffset = 1; // Show last 7 items (indices 1-7)
        }
        if (oldScroll != gTasks[taskId].tScrollOffset)
            RedrawOptionMenuItems(taskId);
        HighlightOptionMenuItem(taskId, gTasks[taskId].tMenuSelection);
    }
    else if (gMain.newKeys & DPAD_DOWN)
    {
        u8 oldScroll = gTasks[taskId].tScrollOffset;
        if (gTasks[taskId].tMenuSelection < 8)
        {
            gTasks[taskId].tMenuSelection++;
            // Scroll down if selection is below visible area
            if (gTasks[taskId].tMenuSelection >= gTasks[taskId].tScrollOffset + 7)
            {
                gTasks[taskId].tScrollOffset = gTasks[taskId].tMenuSelection - 6;
            }
        }
        else
        {
            // Wrap to first item and scroll to show it
            gTasks[taskId].tMenuSelection = 0;
            gTasks[taskId].tScrollOffset = 0;
        }
        if (oldScroll != gTasks[taskId].tScrollOffset)
            RedrawOptionMenuItems(taskId);
        HighlightOptionMenuItem(taskId, gTasks[taskId].tMenuSelection);
    }
    else
    {
        {
            u8 scrollOffset = gTasks[taskId].tScrollOffset;
            u8 visibleIndex = gTasks[taskId].tMenuSelection - scrollOffset;
            u8 visibleRows[] = {5, 7, 9, 11, 13, 15, 17};
            u16 yPixels[] = {40, 56, 72, 88, 104, 120, 136};
            u8 row, yPos;
            
            if (visibleIndex < 7)
            {
                row = visibleRows[visibleIndex];
                yPos = yPixels[visibleIndex];
            }
            else
            {
                row = 0;
                yPos = 0;
            }
            
            switch (gTasks[taskId].tMenuSelection)
            {
            case MENUITEM_TEXTSPEED:
                gTasks[taskId].tOptTextSpeed = TextSpeed_ProcessInput(gTasks[taskId].tOptTextSpeed);
                if (visibleIndex < 7)
                {
                    u8 styles[3];
                    u8 sel = gTasks[taskId].tOptTextSpeed;
                    styles[0] = (sel == 0) ? 0x8 : 0xF;
                    styles[1] = (sel == 1) ? 0x8 : 0xF;
                    styles[2] = (sel == 2) ? 0x8 : 0xF;
                    DrawOptionMenuChoice(gSystemText_Slow, TEXTSPEED_SLOW_LEFT, yPos, styles[0]);
                    DrawOptionMenuChoice(gSystemText_Mid, TEXTSPEED_MIX_LEFT, yPos, styles[1]);
                    DrawOptionMenuChoice(gSystemText_Fast, TEXTSPEED_FAST_LEFT, yPos, styles[2]);
                }
                break;
            case MENUITEM_BATTLESCENE:
                gTasks[taskId].tOptBattleScene = BattleScene_ProcessInput(gTasks[taskId].tOptBattleScene);
                if (visibleIndex < 7)
                {
                    u8 styles[2];
                    u8 sel = gTasks[taskId].tOptBattleScene;
                    styles[0] = (sel == 0) ? 0x8 : 0xF;
                    styles[1] = (sel == 1) ? 0x8 : 0xF;
                    DrawOptionMenuChoice(gSystemText_On, 120, yPos, styles[0]);
                    DrawOptionMenuChoice(gSystemText_Off, 190, yPos, styles[1]);
                }
                break;
            case MENUITEM_BATTLESTYLE:
                gTasks[taskId].tOptBattleStyle = BattleStyle_ProcessInput(gTasks[taskId].tOptBattleStyle);
                if (visibleIndex < 7)
                {
                    u8 styles[2];
                    u8 sel = gTasks[taskId].tOptBattleStyle;
                    styles[0] = (sel == 0) ? 0x8 : 0xF;
                    styles[1] = (sel == 1) ? 0x8 : 0xF;
                    DrawOptionMenuChoice(gSystemText_Shift, BATTLESTYLE_SHIFT, yPos, styles[0]);
                    DrawOptionMenuChoice(gSystemText_Set, BATTLESTYLE_SET, yPos, styles[1]);
                }
                break;
            case MENUITEM_SOUND:
                gTasks[taskId].tOptSound = Sound_ProcessInput(gTasks[taskId].tOptSound);
                if (visibleIndex < 7)
                {
                    u8 styles[2];
                    u8 sel = gTasks[taskId].tOptSound;
                    styles[0] = (sel == 0) ? 0x8 : 0xF;
                    styles[1] = (sel == 1) ? 0x8 : 0xF;
                    DrawOptionMenuChoice(gSystemText_Mono, 120, yPos, styles[0]);
                    DrawOptionMenuChoice(gSystemText_Stereo, 172, yPos, styles[1]);
                }
                break;
            case MENUITEM_BUTTONMODE:
                gTasks[taskId].tOptButtonMode = ButtonMode_ProcessInput(gTasks[taskId].tOptButtonMode);
                if (visibleIndex < 7)
                {
                    u8 styles[3];
                    u8 sel = gTasks[taskId].tOptButtonMode;
                    styles[0] = (sel == 0) ? 0x8 : 0xF;
                    styles[1] = (sel == 1) ? 0x8 : 0xF;
                    styles[2] = (sel == 2) ? 0x8 : 0xF;
                    DrawOptionMenuChoice(gSystemText_Normal, 120, yPos, styles[0]);
                    DrawOptionMenuChoice(gSystemText_LR, 166, yPos, styles[1]);
                    DrawOptionMenuChoice(gSystemText_LA, 188, yPos, styles[2]);
                }
                break;
            case MENUITEM_FRAMETYPE:
                gTasks[taskId].tOptFrameType = FrameType_ProcessInput(gTasks[taskId].tOptFrameType);
                if (visibleIndex < 7)
                {
                    FrameType_DrawChoicesAtRow(gTasks[taskId].tOptFrameType, row);
                }
                break;
            case MENUITEM_VERSION:
                gTasks[taskId].tOptVersion = Version_ProcessInput(gTasks[taskId].tOptVersion);
                if (visibleIndex < 7)
                {
                    u8 styles[2];
                    u8 sel = gTasks[taskId].tOptVersion;
                    styles[0] = (sel == 0) ? 0x8 : 0xF;
                    styles[1] = (sel == 1) ? 0x8 : 0xF;
                    DrawOptionMenuChoice(gSystemText_Ruby, 120, yPos, styles[0]);
                    DrawOptionMenuChoice(gSystemText_Sapphire, 162, yPos, styles[1]);
                }
                break;
            case MENUITEM_TRAINER:
                gTasks[taskId].tOptTrainer = Trainer_ProcessInput(gTasks[taskId].tOptTrainer);
                if (visibleIndex < 7)
                {
                    u8 styles[2];
                    u8 sel = gTasks[taskId].tOptTrainer;
                    styles[0] = (sel == 0) ? 0x8 : 0xF;
                    styles[1] = (sel == 1) ? 0x8 : 0xF;
                    DrawOptionMenuChoice(gSystemText_Original, 120, yPos, styles[0]);
                    DrawOptionMenuChoice(gSystemText_Emerald, 162, yPos, styles[1]);
                }
                break;
            }
        }
    }
}

static void Task_OptionMenuSave(u8 taskId)
{
    gSaveBlock2.optionsTextSpeed       = gTasks[taskId].tOptTextSpeed;
    gSaveBlock2.optionsBattleSceneOff  = gTasks[taskId].tOptBattleScene;
    gSaveBlock2.optionsBattleStyle     = gTasks[taskId].tOptBattleStyle;
    gSaveBlock2.optionsSound           = gTasks[taskId].tOptSound;
    gSaveBlock2.optionsButtonMode      = gTasks[taskId].tOptButtonMode;
    gSaveBlock2.optionsWindowFrameType = gTasks[taskId].tOptFrameType;
    gSaveBlock2.optionsVersion         = gTasks[taskId].tOptVersion;
    gSaveBlock2.optionsTrainer         = gTasks[taskId].tOptTrainer;

    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_OptionMenuFadeOut;
}

static void Task_OptionMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2(gMain.savedCallback);
    }
}

//This version uses addition '+' instead of OR '|'.
#define WIN_RANGE_(a, b) (((a) << 8) + (b))

static void RedrawOptionMenuItems(u8 taskId)
{
    u8 i;
    u8 scrollOffset = gTasks[taskId].tScrollOffset;
    u8 visibleRows[] = {5, 7, 9, 11, 13, 15, 17};
    u16 yPixels[] = {40, 56, 72, 88, 104, 120, 136};  // Y positions in pixels for each row
    
    // Clear the entire content area (inside the frame borders) to remove any ghosting
    Menu_BlankWindowRect(3, 5, 26, 18);
    
    // Draw visible items based on scroll offset
    for (i = 0; i < 7 && (scrollOffset + i) < 9; i++)
    {
        u8 itemIndex = scrollOffset + i;
        u8 row = visibleRows[i];
        u16 yPos = yPixels[i];
        
        switch (itemIndex)
        {
        case MENUITEM_TEXTSPEED:
            Menu_PrintText(gSystemText_TextSpeed, 4, row);
            {
                u8 styles[3];
                u8 sel = gTasks[taskId].tOptTextSpeed;
                styles[0] = (sel == 0) ? 0x8 : 0xF;
                styles[1] = (sel == 1) ? 0x8 : 0xF;
                styles[2] = (sel == 2) ? 0x8 : 0xF;
                DrawOptionMenuChoice(gSystemText_Slow, TEXTSPEED_SLOW_LEFT, yPos, styles[0]);
                DrawOptionMenuChoice(gSystemText_Mid, TEXTSPEED_MIX_LEFT, yPos, styles[1]);
                DrawOptionMenuChoice(gSystemText_Fast, TEXTSPEED_FAST_LEFT, yPos, styles[2]);
            }
            break;
        case MENUITEM_BATTLESCENE:
            Menu_PrintText(gSystemText_BattleScene, 4, row);
            {
                u8 styles[2];
                u8 sel = gTasks[taskId].tOptBattleScene;
                styles[0] = (sel == 0) ? 0x8 : 0xF;
                styles[1] = (sel == 1) ? 0x8 : 0xF;
                DrawOptionMenuChoice(gSystemText_On, 120, yPos, styles[0]);
                DrawOptionMenuChoice(gSystemText_Off, 190, yPos, styles[1]);
            }
            break;
        case MENUITEM_BATTLESTYLE:
            Menu_PrintText(gSystemText_BattleStyle, 4, row);
            {
                u8 styles[2];
                u8 sel = gTasks[taskId].tOptBattleStyle;
                styles[0] = (sel == 0) ? 0x8 : 0xF;
                styles[1] = (sel == 1) ? 0x8 : 0xF;
                DrawOptionMenuChoice(gSystemText_Shift, BATTLESTYLE_SHIFT, yPos, styles[0]);
                DrawOptionMenuChoice(gSystemText_Set, BATTLESTYLE_SET, yPos, styles[1]);
            }
            break;
        case MENUITEM_SOUND:
            Menu_PrintText(gSystemText_Sound, 4, row);
            {
                u8 styles[2];
                u8 sel = gTasks[taskId].tOptSound;
                styles[0] = (sel == 0) ? 0x8 : 0xF;
                styles[1] = (sel == 1) ? 0x8 : 0xF;
                DrawOptionMenuChoice(gSystemText_Mono, 120, yPos, styles[0]);
                DrawOptionMenuChoice(gSystemText_Stereo, 172, yPos, styles[1]);
            }
            break;
        case MENUITEM_BUTTONMODE:
            Menu_PrintText(gSystemText_ButtonMode, 4, row);
            {
                u8 styles[3];
                u8 sel = gTasks[taskId].tOptButtonMode;
                styles[0] = (sel == 0) ? 0x8 : 0xF;
                styles[1] = (sel == 1) ? 0x8 : 0xF;
                styles[2] = (sel == 2) ? 0x8 : 0xF;
                DrawOptionMenuChoice(gSystemText_Normal, 120, yPos, styles[0]);
                DrawOptionMenuChoice(gSystemText_LR, 166, yPos, styles[1]);
                DrawOptionMenuChoice(gSystemText_LA, 188, yPos, styles[2]);
            }
            break;
        case MENUITEM_FRAMETYPE:
            Menu_PrintText(gSystemText_Frame, 4, row);
            FrameType_DrawChoicesAtRow(gTasks[taskId].tOptFrameType, row);
            break;
        case MENUITEM_VERSION:
            Menu_PrintText(gSystemText_Version, 4, row);
            {
                u8 styles[2];
                u8 sel = gTasks[taskId].tOptVersion;
                styles[0] = (sel == 0) ? 0x8 : 0xF;
                styles[1] = (sel == 1) ? 0x8 : 0xF;
                DrawOptionMenuChoice(gSystemText_Ruby, 120, yPos, styles[0]);
                DrawOptionMenuChoice(gSystemText_Sapphire, 162, yPos, styles[1]);
            }
            break;
        case MENUITEM_TRAINER:
            Menu_PrintText(gSystemText_Trainer, 4, row);
            {
                u8 styles[2];
                u8 sel = gTasks[taskId].tOptTrainer;
                styles[0] = (sel == 0) ? 0x8 : 0xF;
                styles[1] = (sel == 1) ? 0x8 : 0xF;
                DrawOptionMenuChoice(gSystemText_Original, 120, yPos, styles[0]);
                DrawOptionMenuChoice(gSystemText_Emerald, 162, yPos, styles[1]);
            }
            break;
        case MENUITEM_CANCEL:
            Menu_PrintText(gSystemText_Cancel, 4, row);
            break;
        }
    }
}

static void HighlightOptionMenuItem(u8 taskId, u8 index)
{
    u8 scrollOffset = gTasks[taskId].tScrollOffset;
    u8 visibleIndex = index - scrollOffset;
    u8 visibleRows[] = {5, 7, 9, 11, 13, 15, 17};
    u16 yPos;
    
    REG_WIN1H = WIN_RANGE(24, 215);
    
    if (visibleIndex < 7)
    {
        // Each menu item takes 2 text rows, so highlight covers both rows
        // Row numbers are in text row units (8 pixels each)
        yPos = visibleRows[visibleIndex] * 8;  // Convert row to pixels (each row is 8 pixels tall)
        REG_WIN1V = WIN_RANGE_(yPos, yPos + 16);  // Highlight covers 2 rows (16 pixels)
    }
    else
    {
        // Hide highlight if item is not visible
        REG_WIN1V = WIN_RANGE_(0, 0);
    }
}

static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style)
{
    u8 dst[15];
    u16 i;

    for (i = 0; *text != EOS && i < 15; i++)
        dst[i] = *(text++);

    dst[2] = style;
    dst[i] = EOS;
    Menu_PrintTextPixelCoords(dst, x, y, 1);
}

static u8 TextSpeed_ProcessInput(u8 selection)
{
    if (gMain.newKeys & DPAD_RIGHT)
    {
        if (selection < 2)
            selection++;
        else
            selection = 0;
    }
    if (gMain.newKeys & DPAD_LEFT)
    {
        if (selection > 0)
            selection--;
        else
            selection = 2;
    }
    return selection;
}

static u8 BattleScene_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
        selection ^= 1;
    return selection;
}

static u8 BattleStyle_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
        selection ^= 1;
    return selection;
}

static u8 Sound_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        SetPokemonCryStereo(selection);
    }
    return selection;
}

static u8 FrameType_ProcessInput(u8 selection)
{
    if (gMain.newKeys & DPAD_RIGHT)
    {
        if (selection < 19)
            selection++;
        else
            selection = 0;
        Menu_LoadStdFrameGraphicsOverrideStyle(selection);
    }
    if (gMain.newKeys & DPAD_LEFT)
    {
        if (selection > 0)
            selection--;
        else
            selection = 19;
        Menu_LoadStdFrameGraphicsOverrideStyle(selection);
    }
    return selection;
}

static void FrameType_DrawChoicesAtRow(u8 selection, u8 row)
{
#if ENGLISH
    u8 text[6];
    u8 n = selection + 1;
    u16 i;

    for (i = 0; gSystemText_Terminator[i] != EOS && i < 6; i++)
        text[i] = gSystemText_Terminator[i];

    //Convert number to decimal string
    if (n / 10 != 0)
    {
        text[i] = n / 10 + CHAR_0;
        i++;
        text[i] = n % 10 + CHAR_0;
        i++;
    }
    else
    {
        text[i] = n % 10 + CHAR_0;
        i++;
        text[i] = CHAR_SPACE;
        i++;
    }

    text[i] = EOS;
    Menu_PrintText(gSystemText_Type, 15, row);
    Menu_PrintText(text, 18, row);
#elif GERMAN
    u8 text[16];
    u8 n = selection + 1;
    u8 *str;

    StringCopy(text, gSystemText_Type);
    str = StringAppend(text, gSystemText_Terminator);

    if (n / 10 != 0)
    {
        *str++ = n / 10 + CHAR_0;
        *str++ = n % 10 + CHAR_0;
    }
    else
    {
        *str++ = n % 10 + CHAR_0;
        *str++ = CHAR_SPACE;
    }
    *str = EOS;
    Menu_PrintText(text, 15, row);
#endif
}

static u8 ButtonMode_ProcessInput(u8 selection)
{
    if (gMain.newKeys & DPAD_RIGHT)
    {
        if (selection < 2)
            selection++;
        else
            selection = 0;
    }
    if (gMain.newKeys & DPAD_LEFT)
    {
        if (selection > 0)
            selection--;
        else
            selection = 2;
    }
    return selection;
}

static u8 Version_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
        selection ^= 1;
    return selection;
}

static u8 Trainer_ProcessInput(u8 selection)
{
    if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
        selection ^= 1;
    return selection;
}

