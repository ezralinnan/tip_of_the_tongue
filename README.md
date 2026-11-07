# Tip of the Tongue: Movie Finder


Search for a movie by describing it rather than searching for popular keywords or a title. This could include actors, characters, settings, plot point, or general descriptions.

** Group 21 ** Ezra Linnan, Claudio Coello, Ana De Cardenas

### Algorithms:
**Brute force:** comparer against every movie (correct answer, very slow) - baseline

**KD-tree:** split the data repeatedly by a dimensional axis and search only promising regions - exact

**Locality-sensitive hashing (LSH):** similar movies are hashed into the same "bucket" and searching only checks that bucket - approximation but fast


What are we measuring: accuracy compared to algorithm speed

Testing results for both data structures are available in the benchmarks folder.


## Pipeline


## Setup


## Data

Training dataset from Kaggle, nearly 1,000,000 movie records along with keywords, genres, summaries, and other movie information.

Download link for movies.csv and vectors.bin: https://drive.google.com/drive/folders/1ILDijFZaFGOCPk6gXT3ipaW0L-DQNeuz?usp=sharing

Both files should be placed into a folder named 'data/' from the project root.


## Results


## Team workflow
