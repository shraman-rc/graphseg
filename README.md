A self-contained C++ implementation of Felzenszwalb's image segmentation algorithm ([Felzenszwalb et. al.](http://people.cs.uchicago.edu/~pff/papers/seg-ijcv.pdf)). Uses [CImg](http://cimg.eu/) for I/O and drawing.

## Compiling

Requires g++ 4.8.4+, make 3.8+. Call `make` and you're set.

## Running

Example: `./main -k 50000 -img img/2.jpg -m 400`. Results saved to `out_seg.png`.

The `-k` flag designates the `k` parameter in computing the threshold for the minimum internal difference for each component. Increasing `k` encourages larger components.

The `-m` flag designates the merge threshold for swallowing smaller components into larger ones during post-processing. Increasing `m` also encourages larger components.

Due to the algorithm's simplicity, getting nice segmentation results requires striking a balance between `k` and `m` to avoid over-/under-segmentation. If, for instance, you'd like to capture the entire railing in `img/2.jpg` as one segment, you must increase `k` (not `m`). Note that `k` influences a threshold which adaptively during the segmentation phase, whereas `m` dictates agglomeration in a local neighborhood _after_ the spanning forest is complete. 


## Implementation Details

- Edges include diagonals (i.e., 8 neighbors for non-border pixels).
- Edges weights are computed as L2 distance between pixels (easily modifiable), see `main.hpp -> build_graph`.
