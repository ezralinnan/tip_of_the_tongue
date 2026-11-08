from flask import Flask, request, render_template
import subprocess

app = Flask(__name__, template_folder="UI")

@app.route("/")
def home():
    return render_template("index.html")

@app.route("/search", methods=["POST"])
def search():

    description = request.form["description"]

    subprocess.run([
        "C:/Users/adcda/AppData/Local/Programs/Python/Python313/python.exe",
        "pipeline/query_embed.py",
        description
    ])

    result = subprocess.run(
        ["movie_search.exe"],
        capture_output=True,
        text=True
    )

    return f"<pre>{result.stdout}</pre>"

if __name__ == "__main__":
    app.run(debug=True)