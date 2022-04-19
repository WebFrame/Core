import sys

if __name__ == "__main__":
    import requests

    url = list(enumerate(sys.argv))[1][1]

    x = requests.get(url)

    print(x.text)