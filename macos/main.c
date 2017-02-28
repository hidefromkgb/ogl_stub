#include "mac_load/mac_load.h"
#include "../core/ogl_load/ogl_load.h"
#include "../core/core.h"



#define VAR_ENGC "engc"

typedef struct {
    ENGC *engc;
    NSView *view;
    NSWindow *mwnd;
    uint32_t pbtn;
} DATA;



bool MAC_Handler(OnTrue) {
    return true;
}

bool MAC_Handler(OnFalse) {
    return false;
}

bool MAC_Handler(OnClose) {
    stop_(sharedApplication(NSApplication()), self);
    return true;
}

void MAC_Handler(OnSize) {
    CGRect rect;
    ENGC *engc;

    rect = frame(self);
    MAC_GetIvar(self, VAR_ENGC, &engc);
    cResizeWindow(engc, rect.size.width, rect.size.height);
}

void MAC_Handler(OnDraw, CGRect rect) {
    ENGC *engc;

    MAC_GetIvar(self, VAR_ENGC, &engc);
    cRedrawWindow(engc);
    flushBuffer(openGLContext(self));
}

void MAC_Handler(OnKeys, NSEvent *ekey) {
    static uint8_t keys[256] = { /** see the list of kVK_* for info **/
        KEY_A         , KEY_S         , KEY_D         , KEY_F         ,
        KEY_H         , KEY_G         , KEY_Z         , KEY_X         ,
        KEY_C         , KEY_V         , KEY_NONE      , KEY_B         ,
        KEY_Q         , KEY_W         , KEY_E         , KEY_R         ,
        KEY_Y         , KEY_T         , KEY_1         , KEY_2         ,
        KEY_3         , KEY_4         , KEY_6         , KEY_5         ,
        KEY_EQUALS    , KEY_9         , KEY_7         , KEY_HYPHEN    ,
        KEY_8         , KEY_0         , KEY_RBRACKET  , KEY_O         ,
        KEY_U         , KEY_LBRACKET  , KEY_I         , KEY_P         ,
        KEY_ENTER     , KEY_L         , KEY_J         , KEY_QUOTE     ,
        KEY_K         , KEY_COLON     , KEY_PIPE      , KEY_LESS      ,
        KEY_QUESTION  , KEY_N         , KEY_M         , KEY_GREATER   ,
        KEY_TAB       , KEY_SPACE     , KEY_TILDE     , KEY_BACKSPACE ,
        KEY_NONE      , KEY_ESC       , KEY_NONE      , KEY_LALT      ,
        KEY_LSHIFT    , KEY_CAPSLOCK  , KEY_LSYSTEM   , KEY_LCTRL     ,
        KEY_RSHIFT    , KEY_RSYSTEM   , KEY_RCTRL     , KEY_NONE      ,
        KEY_NONE      , KEY_NUM_DEL   , KEY_NONE      , KEY_NUM_MUL   ,
        KEY_NONE      , KEY_NUM_ADD   , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NUM_DIV   ,
        KEY_NUM_ENTER , KEY_NONE      , KEY_NUM_SUB   , KEY_NONE      ,
        KEY_NONE      , KEY_EQUALS    , KEY_NUM_0     , KEY_NUM_1     ,
        KEY_NUM_2     , KEY_NUM_3     , KEY_NUM_4     , KEY_NUM_5     ,
        KEY_NUM_6     , KEY_NUM_7     , KEY_NONE      , KEY_NUM_8     ,
        KEY_NUM_9     , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_F5        , KEY_F6        , KEY_F7        , KEY_F3        ,
        KEY_F8        , KEY_F9        , KEY_NONE      , KEY_F11       ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_F10       , KEY_NONE      , KEY_F12       ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_HOME      ,
        KEY_PAGEUP    , KEY_DELETE    , KEY_F4        , KEY_END       ,
        KEY_F2        , KEY_PAGEDOWN  , KEY_F1        , KEY_LEFT      ,
        KEY_RIGHT     , KEY_DOWN      , KEY_UP        ,
        /** only KEY_NONE`s here... **/
    };
    ENGC *engc;

    MAC_GetIvar(self, VAR_ENGC, &engc);
    cKbdInput(engc, keys[keyCode(ekey) & 0xFF], !!(type(ekey) == NSKeyDown));
}

