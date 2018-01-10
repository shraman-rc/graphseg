A self-contained C++ implementation of Felzenszwalb's graph-based image segmentation algorithm ([Felzenszwalb et. al.](http://people.cs.uchicago.edu/~pff/papers/seg-ijcv.pdf)).

Leverages CImg for I/O and drawing but otherwise written from scratch. Ping me if you find bugs!

## Compiling

Requires Ubuntu 14.04+, g++ 4.8.4+, make 3.8+. Then `make` and you're set.

## Running

Example: `./main -k 50000 -img img/2.jpg -m 400`. Results are saved to `out_seg.png`.

The `-k` flag designates the `k` parameter in computing the threshold for the minimum internal difference for each component. Increasing `k` encourages larger components.

The `-m` flag designates the merge threshold for swallowing smaller components into larger ones during post-processing. Increasing `m` also encourages larger components.

Due to the algorithm's simplicity, getting nice segmentation results requires striking a balance between `k` and `m` to avoid over-/under-segmentation artifacts. Note that `k` influences a threshold which adaptively during the segmentation phase, whereas `m` dictates agglomeration in a local neighborhood _after_ the spanning forest is complete. If, for instance, you'd like to capture the entire railing in `img/2.jpg` as one segment, you must increase `k`.

### Keypoint-based tracking

To use this code for tracking, simply feed in a keypoints file with the `-kps` flag. See `img/` for sample keypoint files and `run.sh` for sample commands.

## Implementation Details

- Edges include diagonals (i.e., 8 neighbors for non-border pixels).
- Edges weights are computed as L2 distance between pixels (easily modifiable), see `main.hpp -> build_graph`.
- 
