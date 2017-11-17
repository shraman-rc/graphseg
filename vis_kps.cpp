#include "vis_kps.hpp"
#include "disjoint_set.hpp"

#define IM_DIR "img/"
#define OUT_PATH "out.png"

int main(int argc,char **argv) {

  // Read and check command line parameters.
  cimg_usage("Superimpose keypoints onto 2D image");
  const char *file_im = cimg_option("-img", IM_DIR "1.jpg", "Input Image");
  const char *file_kp = cimg_option("-kps", IM_DIR "1.kpt", "Input Keypoints");
  const double k_tol   = cimg_option("-k", 1.0, "k-val for tolerance");

  clock_t start;

  img_t img(file_im);
  int width = img.width();
  int height = img.height();
  int n_nodes = width*height;
  printf("image loaded. (h: %d, w: %d)\n", height, width);

  // -- Draw keypoints
  vector<point_t> kps;
  extract_points(file_kp, kps);
  color_t point_color  = {255, 0, 0};

  //for (auto& p : kps) {
  //    img.draw_point(int(p.x), int(p.y), &point_color.x);
  //}
  //img.save(OUT_PATH);

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
  vector<color_t> color_cycle;
  for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        for (int k = 0; k < 5; ++k) {
            color_cycle.push_back({50*i, 50*j, 50*k});
        }
      }
  }
  int n_colors = color_cycle.size();

 // color_t color_cycle[3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};
 // int n_colors = 3;

  start = clock();
  vector<vector<int> >segments;
  cc.all_sets(segments);
  printf("extracted segments. (%fs)\n", bench(start));

  int color_ix = 0;
  int n_seg = segments.size();
  vector<int> component_sizes;

  start = clock();
  for (auto& seg : segments) {
    component_sizes.push_back(seg.size());
    color_t color = color_cycle[color_ix++ % n_colors];
    for (int node_idx : seg) {
        int3_t p = itoij(node_idx, width);
        // TODO: change to set the pixel value instead
        img.draw_point(int(p.x), int(p.y), &color.x);
    }
  }
  img.save(OUT_PATH);
  printf("colored segments. (%fs)\n", bench(start));
  printf("average segment size: %f\n", accumulate(
        component_sizes.begin(), component_sizes.end(), 0) / float(n_seg));

  return 0;
}
