from flask import Flask, g
import random
app = Flask(__name__)

@app.route("/<steps>")
def hello(steps):
    x = 0
    for i in range(2 ** int(steps)):
        x += 1
        x %= 1e9
    return "Hello World!"

if __name__ == "__main__":
    app.run()