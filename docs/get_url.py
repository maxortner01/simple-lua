import sys, json

color_ranges = { 0: "red", 50: "orange", 75: "yellow", 90: "green" }
percent = float(json.loads(open(sys.argv[1] + "/kcov-merged/coverage.json", "r").read())["percent_covered"])

print(percent)
colors = [ (v, c) for v, c in color_ranges.items() if percent > v ]
max_val = max([c[0] for c in colors])
color = [c[1] for c in colors if c[0] == max_val][0]
url = "https://img.shields.io/badge/code_coverage-{perc}%25-{color}?link=https://simple-lua.maxortner.com/coverage.html".format(perc=int(percent), color=color)
html = """
<head><meta http-equiv="refresh" content="0; url={url}" /></head>
<body>Redirecting...</body>
""".format(url=url)
open(sys.argv[1] + "/coverage-badge.html", "w").write(html)