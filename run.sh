./main -k 300000 -img img/$1.jpg -kps img/$1.kpt -m $2
eog out_kp.png out_seg.png out_match.png
