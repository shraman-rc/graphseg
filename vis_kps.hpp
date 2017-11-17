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

inline int ijtoi(int2_t pos, int width) {
    return pos.y*width + pos.x;
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

inline edge_t gen_edge(img_t& im, int2_t pos1, int2_t pos2, int width) {
    int3_t p1 = {
        im(pos1.x, pos1.y, 0, 0),
        im(pos1.x, pos1.y, 0, 1),
        im(pos1.x, pos1.y, 0, 2)};
    int3_t p2 = {
        im(pos2.x, pos2.y, 0, 0),
        im(pos2.x, pos2.y, 0, 1),
        im(pos2.x, pos2.y, 0, 2)};

    return {ijtoi(pos1, width), ijtoi(pos2, width), pix_diff(p1, p2)};
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

    for (int y = 1; y < h-1; ++y) {
        for (int x = 1; x < w; ++x) {
            out.push_back(gen_edge(im, {x,y}, {x, y-1}, w));
            out.push_back(gen_edge(im, {x,y}, {x-1, y-1}, w));
            out.push_back(gen_edge(im, {x,y}, {x-1, y}, w));
            out.push_back(gen_edge(im, {x,y}, {x-1, y+1}, w));
        }
    }

    // bottom row
    for (int x = 1; x < w; ++x) {
        out.push_back(gen_edge(im, {x, h-1}, {x, h-2}, w));
        out.push_back(gen_edge(im, {x, h-1}, {x-1, h-2}, w));
        out.push_back(gen_edge(im, {x, h-1}, {x-1, h-1}, w));
    }

    // top row
    for (int x = 1; x < w; ++x) {
        out.push_back(gen_edge(im, {x, 0}, {x-1, 0}, w));
        out.push_back(gen_edge(im, {x, 0}, {x-1, 1}, w));
    }

    // left column
    for (int y = 1; y < h; ++y) {
        out.push_back(gen_edge(im, {0, y}, {0, y-1}, w));
    }
}

__attribute__((always_inline)) double bench(clock_t& start) {
    return (clock() - start) / double(CLOCKS_PER_SEC);
}
