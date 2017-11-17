#include "vis_kps.hpp"
#include "disjoint_set.hpp"

#define IM_DIR "img/"
#define OUT_PATH_KP "out_kp.png"
#define OUT_PATH_SEG "out_seg.png"
#define OUT_PATH_MATCH "out_match.png"

int main(int argc,char **argv) {

  // Read and check command line parameters.
  cimg_usage("Superimpose keypoints onto 2D image");
  const char *file_im = cimg_option("-img", IM_DIR "1.jpg", "Input Image");
  const char *file_kp = cimg_option("-kps", IM_DIR "1.kpt", "Input Keypoints");
  const double k_tol   = cimg_option("-k", 1.0, "k-val for tolerance");
  const char *weight_metric = cimg_option("-weight", "l2",
      "Metric to use for pixel dissimilarity");

  clock_t start;

  img_t img(file_im);
  int width = img.width();
  int height = img.height();
  int n_nodes = width*height;
  printf("image loaded. (h: %d, w: %d)\n", height, width);

  // -- Draw keypoints
  vector<float2_t> kps;
  extract_points(file_kp, kps);
  color_t point_color  = {255, 0, 0};

  img_t temp(img);
  for (auto& p : kps) {
      temp.draw_point(int(p.x), int(p.y), &point_color.x);
  }
  temp.save(OUT_PATH_KP);

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

  start = clock();
  vector<vector<int> >segments;
  cc.all_sets(segments);
  printf("extracted segments. (%fs)\n", bench(start));
  int n_seg = segments.size();

  //    3) Label
  vector<color_t> color_cycle;
  for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        for (int k = 0; k < 5; ++k) {
            color_cycle.push_back({50*i, 50*j, 50*k});
        }
      }
  }
  int n_colors = color_cycle.size();
  int color_ix = 0;

  vector<int> component_sizes;
  int labels[width][height];

  start = clock();
  img_t temp2(img);
  for (int seg_i = 0; seg_i < n_seg; ++seg_i) {
    auto& seg = segments[seg_i];
    component_sizes.push_back(seg.size());

    color_t color = color_cycle[color_ix++ % n_colors];
    for (int node_idx : seg) {
        int2_t p = itoij(node_idx, width);
        temp2(p.x, p.y, 0, 0) = color.x;
        temp2(p.x, p.y, 0, 1) = color.y;
        temp2(p.x, p.y, 0, 2) = color.z;

        labels[p.x][p.y] = seg_i;
    }
  }
  temp2.save(OUT_PATH_SEG);
  printf("colored segments. (%fs)\n", bench(start));
  printf("average segment size: %f\n", accumulate(
        component_sizes.begin(), component_sizes.end(), 0) / float(n_seg));

  // -- Matching
  vector<vector<float2_t> > keypoint_groups(n_seg);
  for (float2_t& p : kps) {
    int x = int(p.x);
    int y = int(p.y);
    keypoint_groups[labels[x][y]].push_back(p);
  }

  img_t temp3(img);
  color_ix = 0;
  for (auto& kp_group : keypoint_groups) {
    color_t color = color_cycle[color_ix++ % n_colors];
    for (auto& p : kp_group) {
        temp3.draw_point(int(p.x), int(p.y), &color.x);
    }
  }
  temp3.save(OUT_PATH_MATCH);
  
  return 0;
}
