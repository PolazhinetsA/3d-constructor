#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include <stdint.h>

enum { TOP, FRONT, SIDE, _3D };
enum { LEFT, RIGHT, DOWN, UP };
enum { X, Y, Z };
enum { A, B };

#define UNIFORM { { 1, 0, 0}, \
                  { 0, 1, 0}, \
                  { 0, 0, 1} }

#define COMBINE3(dst, op, src)  ( dst[0] op src[0], \
                                  dst[1] op src[1], \
                                  dst[2] op src[2] )

typedef double (*Edges)[2][3];

#define SZREALLOC 0x100

#define EdgesCount(edges) \
    (((uint32_t *)edges)[-1])

#define EdgesLast(edges) \
    (edges[EdgesCount(edges)-1])

#define EdgesAlloc \
    ((Edges )((uint32_t *)calloc(1, sizeof(uint32_t)) + 1))

#define EdgesRealloc(edges,sz) \
    ((Edges )((uint32_t *)realloc(&EdgesCount(edges), sz) + 1))

#define EdgesFree(edges) \
    (free(&EdgesCount(edges)))

#define EdgesAdd(edges, edge) {                                             \
    if (EdgesCount(edges) % SZREALLOC == 0) {                               \
        size_t _sz = sizeof(uint32_t) + SZREALLOC*sizeof(*(Edges )NULL);    \
        edges = EdgesRealloc(edges, _sz);                                   \
    }                                                                       \
    COMBINE3(edges[EdgesCount(edges)][A], =, edge[A]);                      \
    COMBINE3(edges[EdgesCount(edges)][B], =, edge[B]);                      \
    ++EdgesCount(edges);                                                    \
}

#define VECMAT3(dst, src)       \
{                               \
    typeof(*dst) _0, _1, _2;    \
                                \
    _0 = dst[0]*src[0][0]       \
       + dst[1]*src[0][1]       \
       + dst[2]*src[0][2];      \
                                \
    _1 = dst[0]*src[1][0]       \
       + dst[1]*src[1][1]       \
       + dst[2]*src[1][2];      \
                                \
    _2 = dst[0]*src[2][0]       \
       + dst[1]*src[2][1]       \
       + dst[2]*src[2][2];      \
                                \
    dst[0] = _0;                \
    dst[1] = _1;                \
    dst[2] = _2;                \
}

#endif
