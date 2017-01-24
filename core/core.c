#include "ogl_load/ogl_load.h"
#include "vec_math/vec_math.h"
#include "core.h"



#define DEF_FANG  0.01  /** Default angular step             **/
#define DEF_FTRN  0.05  /** Default translational step       **/

#define DEF_FFOV 45.0   /** Default perspective view field   **/
#define DEF_ZNEA  0.1   /** Default near clipping plane      **/
#define DEF_ZFAR 90.0   /** Default far clipping plane       **/



struct ENGC {
    VEC_FMST *view, *proj;

    OGL_FVBO *cube;

    VEC_T2IV angp;
    VEC_T2FV fang;
    VEC_T3FV ftrn;

    GLboolean keys[KEY_ALL_KEYS];
};



char *shdr[] = {
/** === main vertex shader **/
"uniform mat4 mMVP;"
"attribute vec3 vert;"
"varying vec3 vpos;"
"void main() {"
    "vpos = vert;"
    "gl_Position = mMVP * vec4(vert, 1.0);"
"}",
/** === main pixel shader **/
"uniform sampler2D tile;"
"varying vec3 vpos;"
"void main() {"
    "vec3 ctex;"
    "if (abs(vpos.x) > 0.999)"
        "ctex = vpos.yzx;"
    "else if (abs(vpos.y) > 0.999)"
        "ctex = vpos.zxy;"
    "else if (abs(vpos.z) > 0.999)"
        "ctex = vpos.xyz;"
    "gl_FragColor = texture2D(tile, ctex.xy * ctex.z * 0.5) / length(ctex);"
"}"};



OGL_FTEX *MakeTileTex(GLuint size, GLuint tile, GLuint tbdr) {
    uint32_t tclr, *bptr;
    GLint x, y, u, v;
    OGL_FTEX *retn;

    size = pow(2.0, size);
    tile = pow(2.0, tile);
    bptr = calloc(size * size, sizeof(*bptr));
    for (y = size / tile; y > 0; y--)
        for (x = size / tile; x > 0; x--) {
            tclr = ((rand() % 32) + 192) * 0x010101;

            for (v = (y - 1) * tile; v < y * tile; v++)
                for (u = x * tile - tbdr; u < x * tile; u++)
                    bptr[size * v + u] = tclr - 0x505050;
            for (v = y * tile - tbdr; v < y * tile; v++)
                for (u = (x - 1) * tile; u < x * tile - tbdr; u++)
                    bptr[size * v + u] = tclr - 0x505050;

            for (v = (y - 1) * tile + tbdr; v < y * tile - tbdr; v++)
                for (u = (x - 1) * tile; u < (x - 1) * tile + tbdr; u++)
                    bptr[size * v + u] = tclr | 0x202020;
            for (v = (y - 1) * tile; v < (y - 1) * tile + tbdr; v++)
                for (u = (x - 1) * tile; u < x * tile - tbdr; u++)
                    bptr[size * v + u] = tclr | 0x202020;

            for (v = (y - 1) * tile + tbdr; v < y * tile - tbdr; v++)
                for (u = (x - 1) * tile + tbdr; u < x * tile - tbdr; u++)
                    bptr[size * v + u] = tclr;
        }
    retn = OGL_MakeTex(size, size, 0, GL_TEXTURE_2D,
                       GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR,
                       GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, bptr);
    free(bptr);
    return retn;
}



void cUpdateState(ENGC *engc) {
    VEC_T3FV vadd;
    VEC_T2FV fang;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (engc->keys[KEY_W] ^ engc->keys[KEY_S]) {
        fang = (VEC_T2FV){{engc->fang.x + 0.5 * M_PI, engc->fang.y}};
        VEC_V3FromAng(&vadd, &fang);
        VEC_V3MulC(&vadd, (engc->keys[KEY_W])? DEF_FTRN : -DEF_FTRN);
        VEC_V3AddV(&engc->ftrn, &vadd);
    }
    if (engc->keys[KEY_A] ^ engc->keys[KEY_D]) {
        fang = (VEC_T2FV){{engc->fang.x, 0.0}};
        VEC_V3FromAng(&vadd, &fang);
        VEC_V3MulC(&vadd, (engc->keys[KEY_A])? DEF_FTRN : -DEF_FTRN);
        VEC_V3AddV(&engc->ftrn, &vadd);
    }
}



void cMouseInput(ENGC *engc, long xpos, long ypos, long btns) {
    if (~btns & 2)
        return;
    if (btns & 1) { /** 1 for moving state, 2 for LMB **/
        engc->fang.y += DEF_FANG * (GLfloat)(ypos - engc->angp.y);
        if (engc->fang.y < -M_PI) engc->fang.y += 2.0 * M_PI;
        else if (engc->fang.y > M_PI) engc->fang.y -= 2.0 * M_PI;

        engc->fang.x += DEF_FANG * (GLfloat)(xpos - engc->angp.x);
        if (engc->fang.x < -M_PI) engc->fang.x += 2.0 * M_PI;
        else if (engc->fang.x > M_PI) engc->fang.x -= 2.0 * M_PI;
    }
    engc->angp = (VEC_T2IV){{xpos, ypos}};
}



