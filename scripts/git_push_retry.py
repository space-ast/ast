#!/usr/bin/env python3
"""
Git Push 重试脚本
持续执行 git push 命令直到成功
"""

import subprocess
import time
import sys
import os

def git_push_with_retry():
    """
    持续尝试执行 git push 命令，直到成功为止
    """
    retry_count = 0
    max_retries = 100  # 防止无限循环，设置最大重试次数
    base_delay = 5     # 基础重试延迟（秒）
    
    print("🚀 开始执行 git push，将持续尝试直到成功...")
    print("按 Ctrl+C 可以终止程序")
    
    while retry_count < max_retries:
        try:
            retry_count += 1
            print(f"\n📝 第 {retry_count} 次尝试执行 git push...")
            
            # 执行 git push 命令
            result = subprocess.run(
                ['git', 'push'],
                capture_output=True,
                text=True,
                check=True
            )
            
            # 如果执行成功
            print("✅ git push 成功！")
            print("输出:", result.stdout)
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"❌ 第 {retry_count} 次 git push 失败")
            print("错误信息:", e.stderr.strip())
            
            # 计算退避延迟（指数退避策略）
            delay = min(base_delay * (2 ** (retry_count // 3)), 300)  # 最大延迟5分钟
            print(f"⏰ {delay} 秒后重试...")
            
            try:
                time.sleep(delay)
            except KeyboardInterrupt:
                print("\n\n⏹️  用户终止了程序")
                return False
                
        except KeyboardInterrupt:
            print("\n\n⏹️  用户终止了程序")
            return False
        except Exception as e:
            print(f"💥 发生未知错误: {e}")
            return False
    
    print(f"⚠️  已达到最大重试次数 ({max_retries})，停止尝试")
    return False

def check_git_repository():
    """
    检查当前目录是否是 git 仓库
    """
    try:
        subprocess.run(['git', 'status'], capture_output=True, check=True)
        return True
    except subprocess.CalledProcessError:
        return False

def main():
    # 检查是否是 git 仓库
    if not check_git_repository():
        print("❌ 当前目录不是 git 仓库！")
        sys.exit(1)
    
    # 显示当前分支信息
    try:
        branch_result = subprocess.run(
            ['git', 'branch', '--show-current'],
            capture_output=True, text=True, check=True
        )
        current_branch = branch_result.stdout.strip()
        print(f"🌿 当前分支: {current_branch}")
    except:
        print("⚠️  无法获取当前分支信息")
    
    # 执行 git push 重试
    success = git_push_with_retry()
    
    if success:
        print("\n🎉 任务完成！git push 成功执行")
    else:
        print("\n😞 任务未能完成")
        sys.exit(1)

if __name__ == "__main__":
    main()