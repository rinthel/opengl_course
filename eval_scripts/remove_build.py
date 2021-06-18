import os
import sys
from html.parser import HTMLParser
from urllib.parse import urlparse
import csv
import zipfile
from pathlib import Path


# find CMakeLists.txt root directory
dirname = sys.argv[1]
output_dirname = dirname
extract_dirs = list(Path(output_dirname).rglob("build"))
build_dirs = []
for extract_dir in extract_dirs:
    rootpath, name = os.path.split(extract_dir)
    if os.path.isdir(extract_dir) and os.path.isfile(os.path.join(rootpath, "CMakeLists.txt")):
        print("removing {}".format(extract_dir))
        os.system("rm -rf {}".format(extract_dir))

# rootdir = os.getcwd()
# for index, repo in enumerate(scorelist):
#     prompt = "[{:2d}/{:2d}]".format(index + 1, len(scorelist))
#     if repo["score"] != -1:
#         print("{} skipping {} / {}: {}".format(
#             prompt, repo["id"], repo["name"], repo["score"]))
#         continue

#     if len(repo["root"]) == 0:
#         target_dir = os.path.join(output_dirname, repo["id"])
#         extract_files = list(Path(target_dir).rglob("*.[tT][xX][tT]"))
#         for extract_file in extract_files:
#             rootpath, filename = os.path.split(extract_file)
#             if filename == 'CMakeLists.txt':
#                 repo["root"] = rootpath

#     if len(repo["root"]) == 0:
#         print("{} not repoted {} / {}. set score to zero...".format(
#             prompt, repo["id"], repo["name"]))
#         repo["score"] = 0
#         repo["comment"] = "미제출"
#         os.chdir(rootdir)
#         save_result(output_filename, scorelist)
#         continue

#     print("=" * 50)
#     print("{} evaluating {} / {}".format(
#         prompt, repo["id"], repo["name"]))

#     basedir = repo["root"]
#     os.chdir(basedir)
#     try:
#         os.system("cmake . -Bbuild")
#         os.system("cmake --build build --config Debug -- /m:12")
#         # find executabe file
#         exename = list(filter(lambda x: ".exe" in x, list(os.listdir(os.path.join("build", "Debug")))))[0]
#         exepath = os.path.join(".", "build", "Debug", exename)
        
#         # test code
#         os.system("{}".format(exepath))
#         # os.system("{} 0.6 0.5 32".format(exepath))
#         # os.system("{} 0.8 0.4 32 45 135".format(exepath))
#         # os.system("{} 0.8 0.7 32 0 270".format(exepath))
#         # os.system("{} 0.5 0.25 64 0 360 0 1 0".format(exepath))
#         # os.system("{} 0.6 0.3 64 120 240 1 0.5 0".format(exepath))
#     except Exception as e:
#         print("error occurred:", e)
#     finally:
#         os.chdir(rootdir)

#     print("{} evaluating {} / {}".format(
#         prompt, repo["id"], repo["name"]))
#     print("please set score (0-10):")
#     repo["score"] = int(sys.stdin.readline())
#     print("please leave comment:")
#     repo["comment"] = sys.stdin.readline().strip()

#     save_result(output_filename, scorelist)
