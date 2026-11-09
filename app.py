from flask import Flask, request, render_template
import subprocess

app = Flask(__name__, template_folder="UI")

last_kd_results = ""
last_lsh_results = ""


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/search", methods=["POST"])
def search():

    global last_kd_results
    global last_lsh_results

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

    output = result.stdout

    last_kd_results = output
    last_lsh_results = output

    return render_template(
        "results.html",
        search_results=last_kd_results
    )


@app.route("/comparison")
def comparison():

    return render_template(
        "comparison.html",
        kd_results=last_kd_results,
        lsh_results=last_lsh_results
    )


if __name__ == "__main__":
    app.run(debug=True)