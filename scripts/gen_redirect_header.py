import os
import shutil
import traceback

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

def clean_dir(incroot):
    # 转发头文件是平铺生成的（include/<模块>/<文件名>.hpp），
    # 模块目录下出现的子目录都是历史残留，直接删掉；空目录也一并清理
    for dirname in os.listdir(incroot):
        modpath = os.path.join(incroot, dirname)
        # 跳过 ast/ 目录，该目录由 gen_ast_forward.py 管理
        if not os.path.isdir(modpath) or dirname == "ast":
            continue
        for sub in os.listdir(modpath):
            subpath = os.path.join(modpath, sub)
            if os.path.isdir(subpath):
                print("remove", subpath)
                shutil.rmtree(subpath)
        if not os.listdir(modpath):
            print("remove", modpath)
            os.rmdir(modpath)

def clean_header(incroot):
    for root, dirnames, filenames in os.walk(incroot):
        # 跳过 ast/ 目录，该目录由 gen_ast_forward.py 管理
        if "ast" in dirnames:
            dirnames.remove("ast")
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
            if len(res) == 2 or len(res) == 3 and res[2] in ["\n", ""]:
                relpath = res[1]
                dstpath = os.path.join(root, relpath)
                if not os.path.exists(dstpath):
                    print("remove", filepath)
                    os.remove(filepath)
                elif os.path.basename(relpath) != file:
                    # 目标文件名与自身文件名不一致，说明是文件重命名后残留的别名头文件
                    print("remove", filepath, "->", relpath)
                    os.remove(filepath)

if __name__ == "__main__":
    try:
        scriptpath = os.path.realpath(__file__)
        rootdir = os.path.dirname(scriptpath)
        rootdir = os.path.dirname(rootdir)
        print("rootdir", rootdir) 
        os.chdir(rootdir)
        incroot = os.path.join(rootdir, "include")
        clean_dir(incroot)
        clean_header(incroot)
        for module in ["src"]:
            srcroot = os.path.join(rootdir, module)
            gen_module_header(srcroot, incroot)
    except:
        traceback.print_exc()



