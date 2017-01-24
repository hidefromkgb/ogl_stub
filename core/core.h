#include <stdint.h>



#define DEF_UTMR   8 /** Default interval of the state-updating timer **/

enum {
    /** = 'no such key!' = **/
    KEY_NONE  =  0,

    /** == regular keys == **/
    KEY_ESC       ,
    KEY_F1        , KEY_F2        , KEY_F3        , KEY_F4        ,
    KEY_F5        , KEY_F6        , KEY_F7        , KEY_F8        ,
    KEY_F9        , KEY_F10       , KEY_F11       , KEY_F12       ,

    KEY_TILDE     ,
    KEY_1         , KEY_2         , KEY_3         , KEY_4         ,
    KEY_5         , KEY_6         , KEY_7         , KEY_8         ,
    KEY_9         , KEY_0         , KEY_HYPHEN    , KEY_EQUALS    ,
    KEY_BACKSPACE ,

    KEY_TAB       ,
    KEY_Q         , KEY_W         , KEY_E         , KEY_R         ,
    KEY_T         , KEY_Y         , KEY_U         , KEY_I         ,
    KEY_O         , KEY_P         , KEY_LBRACKET  , KEY_RBRACKET  ,

    KEY_CAPSLOCK  ,
    KEY_A         , KEY_S         , KEY_D         , KEY_F         ,
    KEY_G         , KEY_H         , KEY_J         , KEY_K         ,
    KEY_L         , KEY_COLON     , KEY_QUOTE     , KEY_PIPE      ,
    KEY_ENTER     ,

    KEY_LSHIFT    , KEY_Z         , KEY_X         , KEY_C         ,
    KEY_V         , KEY_B         , KEY_N         , KEY_M         ,
    KEY_LESS      , KEY_GREATER   , KEY_QUESTION  , KEY_RSHIFT    ,

    KEY_LCTRL     , KEY_LSYSTEM   , KEY_LALT      , KEY_SPACE     ,
    KEY_RALT      , KEY_RSYSTEM   , KEY_RMENU     , KEY_RCTRL     ,

    /** === side panel === **/
    KEY_PRTSCR    , KEY_SCRLOCK   , KEY_PAUSE     ,
    KEY_INSERT    , KEY_HOME      , KEY_PAGEUP    ,
    KEY_DELETE    , KEY_END       , KEY_PAGEDOWN  ,
    KEY_UP        , KEY_LEFT      , KEY_DOWN      , KEY_RIGHT     ,

    /** ===== numpad ===== **/
    KEY_NUM_LOCK  , KEY_NUM_DIV   , KEY_NUM_MUL   , KEY_NUM_SUB   ,
    KEY_NUM_7     , KEY_NUM_8     , KEY_NUM_9     , KEY_NUM_4     ,
    KEY_NUM_5     , KEY_NUM_6     , KEY_NUM_ADD   , KEY_NUM_1     ,
    KEY_NUM_2     , KEY_NUM_3     , KEY_NUM_0     , KEY_NUM_DEL   ,
    KEY_NUM_ENTER ,

    /** == mouse 'keys' == **/
    KEY_LMB       , KEY_MMB       , KEY_RMB       ,

    /** = 'total count!' = **/
    KEY_ALL_KEYS
};



typedef struct ENGC ENGC;

void cUpdateState(ENGC *engc);
void cMouseInput(ENGC *engc, long xpos, long ypos, long btns);
void cKbdInput(ENGC *engc, uint8_t code, long down);
void cResizeWindow(ENGC *engc, long xdim, long ydim);
void cRedrawWindow(ENGC *engc);
void cFreeEngine(ENGC **engc);
ENGC *cMakeEngine();
