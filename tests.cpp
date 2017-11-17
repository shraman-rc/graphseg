#include "vis_kps.hpp"
#include "disjoint_set.hpp"

void test_disjoint_set() {
    printf("== test_disjoint_set ==\n");

    constexpr int N = 500;
    DisjointSets cc(N);

    printf("-> Simple merge...");
    cc.merge(4, 499, 1);
    cc.merge(8, 1, 2);
    cc.merge(1, 499, 3);
    cc.merge(1, 8, 3);
    cc.merge(4, 4, 3);

    assert((cc.set_size(8) == 4) &&
           (cc.set_size(499) == 4) &&
           (cc.set_size(4) == 4) &&
           (cc.set_size(1) == 4));

    assert((cc.find(8) == 8) &&
           (cc.find(499) == 8) &&
           (cc.find(4) == 8) &&
           (cc.find(1) == 8));

    vector<vector<int> > sets;
    cc.all_sets(sets);

    assert(sets.size() == N-3);

    printf("PASSED\n");

}

int main() {
    test_disjoint_set();
    return 0;
}