void OnRedraw(CFRunLoopObserverRef runp, CFRunLoopActivity acti, void *user) {
    setNeedsDisplay_((NSView*)user, true);
}

void OnUpdate(CFRunLoopTimerRef tmrp, void *user) {
    DATA *data = (DATA*)user;
    uint32_t pbtn, attr, here;
    CGRect wdim, vdim;
    CGPoint mptr;

    if (isKeyWindow(data->mwnd)) {
        wdim = frame(data->mwnd);
        vdim = frame(data->view);
        mptr = mouseLocation(NSEvent());
        pbtn = pressedMouseButtons(NSEvent());
        mptr.x -= wdim.origin.x + vdim.origin.x;
        mptr.y -= wdim.origin.y + vdim.origin.y;
        mptr.y = vdim.size.height - mptr.y;

        attr = (((data->pbtn ^ pbtn) & 3)? 0 : 1) |
                ((pbtn & 1)? 2 : 0) | ((pbtn & 2)? 8 : 0);
        data->pbtn = (data->pbtn & -0x100) | (pbtn & 0xFF);

        here = !!((mptr.x < vdim.size.width) && (mptr.y < vdim.size.height)
               && (mptr.x >= 0) && (mptr.y >= 0));
        if ((~attr & 1) && (here || ((data->pbtn >> 8) & ~attr & 14)))
            data->pbtn = (data->pbtn & 0xFF) | ((attr & 14) << 8);
        if ((data->pbtn & (14 << 8)))
            cMouseInput(data->engc, mptr.x, mptr.y, attr);
    }
    cUpdateState(data->engc);
}



int main(int argc, char *argv[]) {
    GLint attr[] = {NSOpenGLPFADoubleBuffer, NSOpenGLPFADepthSize, 32, 0};
    CFRunLoopTimerRef tupd;
    CFRunLoopObserverRef idrw;
    NSOpenGLPixelFormat *pfmt;
    NSAutoreleasePool *pool;
    NSApplication *thrd;
    CGSize size;
    CGRect dims;
    Class vogl;

    DATA data = {};

    pool = init(alloc(NSAutoreleasePool()));
    thrd = sharedApplication(NSApplication());
    setActivationPolicy_(thrd, NSApplicationActivationPolicyAccessory);

    size = (CGSize){800, 600};
    dims = visibleFrame(mainScreen(NSScreen()));
    dims = (CGRect){{dims.origin.x + (dims.size.width - size.width) * 0.5,
                     dims.origin.y + (dims.size.height - size.height) * 0.5},
                     size};

    vogl = MAC_MakeClass("NSO", NSOpenGLView(), MAC_TempArray(VAR_ENGC),
                         MAC_TempArray(drawRect_(), OnDraw,
                                       windowDidResize_(), OnSize,
                                       windowShouldClose_(), OnClose,
                                       keyDown_(), OnKeys, keyUp_(), OnKeys,
                                       acceptsFirstResponder(), OnTrue));

    pfmt = initWithAttributes_(alloc(NSOpenGLPixelFormat()), attr);
    data.view = (NSView*)initWithFrame_pixelFormat_(alloc(vogl), dims, pfmt);
    makeCurrentContext(openGLContext(data.view));
    release(pfmt);

    MAC_SetIvar(data.view, VAR_ENGC, data.engc = cMakeEngine());
    data.mwnd = initWithContentRect_styleMask_backing_defer_
                    (alloc(NSWindow()), dims, NSTitledWindowMask
                                            | NSClosableWindowMask
                                            | NSResizableWindowMask
                                            | NSMiniaturizableWindowMask,
                     kCGBackingStoreBuffered, false);
    setContentView_(data.mwnd, data.view);
    setDelegate_(data.mwnd, data.view);

    activateIgnoringOtherApps_(thrd, true);
    makeKeyWindow(data.mwnd);
    orderFront_(data.mwnd, thrd);

    OnSize(data.view, 0);
    tupd = MAC_MakeTimer(DEF_UTMR, OnUpdate, &data);
    idrw = MAC_MakeIdleFunc(OnRedraw, data.view);
    run(thrd);
    MAC_FreeTimer(tupd);
    MAC_FreeIdleFunc(idrw);
    cFreeEngine(&data.engc);

    release(pool);
    MAC_FreeClass(vogl);
    return 0;
}
