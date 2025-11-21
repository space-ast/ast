import os

def gen_header(srcdir, incdir):
    if not os.path.exists(incdir):
        os.makedirs(incdir)
    for root, dirnames, filenames in os.walk(srcdir):
        if "libf2c/libf2c" in root or "libf2c\\libf2c" in root:
            continue
        for file in filenames:
            if file in ["ctype.h"]:
                continue
            if not file.endswith(".h") and not file.endswith(".hpp") and not file.endswith(".inl"):
                continue
            dstpath = os.path.join(root, file)
            headerpath = os.path.join(incdir, file)
            relpath = os.path.relpath(dstpath, incdir).replace("\\", "/")
            content = f'#include "{relpath}"\n'
            with open(headerpath, "w") as f:
                f.write(content)

def gen_module_header(srcroot, incroot):
    for dirname in os.listdir(srcroot):
        if not os.path.isdir(os.path.join(srcroot, dirname)):
            continue
        srcdir = os.path.join(srcroot, dirname)
        incdir = os.path.join(incroot, dirname)
        gen_header(srcdir, incdir)

def clean_header(incroot):
    for root, dirnames, filenames in os.walk(incroot):
        for file in filenames:
            if not file.endswith(".h") and not file.endswith(".hpp"):
                continue   
            filepath = os.path.join(root, file)
            try:
                with open(filepath, "r") as f:
                    content = f.read()
            except:
                with open(filepath, "r", encoding="utf8") as f:
                    content = f.read()
            lines = len(content.split("\n"))
            if lines>2:
                continue
            res = content.split('"')
            if len(res) == 2:
                relpath = [1]
                dstpath = os.path.join(root, relpath)
                if not os.path.exists(dstpath):
                    os.remove(filepath)

if __name__ == "__main__":
    
    rootdir = os.path.dirname(__file__)
    rootdir = os.path.dirname(rootdir)
    print("rootdir", rootdir) 
    os.chdir(rootdir)
    incroot = os.path.join(rootdir, "include")
    clean_header(incroot)
    for module in ["src"]:
        srcroot = os.path.join(rootdir, module)
        gen_module_header(srcroot, incroot)



