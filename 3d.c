#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include "defs.h"

FILE *scrdst[NSCR];
char *scrbuf[NSCR];
int wid, hei, widhei;

void tmode_switch();
void mainloop();

int main(int argc, char **argv)
{
    if (argc >= 3) {
        wid = atoi(argv[1]);
        hei = atoi(argv[2]);
    } else {
        wid = 80;
        hei = 40;
    }
    widhei = wid*hei;

    scrdst[PRIM] = stdout;
    scrbuf[PRIM] = malloc(widhei);

    if (argc >= 4) {
        scrdst[SECOND] = fopen(argv[3], "w");
        scrbuf[SECOND] = malloc(widhei);
    }

    tmode_switch();
    mainloop();
    tmode_switch();

    return 0;
}

const double move[][4][3] = { { {-1, 0, 0},
                                { 1, 0, 0},
                                { 0,-1, 0},
                                { 0, 1, 0} },

                              { {-1, 0, 0},
                                { 1, 0, 0},
                                { 0, 0, 1},
                                { 0, 0,-1} },

                              { { 0,-1, 0},
                                { 0, 1, 0},
                                { 0, 0, 1},
                                { 0, 0,-1} } };


double center[3] = {0.0, 0.0, 0.0},
       far = 100.0,
       zang = 0.0,
       xang = 0.0;

const char *strview[] = { "Top", "Front", "Side", "3d" };

int view = TOP;

Edges edges;

int adding = 2;

void render(int);
void redraw(int);
void erase(int);

void save();
void load();

void mainloop()
{
    erase(PRIM);
    redraw(PRIM);
    erase(SECOND);
    redraw(SECOND);

    edges = EdgesAlloc;

    double last[3] = {};

    for (int key; key = getchar(), key != 'Q' && key != 'q'; )
    {
        switch (key)
        {
        case 'V': case 'v': view = (view + 1) % NVIEW; break;

        case '-': case '_': far += 10.0; break;
        case '=': case '+': far -= 10.0; break;

        case 'W': case 'w': if (adding == 2) save(); break;
        case 'R': case 'r': if (adding == 2) load(); break;

        default: if (view == _3D)
        {
            switch (key)
            {
            case 'S': case 's': zang -= 0.1; break;
            case 'F': case 'f': zang += 0.1; break;
            case 'D': case 'd': xang -= 0.1; break;
            case 'E': case 'e': xang += 0.1; break;
            }
        }
        else
        {
            switch (key)
            {
            case 'S': case 's': COMBINE3(center, -=, move[view][LF]); break;
            case 'F': case 'f': COMBINE3(center, -=, move[view][RI]); break;
            case 'D': case 'd': COMBINE3(center, -=, move[view][DN]); break;
            case 'E': case 'e': COMBINE3(center, -=, move[view][UP]); break;

            case 'J': case 'j': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, move[view][LF]); break;
            case 'L': case 'l': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, move[view][RI]); break;
            case 'K': case 'k': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, move[view][DN]); break;
            case 'I': case 'i': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, move[view][UP]); break;

            case ' ':
                adding = (adding + 1) % 3;
                switch (adding) {
                case 0:
                    {
                    double newedge[2][3];
                    COMBINE3(newedge[A], =, last);
                    COMBINE3(newedge[B], =, last);
                    EdgesAdd(edges, newedge);
                    }
                    break;
                case 1:
                    COMBINE3(EdgesLast(edges)[B], =, EdgesLast(edges)[A]);
                    break;
                case 2:
                    COMBINE3(last, =, EdgesLast(edges)[B]);
                    break;
                }
                break;
            }
        }
        }
        render(PRIM);
        redraw(PRIM);
        if (view == _3D) {
            render(SECOND);
            redraw(SECOND);
        }
    }
}

double transform[3][3][3] = { { { 1, 0, 0},
                                { 0, 1, 0},
                                { 0, 0, 0} },

                              { { 1, 0, 0},
                                { 0, 0,-1},
                                { 0, 0, 0} },

                              { { 0, 1, 0},
                                { 0, 0,-1},
                                { 0, 0, 0} } };

double uniform[3][3] = UNIFORM;

