#include<vector>
#include<map>

class DisjointSets {

  public:

    size_t numel = 0;
    size_t numsets = 0;

    vector<int> parents; // Parent pointers
    vector<size_t> size; // Size of the set rooted at index i
    vector<float> max_w;   // Max edge weight of the set rooted at index i
    vector<int> rank;    // Rank of the set rooted at index i
      //  - if there is no set rooted at i then rank[i] are meaningless
      //  - although similar, rank != height; we do not update rank after
      //    path compression, but doesn't matter since amortized analysis
      //    holds true so long as |S| >= 2^(rank(S)) for every set S


    DisjointSets(int n, float init_internal_diff=0.0f) : numel(n), numsets(n) {
        parents.resize(n);
        size.resize(n);
        rank.resize(n);
        max_w.resize(n);

        for (int i = 0; i < n; ++i) {
            parents[i] = i; // each node starts off as its own component
            size[i] = 1;
            rank[i] = 0;
            max_w[i] = init_internal_diff;
        }
    }

///    void make_set(int elem) {
///
///      if (ttoi.count(elem) > 0) return;
///
///      ttoi[elem] = numel;
///      itot[numel] = elem;
///      parents.push_back(numel);
///      rank.push_back(0);
///      size.push_back(1);
///      numel++;
///      numsets++;
///    }

    int find(int elem) {
      vector<int> idxs;

      // trace back to root
      while(parents[elem] != elem) {
        idxs.push_back( elem );
        elem = parents[elem];
      }

      // path compression
      for (int i = 0; i < int(idxs.size()); ++i) {
        parents[idxs[i]] = elem;
      }

      return elem;
    }

    void merge(int e1, int e2, float max_weight) {
      int p1 = find(e1);
      int p2 = find(e2);

      if (p1 == p2) return; // same set

      numsets--;

      // union-by-rank
      if (rank[p1] > rank[p2]) {
        parents[p2] = p1;
        size[p1] += size[p2];
        size[p2] = 0;
        max_w[p1] = max_weight;
      } else if (rank[p1] < rank[p2]) {
        parents[p1] = p2;
        size[p2] += size[p1];
        size[p1] = 0;
        max_w[p2] = max_weight;
      } else {
        parents[p2] = p1;
        size[p1] += size[p2];
        size[p2] = 0;
        rank[p1]++;
        max_w[p1] = max_weight;
      }
    }

    size_t set_size(int elem) {
      // Returns size of set containing elem
      return size[find(elem)];
    }

    int int_diff(int e1) {
        return max_w[parents[e1]];
    }

    void all_sets(vector<vector<int> >& sets) {
      unordered_map<int, int> pi_map;
      int pi_ix = 0;
      for (int node_i = 0; node_i < int(numel); ++node_i) {
        if (size[node_i] > 0) { // found set rep
          pi_map[node_i] = pi_ix;
          pi_ix++;
        }
      }

      sets.resize(numsets);
      for (int node_i = 0; node_i < int(numel); ++node_i) {
          int pi = find(node_i);
          sets[pi_map[pi]].push_back(node_i);
      }
    }
};
