import pandas as pd

df = pd.read_csv("data/raw/TMDB_movie_dataset_v11.csv")

print("raw:", len(df))

df = df[df["adult"] == False]
print("Remove adult:", len(df))

df = df.drop_duplicates(subset="id") # on id, because some titles might get mixed up
print("Remove duplicate ids:", len(df))

df = df[df["overview"].fillna("").str.split().str.len() >= 15] # split the string, then check the length is over 15 words.
print("overview >= 15 words:", len(df))

df = df[df["vote_count"] >= 5]
print("after number of votes in IMDB >= 5: (people know this movie)", len(df))

df = df.reset_index(drop=True)
df.to_csv("data/clean_movies.csv", index=False)
