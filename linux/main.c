#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include "../core/core.h"

typedef struct {
    GtkWidget *gwnd;
    ENGC *engc;
} DATA;



static inline GdkGLDrawable *gtk_widget_gl_begin(GtkWidget *gwnd) {
    GdkGLDrawable *pGLD = gtk_widget_get_gl_drawable(gwnd);

    if (!gdk_gl_drawable_gl_begin(pGLD, gtk_widget_get_gl_context(gwnd)))
        return 0;
    return pGLD;
}



gboolean DrawFunc(gpointer user) {
    gdk_window_invalidate_rect(GDK_WINDOW(user), 0, FALSE);
    gdk_window_process_updates(GDK_WINDOW(user), FALSE);
    return TRUE;
}



gboolean UpdateFunc(gpointer user) {
    DATA *data = (DATA*)user;
    GdkGLDrawable *pGLD;

    pGLD = gtk_widget_gl_begin(data->gwnd);
    cUpdateState(data->engc);
    gdk_gl_drawable_gl_end(pGLD);
    return TRUE;
}



gboolean OnDestroy(GtkWidget *gwnd, gpointer user) {
    gtk_main_quit();
    return TRUE;
}



gboolean OnMouseMove(GtkWidget *gwnd, GdkEventMotion *emov, gpointer user) {
    cMouseInput(*(ENGC**)user, emov->x, emov->y,
               ((emov->state & GDK_BUTTON1_MASK)? 2 : 0) |
               ((emov->state & GDK_BUTTON2_MASK)? 4 : 0) |
               ((emov->state & GDK_BUTTON3_MASK)? 8 : 0) | 1);
    return TRUE;
}



gboolean OnMousePress(GtkWidget *gwnd, GdkEventButton *ebtn, gpointer user) {
    long down = (ebtn->type == GDK_BUTTON_PRESS)? 1 : 0;

    cMouseInput(*(ENGC**)user, ebtn->x, ebtn->y,
               ((ebtn->button == 1)? down << 1 : 0) |
               ((ebtn->button == 2)? down << 2 : 0) |
               ((ebtn->button == 3)? down << 3 : 0));
    return TRUE;
}



gboolean OnKeyPress(GtkWidget *gwnd, GdkEventKey *ekey, gpointer user) {
    static uint8_t keys[256] = {
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_ESC       , KEY_1         , KEY_2         ,
        KEY_3         , KEY_4         , KEY_5         , KEY_6         ,
        KEY_7         , KEY_8         , KEY_9         , KEY_0         ,
        KEY_HYPHEN    , KEY_EQUALS    , KEY_BACKSPACE , KEY_TAB       ,
        KEY_Q         , KEY_W         , KEY_E         , KEY_R         ,
        KEY_T         , KEY_Y         , KEY_U         , KEY_I         ,
        KEY_O         , KEY_P         , KEY_LBRACKET  , KEY_RBRACKET  ,
        KEY_ENTER     , KEY_LCTRL     , KEY_A         , KEY_S         ,
        KEY_D         , KEY_F         , KEY_G         , KEY_H         ,
        KEY_J         , KEY_K         , KEY_L         , KEY_COLON     ,
        KEY_QUOTE     , KEY_TILDE     , KEY_LSHIFT    , KEY_PIPE      ,
        KEY_Z         , KEY_X         , KEY_C         , KEY_V         ,
        KEY_B         , KEY_N         , KEY_M         , KEY_LESS      ,
        KEY_GREATER   , KEY_QUESTION  , KEY_RSHIFT    , KEY_NUM_MUL   ,
        KEY_LALT      , KEY_SPACE     , KEY_CAPSLOCK  , KEY_F1        ,
        KEY_F2        , KEY_F3        , KEY_F4        , KEY_F5        ,
        KEY_F6        , KEY_F7        , KEY_F8        , KEY_F9        ,
        KEY_F10       , KEY_NUM_LOCK  , KEY_SCRLOCK   , KEY_NUM_7     ,
        KEY_NUM_8     , KEY_NUM_9     , KEY_NUM_SUB   , KEY_NUM_4     ,
        KEY_NUM_5     , KEY_NUM_6     , KEY_NUM_ADD   , KEY_NUM_1     ,
        KEY_NUM_2     , KEY_NUM_3     , KEY_NUM_0     , KEY_NUM_DEL   ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_F11       ,
        KEY_F12       , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NUM_ENTER , KEY_RCTRL     , KEY_NUM_DIV   , KEY_PRTSCR    ,
        KEY_RALT      , KEY_NONE      , KEY_HOME      , KEY_UP        ,
        KEY_PAGEUP    , KEY_LEFT      , KEY_RIGHT     , KEY_END       ,
        KEY_DOWN      , KEY_PAGEDOWN  , KEY_INSERT    , KEY_DELETE    ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_PAUSE     ,
        KEY_NONE      , KEY_NONE      , KEY_NONE      , KEY_NONE      ,
        KEY_NONE      , KEY_LSYSTEM   , /** only KEY_NONE`s here... **/
    };
    cKbdInput(*(ENGC**)user, keys[ekey->hardware_keycode & 0xFF],
             (ekey->type == GDK_KEY_PRESS)? TRUE : FALSE);
    return TRUE;
}



