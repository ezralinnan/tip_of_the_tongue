import pandas as pd
import numpy as np

import sys

SAMPLE_SIZE = None

suffix = f"_sample{SAMPLE_SIZE}" if SAMPLE_SIZE else ""

failures = 0

def check(name, ok):
    global failures
    print("PASS" if ok else "FAIL:", name)
    if not ok:
        failures += 1


movies = pd.read_csv(f"data/movies{suffix}.csv")

with open(f"data/vectors{suffix}.bin", "rb") as f:
    count, dim = np.fromfile(f, dtype=np.int32, count=2)
    vectors = np.fromfile(f, dtype=np.float32)


print(f"Read {count} vectors of dimension {dim} from data/vectors{suffix}.bin")

# check if sizes match for reshaping
if len(vectors) != count * dim:
    print(f"FAIL: Expected {count * dim} elements, but got {len(vectors)}")
    sys.exit(1)


vectors = vectors.reshape((count, dim))

# file checks

check("vectors count matches movies", count == len(movies))
check("each vector has correct dimension", dim == 384)
check("row columns in order", (movies["row"] == np.arange(len(movies))).all())
check("tmdb_id is unique", movies["tmdb_id"].is_unique)
check("no NaNs in vectors", not np.isnan(vectors).any())

lengths = np.linalg.norm(vectors, axis=1)
check("vectors are normalized", np.allclose(lengths, 1.0, atol=1e-5))




if failures == 0:
    print("All checks passed!")