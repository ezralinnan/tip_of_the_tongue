# Tip of the Tongue: Movie Finder

Search for a movie by describing it rather than searching for popular keywords or a title. This could include actors, characters, settings, plot point, or general descriptions.

** Group 21 ** Ezra Linnan, Claudio Coello, Ana De Cardenas

### Algorithms:
**Brute force:** compare the query against every movie (correct answer, very slow) - baseline

**KD-tree:** split the data repeatedly by a dimensional axis and search only promising regions - exact

**Locality-sensitive hashing (LSH):** similar movies are hashed into the same "bucket" by random hyperplanes and searching queries only checks the hash bucket it matches - approximate but fast

We also tested a capped KD-tree, which stops after visiting a certain amount of leaves. This approach makes it approximate like LSH but improves speed.

What are we measuring: accuracy compared to algorithm speed

Testing results for both data structures are available in the benchmarks folder.


## Pipeline
1. Clean the data: we cleaned the raw Kaggle file data to remove duplicates, adult films, unknown movies, and movies with little-to-no summary
2. Embed data into vectors: combine each movie's summary, genres, and keywords into a large text and then transform it to a 384-D number vector using `all-MiniLM-L6-v2` model.
3. Reconcile: check to make sure vectors and movie lists line up and that embedding went correct. Also checks that the vectors will be usable throughout the rest of the pipeline (no NaN, normalized vectors, 384D).
4. Query: the user's description will go through the same processing, turning into a vector that can be fed into all algorithms. 
5. Search: the C++ engines build by scratch to load the vector and search via the algorithms.
6. Program: the Flask app connects a sophisticated UI to the engines and provides results and algorithm comparisons.

What each title's "score" means: every result has a score from cosine similarity between the query and the found movie. Closer to 1 means a closer match.

## Setup

1. ```pip install -r requirements.txt```
2. Download the data from the Google Drive folder into `data/`
3. Compile the engine using ```g++ -O2 -std=c++17 -Iengine/include engine/src/movie_search.cpp engine/src/kd_tree.cpp engine/src/lsh.cpp engine/src/loader.cpp -o movie_search```
4. Run the app from the root and open http://127.0.0.1:5000
5. Finally, run ```python app.py```


## Data

Training dataset from Kaggle, nearly 1,000,000 movie records along with keywords, genres, summaries, and other movie information. After cleaning, we have 115,049 viable movies for searching.

Download link for movies.csv and vectors.bin: https://drive.google.com/drive/folders/1ILDijFZaFGOCPk6gXT3ipaW0L-DQNeuz?usp=sharing

Both files should be placed into a folder named `data/` from the project root.


## Results

Testing methods: 1,000 most popular movies are queries, and we ask for the top 6 closest matches (5 + itself). Recall is the share of brute force's top 6 that each method correctly found (the brute force algorithm is our answer key). 

**Exact KD-tree actually turned out slower than brute force:** 69 ms vs. 39 ms after checking all 115,049 movies. This is because of the "curse of dimensionality" where in high dimensions, everything is equally as far from each other, so the tree never gets to skip branches. The cost of walking the tree on addition to querying every movie leads to a slower search.

**Capped KD-tree**

| Share of movies checked | Recall | ms per query |
|---|---|---|
| 1% | 0.31 | 0.66 |
| 5% | 0.50 | 3.35 |
| 10% | 0.60 | 6.77 |
| 25% | 0.77 | 17.16 |

**LSH**

 | Bits | Tables | Share checked | Recall | ms per query | Build time |
|---|---|---|---|---|---|
| 8 | 20 | 18.9% | 0.80 | 16.36 | 9.2 s |
| 8 | 10 | 11.1% | 0.62 | 9.73 | 4.9 s |
| 10 | 20 | 7.3% | 0.62 | 6.71 | 11.8 s |
| 12 | 20 | 2.5% | 0.44 | 2.45 | 14.5 s |
| 16 | 10 | 0.1% | 0.22 | 0.22 | 10.0 s |

- When both algorithms check a similar amount of data, LSH and capped KD-tree perform close to equally. LSH pulls ahead slightly when budget is small.
- More bits means smaller buckets, which turns out to be faster but worse recall. More tables means more candidates to find a match from, leading to better recall but a longer build and query.
- LSH cost is up front, so an improvement could be to cache this somehow so we only compute it once and then worry about query time alone.


## Team workflow

Ezra Linnan: embedding the data, LSH implementation and testing, LSH benchmark 
Claudio Coello: cleaning the data, Brute force & KD-tree implementation and testing, KD-tree benchmarks
Ana De Cardenas: UI, visuals, program connection, overall project management
