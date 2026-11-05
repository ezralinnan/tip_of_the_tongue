import numpy as np
import pandas as pd

from sentence_transformers import SentenceTransformer


SAMPLE_SIZE = 100

def build_text(r):
    parts = [r.overview.strip()]
    if r.genres:
        parts.append("Genres: " + r.genres + ".")
    if r.keywords:
        parts.append("Keywords: " + r.keywords + ".")

    return " ".join(parts)


df = pd.read_csv("data/clean_movies.csv").reset_index(drop=True)
print("Clean movies: ", len(df))


for col in ["overview","genres", "keywords", "poster_path"]: # fill in NaN with empty strings
    df[col] = df[col].fillna("")
#^ has to live here because CSV can't store empty strings


if SAMPLE_SIZE:
    df = df.sample(SAMPLE_SIZE, random_state=21).reset_index(drop=True)
    print("Sampled movies: ", len(df))


df['text'] = df.apply(build_text, axis=1)
print(df['text'].sample(3, random_state=21).to_list())

model = SentenceTransformer('all-MiniLM-L6-v2')
vectors = model.encode(df['text'].tolist(), batch_size=256, 
                       normalize_embeddings=True, show_progress_bar=True).astype(np.float32)

suffix = f"_sample{SAMPLE_SIZE}" if SAMPLE_SIZE else ""

df["row"] = df.index

movies = df[["row", "id", "title", "year", "poster_path"]].rename(columns={"id": "tmdb_id"})
movies.to_csv(f"data/movies{suffix}.csv", index=False)


with open(f"data/vectors{suffix}.bin", "wb") as f:
    np.array([len(vectors), 384], dtype=np.int32).tofile(f)
    vectors.tofile(f)


print(f"Wrote {len(vectors)} vectors to data/vectors{suffix}.bin")