from flask import Flask, g
import random
app = Flask(__name__)

@app.route("/<steps>/1")
def hello(steps):
    x = 0
    for i in range(2 ** int(steps)):
        x += 1
        x %= 1e9
    return "Hello World!"

@app.route("/<steps>/2")
def hello2(steps):
    x = 0
    for i in range(2 ** int(steps)):
        x += random.randint(0, 1e9)
        x %= 1e9
    return "Hello World!"

if __name__ == "__main__":
    app.run(port=8888)