void cKbdInput(ENGC *engc, uint8_t code, long down) {
    engc->keys[code] = down;
}



void cResizeWindow(ENGC *engc, long xdim, long ydim) {
    GLfloat maty = DEF_ZNEA * tanf(0.5 * DEF_FFOV * VEC_DTOR),
            matx = maty * (GLfloat)xdim / (GLfloat)ydim;

    VEC_PurgeMatrixStack(&engc->proj);
    VEC_PushMatrix(&engc->proj);
    VEC_M4Frustum(engc->proj->curr,
                 -matx, matx, -maty, maty, DEF_ZNEA, DEF_ZFAR);
    VEC_PushMatrix(&engc->proj);

    VEC_PurgeMatrixStack(&engc->view);
    VEC_PushMatrix(&engc->view);
    VEC_PushMatrix(&engc->view);

    VEC_M4Multiply(engc->proj->prev->curr,
                   engc->view->prev->curr, engc->proj->curr);

    glViewport(0, 0, xdim, ydim);
}



void cRedrawWindow(ENGC *engc) {
    VEC_TMFV rmtx, tmtx, mmtx;

    if (!engc->proj)
        return;

    VEC_M4Translate(tmtx, engc->ftrn.x, engc->ftrn.y, engc->ftrn.z);
    VEC_M4RotOrts(rmtx, engc->fang.y, engc->fang.x, 0.0);
    VEC_M4Multiply(rmtx, tmtx, mmtx);
    VEC_M4Multiply(engc->proj->curr, mmtx, engc->view->curr);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OGL_DrawVBO(engc->cube, 0, 0);
}



GLenum MakeCube(OGL_UNIF *pind, OGL_UNIF *pver, GLfloat pdim) {
    GLfloat hdim = pdim * 0.5;
    VEC_T3FV vert[] = {
        {{-hdim,-hdim,-hdim}}, {{-hdim,-hdim, hdim}},
        {{-hdim, hdim,-hdim}}, {{-hdim, hdim, hdim}},
        {{ hdim,-hdim,-hdim}}, {{ hdim,-hdim, hdim}},
        {{ hdim, hdim,-hdim}}, {{ hdim, hdim, hdim}},
    };
    GLuint indx[] = {
        0, 1, 2,  1, 3, 2,     3, 1, 7,  1, 5, 7,     5, 1, 4,  1, 0, 4,
        7, 5, 6,  4, 6, 5,     4, 0, 6,  2, 6, 0,     2, 3, 6,  7, 6, 3,
    };
    pind->type = 0;
    pind->pdat = calloc(1, pind->cdat = sizeof(indx));
    memcpy(pind->pdat, indx, pind->cdat);
    pver->type = OGL_UNI_T3FV;
    pver->pdat = calloc(1, pver->cdat = sizeof(vert));
    memcpy(pver->pdat, vert, pver->cdat);
    return GL_TRIANGLES;
}



ENGC *cMakeEngine() {
    ENGC *retn;

    retn = calloc(1, sizeof(*retn));

    retn->ftrn.x =  3.4;
    retn->ftrn.y = -3.8;
    retn->ftrn.z = -6.0;

    retn->fang.x = 30.00 * VEC_DTOR;
    retn->fang.y = 30.00 * VEC_DTOR;

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    OGL_UNIF attr[] =
        {{/** indices **/ .draw = GL_STATIC_DRAW},
         {.name = "vert", .draw = GL_STATIC_DRAW}};

    OGL_UNIF puni[] =
        {{.name = "mMVP", .type = OGL_UNI_TMFV, .pdat = &retn->view},
         {.name = "tile", .type = OGL_UNI_T1II, .pdat = (GLvoid*)0}};

    retn->cube = OGL_MakeVBO(1, MakeCube(&attr[0], &attr[1], 2.0),
                             sizeof(attr) / sizeof(*attr), attr,
                             sizeof(puni) / sizeof(*puni), puni,
                             sizeof(shdr) / sizeof(*shdr), shdr);
    free(attr[0].pdat);
    free(attr[1].pdat);
    *OGL_BindTex(retn->cube, 0, OGL_TEX_NSET) = MakeTileTex(9, 5, 1);
    return retn;
}



void cFreeEngine(ENGC **engc) {
    OGL_FreeVBO(&(*engc)->cube);

    VEC_PurgeMatrixStack(&(*engc)->proj);
    VEC_PurgeMatrixStack(&(*engc)->view);

    free(*engc);
    *engc = 0;
}
