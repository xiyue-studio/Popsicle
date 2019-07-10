#!/usr/bin/python
import platform, os, argparse, shutil

def system(cmd):
    if os.system(cmd) != 0:
        raise "Failed to execute cmd:", cmd

def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("--release", action="store_true", help = "release build")
    parser.add_argument("--clean", action="store_true", help = "clean tmp building files")

    args = parser.parse_args()

    if (args.clean):
        if (os.path.exists("tmp")):
            shutil.rmtree("tmp")
        if (os.path.exists("lib")):
            shutil.rmtree("lib")
        print "remove tmp building files"
        return
        
    if args.release:
        build_type = "-DCMAKE_BUILD_TYPE=Release"
    else:
        build_type = "-DCMAKE_BUILD_TYPE=Debug"

    if not os.path.isdir("tmp"):
        os.mkdir("tmp")
    os.chdir("tmp")

    if platform.system() == "Windows":
        system("cmake -G \"Visual Studio 12 Win64\" .. %s %s" % (build_type, "-DPLATFORM=win32"))
        system("cmake --build . --config Release")
    elif platform.system() == "Linux":
        system("cmake -G \"Unix Makefiles\" .. %s %s" % (build_type, "-DPLATFORM=linux_x64"))
        system("make")

if __name__ == '__main__':
    main()