void render(int scrno)
{
    if (!scrdst[scrno]) return;

    double rot1[3][3] = UNIFORM,
           rot2[3][3] = UNIFORM,
           (*mat1)[3],
           (*mat2)[3];

    if (view == _3D)
    {
        double zsin = sin(zang + 0.1*scrno),
               zcos = cos(zang + 0.1*scrno),
               xsin = sin(xang),
               xcos = cos(xang);

        rot1[X][X] =  zcos;
        rot1[X][Y] = -zsin;
        rot1[Y][X] =  zsin;
        rot1[Y][Y] =  zcos;

        rot2[Y][Y] =  xcos;
        rot2[Y][Z] = -xsin;
        rot2[Z][Y] =  xsin;
        rot2[Z][Z] =  xcos;

        mat1 = rot1;
        mat2 = rot2;
    }
    else
    {
        mat1 = transform[view];
        mat2 = uniform;
    }

    erase(scrno);

    char (*scr)[wid] = (char (*)[wid])scrbuf[scrno];

    for (int i = 0; i < EdgesCount(edges); ++i)
    {
        double edge[2][3];

        for (int j = 0; j < 2; ++j)
        {
            COMBINE3(edge[j], =, edges[i][j]);
            COMBINE3(edge[j], -=, center);

            VECMAT3(edge[j], mat1);
            VECMAT3(edge[j], mat2);

            edge[j][Z] += far;
            if (edge[j][Z] < 10.0) continue;

            edge[j][X] = edge[j][X]*wid/edge[j][Z];
            edge[j][Y] = edge[j][Y]*wid/edge[j][Z];
        }

        if (adding != 2 && &edges[i] == &EdgesLast(edges))
        {
            for (int j = 0; j <= adding; ++j)
            {
                if (edge[j][Z] < 10.0) continue;

                int col = wid/2 + edge[j][X]*2,
                    row = hei/2 - edge[j][Y];

                if (0 <= col && col < wid-1 && 0 <= row && row < hei)
                    scr[row][col] = 'A'+j;
            }
        }
        else if (edge[A][Z] >= 10.0 && edge[B][Z] >= 10.0)
        {
            double len = fmax(fabs(edge[A][X]-edge[B][X])*2,
                              fabs(edge[A][Y]-edge[B][Y]));

            for (double l = 0; l <= len; l += 1.0)
            {
                int col, row;
                col = wid/2 + (edge[A][X] + (edge[B][X]-edge[A][X])*l/len)*2,
                row = hei/2 - (edge[A][Y] + (edge[B][Y]-edge[A][Y])*l/len);

                if (0 <= col && col < wid-1 && 0 <= row && row < hei)
                    scr[row][col] = '#';
            }
        }
    }
}

void erase(int scrno)
{
    if (!scrdst[scrno]) return;

    memset(scrbuf[scrno], ' ', widhei);
    for (int y = 1; y <= hei; ++y) scrbuf[scrno][y*wid-1] = '\n';
}

void redraw(int scrno)
{
    if (!scrdst[scrno]) return;

    fwrite(scrbuf[scrno], 1, widhei, scrdst[scrno]);
    fprintf(scrdst[scrno], "%-5s \n\e[%dA", strview[view], hei+1);
    fflush(scrdst[scrno]);
}

void save()
{
    FILE *file;
    char path[0x100];

    tmode_switch();
    printf("filename: ");
    scanf("%s", path);
    tmode_switch();

    file = fopen(path, "w");
    fwrite(&EdgesCount(edges), sizeof(uint32_t), 1, file);
    fwrite(edges, sizeof(*edges), EdgesCount(edges), file);
    fclose(file);
}

void load()
{
    FILE *file;
    char path[0x100];
    uint32_t count;

    tmode_switch();
    printf("filename: ");
    scanf("%s", path);
    tmode_switch();

    file = fopen(path, "r");
    fread(&count, sizeof(uint32_t), 1, file);
    EdgesFree(edges);
    edges = EdgesAlloc;
    edges = EdgesRealloc(edges, (count+SZREALLOC-1)/SZREALLOC*SZREALLOC
                                * sizeof(*edges));
    fread(edges, sizeof(*edges), count, file);
    EdgesCount(edges) = count;
    fclose(file);
}

void tmode_switch()
{
    static int current = 0;

    static struct termios tmode_old, tmode_new;

    if (!current) {
        tcgetattr(STDIN_FILENO, &tmode_old);
        tmode_new = tmode_old;
        tmode_new.c_lflag &= ~(ICANON);
        tmode_new.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &tmode_new);
        current = 1;
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &tmode_old);
        current = 0;
    }
}
