import numpy as np
import pandas as pd

from sentence_transformers import SentenceTransformer


def build_text(r):
    parts = [r.overview.strip()]
    if r.genres:
        parts.append("Genres: " + r.genres + ".")
    if r.keywords:
        parts.append("Keywords: " + r.keywords + ".")

    return " ".join(parts)



df = pd.read_csv("clean.csv").reset_index(drop=True)
df['text'] = df.apply(build_text, axis=1)

model = SentenceTransformer('all-MiniLM-L6-v2')
vectors = model.encode(df['text'].tolist(), batch_size=256, 
                       normalize_embeddings=True, show_progress_bar=True).astype(np.float32)

df["row"] = df.index

with open("vectors.bin", "wb") as f:
    np.array([len(vectors), 384], dtype=np.int32).tofile(f)
    vectors.tofile(f)