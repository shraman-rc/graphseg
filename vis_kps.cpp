#include<bits/stdc++.h>
#include "CImg.h"

using namespace std;

#include "disjoint_set.hpp"

#define IM_DIR "test/"
#define OUT_PATH "out.png"

struct point_t {
    float x,y;
};

struct edge_t {
    int u, v;
    float w;
};

template<typename T>
struct vec3_t {
    T x,y,z;
};

typedef cil::CImg<unsigned char> img_t;
typedef vec3_t<int> int3_t;
typedef vec3_t<float> float3_t;
typedef vec3_t<double> double3_t;

inline int ijtoi(int r, int c, int width) {
    return r*width + c;
}

inline int3_t itoij(int i, int width) {
    int y = i / width;
    int x = i - (y*width);
    return {x, y, 0};
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

void extract_points(const char* path, vector<point_t>& out) {
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


int main(int argc,char **argv) {

  // Read and check command line parameters.
  cimg_usage("Superimpose keypoints onto 2D image");
  const char *file_im = cimg_option("-img", IM_DIR "1.jpg", "Input Image");
  const char *file_kp = cimg_option("-kps", IM_DIR "1.kpt", "Input Keypoints");
  const double k_tol   = cimg_option("-k", 40.0, "k-val for tolerance");

  clock_t start;

  img_t img(file_im);
  int width = img.width();
  int height = img.height();
  int n_nodes = width*height;
  printf("image loaded. (h: %d, w: %d)\n", height, width);

  // -- Draw keypoints
  vector<point_t> kps;
  extract_points(file_kp, kps);
  unsigned char point_color[3] = {255, 0, 0};

  for (auto& p : kps) {
      img.draw_point(int(p.x), int(p.y), point_color);
  }

  img.save(OUT_PATH);

  // -- Segmentation
  //    1) Build graph
  start = clock();
  vector<edge_t> edges;
  edges.reserve(4*n_nodes); // over-approximation
  build_graph(img, edges);
  printf("graph built. (edges: %zu) (%fs)\n", edges.size(), bench(start)); // TODO: Check

  //    1.5) Sort edges
  start = clock();
  auto cmp = [](edge_t l, edge_t r) {
    return l.w < r.w;
  };
  sort(edges.begin(), edges.end(), cmp);
  printf("sorted. (%fs)\n", bench(start));

  //    2) Felzenswalb
  start = clock();
  DisjointSets cc(n_nodes);
  for (auto& e : edges) {
      int n1 = e.u, n2 = e.v;
      float thres1 = cc.int_diff(n1) + k_tol / float(cc.set_size(n1));
      float thres2 = cc.int_diff(n2) + k_tol / float(cc.set_size(n2));

      if (e.w < min(thres1, thres2)) {
          cc.merge(n1, n2, e.w);
      }
  }
  printf("segmented. (%fs)\n", bench(start));

  //    3) Draw
  for (


// for (auto& p : kps) {
//        cout << p.x << " " << p.y << endl;
//    }

//  // Compute the faded image.
//  const double ca = std::cos(angle), sa = std::sin(angle);
//  double alpha;
//  cimg_forXYZC(dest,x,y,z,k) {
//    const double X = ((double)x/img1.width() - 0.5)*ca + ((double)y/img1.height() - 0.5)*sa;
//    if (X + 0.5<pmin) alpha = 0; else {
//      if (X + 0.5>pmax) alpha = 1; else
//        alpha = (X + 0.5 - pmin)/(pmax - pmin);
//    }
//    dest(x,y,z,k) = (unsigned char)((1 - alpha)*img1(x,y,z,k) + alpha*img2(x,y,z,k));
//  }
//
//  // Save and exit
//  if (file_o) dest.save(file_o);
//  if (visu) dest.display("Image fading");
  return 0;
}