gboolean OnChange(GtkWidget *gwnd, GdkScreen *scrn, gpointer user) {
    GdkColormap *cmap;

    cmap = gdk_screen_get_rgba_colormap(gtk_widget_get_screen(gwnd));
    gtk_widget_set_colormap(gwnd, cmap);
    return TRUE;
}



gboolean OnResize(GtkWidget *gwnd, GdkEventConfigure *ecnf, gpointer user) {
    GdkGLDrawable *pGLD;

    pGLD = gtk_widget_gl_begin(gwnd);
    cResizeWindow(*(ENGC**)user, ecnf->width, ecnf->height);
    gdk_gl_drawable_gl_end(pGLD);
    return FALSE;
}



gboolean OnRedraw(GtkWidget *gwnd, GdkEventExpose *eexp, gpointer user) {
    GdkGLDrawable *pGLD;

    pGLD = gtk_widget_gl_begin(gwnd);
    cRedrawWindow(*(ENGC**)user);
    gdk_gl_drawable_swap_buffers(pGLD);
    gdk_gl_drawable_gl_end(pGLD);
    return TRUE;
}



int main(int argc, char *argv[]) {
    GdkGLDrawable *pGLD;
    guint tmru, tmrd;
    DATA data = {};

    gtk_init(0, 0);
    gtk_gl_init(0, 0);
    data.gwnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    OnChange(data.gwnd, 0, 0);

    g_signal_connect(G_OBJECT(data.gwnd), "expose-event",
                     G_CALLBACK(OnRedraw), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "delete-event",
                     G_CALLBACK(OnDestroy), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "screen-changed",
                     G_CALLBACK(OnChange), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "configure-event",
                     G_CALLBACK(OnResize), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "key-press-event",
                     G_CALLBACK(OnKeyPress), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "key-release-event",
                     G_CALLBACK(OnKeyPress), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "button-press-event",
                     G_CALLBACK(OnMousePress), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "button-release-event",
                     G_CALLBACK(OnMousePress), &data.engc);
    g_signal_connect(G_OBJECT(data.gwnd), "motion-notify-event",
                     G_CALLBACK(OnMouseMove), &data.engc);
    gtk_widget_set_events(data.gwnd, gtk_widget_get_events(data.gwnd)
                                   | GDK_POINTER_MOTION_MASK
                                   | GDK_BUTTON_RELEASE_MASK
                                   | GDK_BUTTON_PRESS_MASK
                                   | GDK_KEY_RELEASE_MASK
                                   | GDK_KEY_PRESS_MASK);
    gtk_widget_set_gl_capability(data.gwnd,
                                 gdk_gl_config_new_by_mode(GDK_GL_MODE_DEPTH
                                                         | GDK_GL_MODE_DOUBLE
                                                         | GDK_GL_MODE_ALPHA
                                                         | GDK_GL_MODE_RGBA),
                                 0, TRUE, GDK_GL_RGBA_TYPE);
    gtk_widget_realize(data.gwnd);

    pGLD = gtk_widget_gl_begin(data.gwnd);
    data.engc = cMakeEngine();
    gdk_gl_drawable_gl_end(pGLD);

    gtk_widget_set_app_paintable(data.gwnd, TRUE);
    gtk_widget_set_size_request(data.gwnd, 800, 600);
    gtk_window_set_position(GTK_WINDOW(data.gwnd), GTK_WIN_POS_CENTER);
    gtk_widget_show(data.gwnd);

    tmru = g_timeout_add(DEF_UTMR, UpdateFunc, &data);
    tmrd = g_idle_add(DrawFunc, gtk_widget_get_window(data.gwnd));

    gtk_main();

    pGLD = gtk_widget_gl_begin(data.gwnd);
    cFreeEngine(&data.engc);
    gdk_gl_drawable_gl_end(pGLD);

    g_source_remove(tmrd);
    g_source_remove(tmru);
    gtk_widget_destroy(data.gwnd);
}
