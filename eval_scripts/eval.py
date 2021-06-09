import os
import sys
from html.parser import HTMLParser
from urllib.parse import urlparse
import csv

class GithubParser(HTMLParser):
    def __init__(self):
        super().__init__()
        self.data = []
        self.result = None

    def handle_data(self, data):
        self.data.append(data)
        tokens = data.split()
        for token in tokens:
            if "https://github.com" in token:
                self.result = urlparse(token)

def get_idnumber_and_name(filename):
    [name, idnumber, *_] = filename.split("_")
    return (idnumber, name)

def get_repo_url(filepath):
    with open(filepath, "r", encoding="utf8") as f:
        text = "\n".join(f.readlines())
        parser = GithubParser()
        parser.feed(text)
        if parser.result is None:
            print("can't find url:", parser.data)
            while True:
                try:
                    print("please type username/reponame")
                    user_repo_name = sys.stdin.readline().strip()
                    parser.result = urlparse("https://github.com/" + user_repo_name)
                    break
                except Exception as e:
                    print("failed to parse:", e)
        return parser.result

dirname = sys.argv[1]
tag_name = ""
print(sys.argv)
if len(sys.argv) > 2:
    tag_name = sys.argv[2]

print("dirname:", dirname)
output_dirname = dirname + "_result"
output_filename = output_dirname + ".csv"

def save_result(output_filename, repolist):
    with open(output_filename, "wt", encoding="utf8", newline='') as f:
        writer = csv.writer(f)
        for repo in repolist:
            writer.writerow(repo.values())

def load_result(output_filename):
    repolist = []
    with open(output_filename, "rt", encoding="utf8") as f:
        reader = csv.reader(f)
        keys = ["index", "id", "name", "github_id", "github_repo", "score", "comment"]
        for row in reader:
            repo = dict(zip(keys, row))
            print(repo)
            repo["index"] = int(repo["index"])
            repo["score"] = int(repo["score"])
            repolist.append(repo)
    return repolist


if os.path.isfile(output_filename):
    # load existing file
    repolist = load_result(output_filename)
    print("loaded repo list")
    print(repolist)
else:
    # create file
    filelist = os.listdir(dirname)
    repolist = []

    with open('id_name.csv', "rt", encoding="utf8") as f:
        reader = csv.reader(f)
        for row in reader:
            repolist.append({
                "index": row[0],
                "id": row[1],
                "name": row[2],
                "github_id": "",
                "github_repo": "",
                "score": -1,
                "comment": "",
                })

    for filename in filelist:
        try:
            idnumber, name = get_idnumber_and_name(filename)
            found_repo = list(filter(lambda repo: repo["name"] == name, repolist))
            assert len(found_repo) == 1
            repo = found_repo[0]
            url = get_repo_url(os.path.join(dirname, filename))
            [_, username, reponame, *_] = url.path.split("/")
            if ".git" in reponame:
                reponame = reponame.split(".")[0]
            repo["github_id"] = username
            repo["github_repo"] = reponame
        except Exception as e:
            print("cannot process:", filename)
            raise e

    save_result(output_filename, repolist)

rootdir = os.getcwd()
for index, repo in enumerate(repolist):
    prompt = "[{:2d}/{:2d}]".format(index + 1, len(repolist))
    if repo["score"] != -1:
        print("{} skipping {} / {}: {}".format(
            prompt, repo["id"], repo["name"], repo["score"]))
        continue

    if len(repo["github_id"]) == 0 or len(repo["github_repo"]) == 0:
        print("{} not repoted {} / {}. set score to zero...".format(
            prompt, repo["id"], repo["name"]))
        repo["score"] = 0
        repo["comment"] = "미제출"
        os.chdir(rootdir)
        save_result(output_filename, repolist)
        continue

    github_url = "https://github.com/{}/{}".format(repo["github_id"], repo["github_repo"])
    print("=" * 50)
    print("{} evaluating {} / {}: {}".format(
        prompt, repo["id"], repo["name"], github_url))

    basedir = os.path.join(output_dirname, repo["id"])
    os.makedirs(basedir, exist_ok=True)
    os.chdir(basedir)
    try:
        os.system("git clone {}".format(github_url))
        os.chdir(repo["github_repo"])
        if tag_name:
            os.system("git checkout {}".format(tag_name))
        os.system("cmake . -Bbuild")
        os.system("cmake --build build --config Debug -- /m:12")
        # find executabe file
        exename = list(filter(lambda x: ".exe" in x, list(os.listdir(os.path.join("build", "Debug")))))[0]
        os.system(os.path.join(".", "build", "Debug", exename))
    except Exception as e:
        print("error occurred:", e)
    finally:
        os.chdir(rootdir)

    print("{} evaluating {} / {}: {}".format(
        prompt, repo["id"], repo["name"], github_url))
    print("please set score (0-10):")
    repo["score"] = int(sys.stdin.readline())
    print("please leave comment:")
    repo["comment"] = sys.stdin.readline().strip()

    save_result(output_filename, repolist)
