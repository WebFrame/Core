import sys

if __name__ == "__main__":
    import requests

    url = list(enumerate(sys.argv))[1][1]
    try:
        x = requests.get(url)

        print(x.text)
        print(x.elapsed.total_seconds(), file=sys.stderr)
    except:
        print('Bad request')