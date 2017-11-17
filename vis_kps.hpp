#include<bits/stdc++.h>
#include "CImg.h"

using namespace std; // bad practice

template<typename T>
struct vec2_t {
    T x,y;
};

template<typename T>
struct vec3_t {
    T x,y,z;
};

struct edge_t {
    int u, v;
    float w;
};

typedef cil::CImg<uint8_t> img_t;
typedef vec2_t<int>       int2_t;
typedef vec2_t<float>   float2_t;
typedef vec2_t<double> double2_t;
typedef vec3_t<int>       int3_t;
typedef vec3_t<float>   float3_t;
typedef vec3_t<double> double3_t;
typedef vec3_t<uint8_t> color_t;

// TODO: make consistent - replace r,c with x,y
inline int ijtoi(int r, int c, int width) {
    return r*width + c;
}

inline int2_t itoij(int i, int width) {
    int y = i / width;
    int x = i - (y*width);
    return {x, y};
}

inline float pix_diff(int3_t p1, int3_t p2) {
    float xd = float(p1.x-p2.x);
    float yd = float(p1.y-p2.y);
    float zd = float(p1.z-p2.z);

    return (xd*xd) + (yd*yd) + (zd*zd);
}

inline edge_t gen_edge(img_t& im, int r1, int c1, int r2, int c2, int width) {
    // TODO: confirm c,r order
    int3_t p1 = {
        im(c1, r1, 0, 0),
        im(c1, r1, 0, 1),
        im(c1, r1, 0, 2)};
    int3_t p2 = {
        im(c2, r2, 0, 0),
        im(c2, r2, 0, 1),
        im(c2, r2, 0, 2)};

    return {ijtoi(r1,c1,width), ijtoi(r2,c2,width), pix_diff(p1, p2)};
}

void extract_points(const char* path, vector<float2_t>& out) {
    ifstream f(path);
    int n;
    f >> n;
    out.reserve(n);

    float x,y;
    for (int i = 0; i < n; ++i) {
        f >> x >> y;
        out.push_back({x, y});
    }
}

void build_graph(img_t& im, vector<edge_t>& out) {
    int h = im.height();
    int w = im.width();

    for (int r = 1; r < h-1; ++r) {
        for (int c = 1; c < w; ++c) {
            out.push_back(gen_edge(im, r, c, r-1, c, w));
            out.push_back(gen_edge(im, r, c, r-1, c-1, w));
            out.push_back(gen_edge(im, r, c, r, c-1, w));
            out.push_back(gen_edge(im, r, c, r+1, c-1, w));
        }
    }

    // bottom row
    for (int c = 1; c < w; ++c) {
        out.push_back(gen_edge(im, h-1, c, h-2, c, w));
        out.push_back(gen_edge(im, h-1, c, h-2, c-1, w));
        out.push_back(gen_edge(im, h-1, c, h-1, c-1, w));
    }

    // top row
    for (int c = 1; c < w; ++c) {
        out.push_back(gen_edge(im, 0, c, 0, c-1, w));
        out.push_back(gen_edge(im, 0, c, 1, c-1, w));
    }

    // left column
    for (int r = 1; r < h; ++r) {
        out.push_back(gen_edge(im, r, 0, r-1, 0, w));
    }
}

__attribute__((always_inline)) double bench(clock_t& start) {
    return (clock() - start) / double(CLOCKS_PER_SEC);
}



