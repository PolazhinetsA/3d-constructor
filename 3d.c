#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include "defs.h"

char *screen;
int wid, hei, widhei;

void tmode_switch();

void mainloop();

int main(int argc, char **argv)
{
    if (argc == 3) {
        wid = atoi(argv[1]);
        hei = atoi(argv[2]);
    } else {
        wid = 80;
        hei = 40;
    }

    widhei = wid*hei;
    screen = malloc(widhei);

    tmode_switch();
    mainloop();
    tmode_switch();

    return 0;
}

const double shift[4][4][3] = { { {-1, 0, 0},
                                  { 1, 0, 0},
                                  { 0,-1, 0},
                                  { 0, 1, 0} },

                                { {-1, 0, 0},
                                  { 1, 0, 0},
                                  { 0, 0,-1},
                                  { 0, 0, 1} },

                                { { 0,-1, 0},
                                  { 0, 1, 0},
                                  { 0, 0,-1},
                                  { 0, 0, 1} } };


double center[3] = {0.0, 0.0, 0.0},
       far = 100.0,
       zang = 0.0,
       xang = 0.0;

const char *strview[] = { "Top", "Front", "Side", "3d" };

int view = TOP;

Edges edges;

int adding = 2;

void render();
void redraw();
void erase();

void mainloop()
{
    erase();
    redraw();

    edges = EdgesAlloc;

    double last[3] = {};

    for (int key; key = getchar(), key != 'Q' && key != 'q'; )
    {
        if (view == _3D)
        {
            switch (key) {
            case 'V': case 'v':
                view = (view + 1) % 4;
                break;
            case 'S': case 's':
                zang -= 0.1;
                break;
            case 'F': case 'f':
                zang += 0.1;
                break;
            case 'D': case 'd':
                xang -= 0.1;
                break;
            case 'E': case 'e':
                xang += 0.1;
                break;
            case '-': case '_':
                far += 10.0;
                break;
            case '=': case '+':
                far -= 10.0;
                break;
            }
        }
        else
        {
            switch (key) {
            case 'V': case 'v':
                view = (view + 1) % 4;
                break;
            case 'S': case 's':
                COMBINE3(center, -=, shift[view][LEFT]);
                break;
            case 'F': case 'f':
                COMBINE3(center, -=, shift[view][RIGHT]);
                break;
            case 'D': case 'd':
                COMBINE3(center, -=, shift[view][DOWN]);
                break;
            case 'E': case 'e':
                COMBINE3(center, -=, shift[view][UP]);
                break;
            case 'J': case 'j': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, shift[view][LEFT]);
                break;
            case 'L': case 'l': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, shift[view][RIGHT]);
                break;
            case 'K': case 'k': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, shift[view][DOWN]);
                break;
            case 'I': case 'i': if (adding != 2)
                COMBINE3(EdgesLast(edges)[adding], +=, shift[view][UP]);
                break;
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
            case '-': case '_':
                far += 10.0;
                break;
            case '=': case '+':
                far -= 10.0;
                break;
            }
        }
        render();
        redraw();
    }
}

double transform[3][3][3] = { { { 1, 0, 0},
                                { 0, 1, 0},
                                { 0, 0, 0} },

                              { { 1, 0, 0},
                                { 0, 0, 1},
                                { 0, 0, 0} },

                              { { 0, 1, 0},
                                { 0, 0, 1},
                                { 0, 0, 0} } };

double uniform[3][3] = UNIFORM;

void render()
{
    double rot1[3][3] = UNIFORM,
           rot2[3][3] = UNIFORM,
           (*mat1)[3],
           (*mat2)[3];

    if (view == _3D)
    {
        double zsin = sin(zang),
               zcos = cos(zang),
               xsin = sin(xang),
               xcos = cos(xang);

        rot1[X][X] =  zcos;
        rot1[X][Y] =  zsin;
        rot1[Y][X] = -zsin;
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

    erase();

    char (*scr)[wid] = (char (*)[wid])screen;

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

            edge[j][X] = edge[j][X]*100.0/edge[j][Z];
            edge[j][Y] = edge[j][Y]*100.0/edge[j][Z];
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

void redraw()
{
    fwrite(screen, 1, widhei, stdout);
    printf("%-5s \n\e[%dA", strview[view], hei+1);
}

void erase()
{
    memset(screen, ' ', widhei);
    for (int y = 1; y <= hei; ++y) screen[y*wid-1] = '\n';
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
