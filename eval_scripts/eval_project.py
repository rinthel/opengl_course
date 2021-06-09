import os
import sys
from html.parser import HTMLParser
from urllib.parse import urlparse
import csv
import zipfile
from pathlib import Path

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

print("dirname:", dirname)
output_dirname = dirname + "_result"
output_filename = output_dirname + ".csv"

def save_result(output_filename, scorelist):
    with open(output_filename, "wt", encoding="utf8", newline='') as f:
        writer = csv.writer(f)
        for repo in scorelist:
            writer.writerow(repo.values())

def load_result(output_filename):
    scorelist = []
    with open(output_filename, "rt", encoding="utf8") as f:
        reader = csv.reader(f)
        keys = ["index", "id", "name", "root", "score", "comment"]
        for row in reader:
            repo = dict(zip(keys, row))
            print(repo)
            repo["index"] = int(repo["index"])
            repo["score"] = int(repo["score"])
            scorelist.append(repo)
    return scorelist


if os.path.isfile(output_filename):
    # load existing file
    scorelist = load_result(output_filename)
    print("loaded repo list")
    print(scorelist)
else:
    # create file
    filelist = os.listdir(dirname)
    scorelist = []

    with open('id_name.csv', "rt", encoding="utf8") as f:
        reader = csv.reader(f)
        for row in reader:
            scorelist.append({
                "index": row[0],
                "id": row[1],
                "name": row[2],
                "root": "",
                "score": -1,
                "comment": "",
                })

    for filename in filelist:
        if ".zip" not in filename:
            continue
        try:
            idnumber, name = get_idnumber_and_name(filename)
            found_row = list(filter(lambda row: row["name"] == name, scorelist))
            assert len(found_row) == 1
            row = found_row[0]
            
            # unzip file
            f = zipfile.ZipFile(os.path.join(dirname, filename))
            target_dir = os.path.join(output_dirname, row["id"])
            f.extractall(target_dir)

            # find CMakeLists.txt root directory
            extract_files = list(Path(target_dir).rglob("*.[tT][xX][tT]"))
            for extract_file in extract_files:
                rootpath, filename = os.path.split(extract_file)
                if filename.lower() == 'cmakelists.txt':
                    row["root"] = rootpath
            
        except Exception as e:
            print("cannot process:", filename)
            raise e

    save_result(output_filename, scorelist)

rootdir = os.getcwd()
for index, repo in enumerate(scorelist):
    prompt = "[{:2d}/{:2d}]".format(index + 1, len(scorelist))
    if repo["score"] != -1:
        print("{} skipping {} / {}: {}".format(
            prompt, repo["id"], repo["name"], repo["score"]))
        continue

    if len(repo["root"]) == 0:
        target_dir = os.path.join(output_dirname, repo["id"])
        extract_files = list(Path(target_dir).rglob("*.[tT][xX][tT]"))
        for extract_file in extract_files:
            rootpath, filename = os.path.split(extract_file)
            if filename == 'CMakeLists.txt':
                repo["root"] = rootpath

    if len(repo["root"]) == 0:
        print("{} not repoted {} / {}. set score to zero...".format(
            prompt, repo["id"], repo["name"]))
        repo["score"] = 0
        repo["comment"] = "미제출"
        os.chdir(rootdir)
        save_result(output_filename, scorelist)
        continue

    print("=" * 50)
    print("{} evaluating {} / {}".format(
        prompt, repo["id"], repo["name"]))

    basedir = repo["root"]
    os.chdir(basedir)
    try:
        os.system("cmake . -Bbuild")
        os.system("cmake --build build --config Debug -- /m:12")
        # find executabe file
        exename = list(filter(lambda x: ".exe" in x, list(os.listdir(os.path.join("build", "Debug")))))[0]
        exepath = os.path.join(".", "build", "Debug", exename)
        
        # test code
        os.system("{}".format(exepath))
        # os.system("{} 0.6 0.5 32".format(exepath))
        # os.system("{} 0.8 0.4 32 45 135".format(exepath))
        # os.system("{} 0.8 0.7 32 0 270".format(exepath))
        # os.system("{} 0.5 0.25 64 0 360 0 1 0".format(exepath))
        # os.system("{} 0.6 0.3 64 120 240 1 0.5 0".format(exepath))
    except Exception as e:
        print("error occurred:", e)
    finally:
        os.chdir(rootdir)

    print("{} evaluating {} / {}".format(
        prompt, repo["id"], repo["name"]))
    print("please set score (0-10):")
    repo["score"] = int(sys.stdin.readline())
    print("please leave comment:")
    repo["comment"] = sys.stdin.readline().strip()

    save_result(output_filename, scorelist)
