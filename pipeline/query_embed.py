import numpy as np
import sys

from sentence_transformers import SentenceTransformer

if len(sys.argv) < 2:
    print("Usage: python query_embed.py \"movie description\"")
    sys.exit(1)

query = sys.argv[1]

model = SentenceTransformer("all-MiniLM-L6-v2")

vector = model.encode(
    [query],
    normalize_embeddings=True
).astype(np.float32)

vector.tofile("data/query_vector.bin")

print("Query vector written to data/query_vector.bin")
print("Dimension:", vector.shape[1])