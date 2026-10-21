import pandas as pd

df = pd.read_csv("data/raw/TMDB_movie_dataset_v11.csv")

print(df.shape)
print(df.head())
print(df.info())