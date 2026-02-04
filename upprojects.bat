chcp 65001 > nul 2>&1
setlocal enabledelayedexpansion
echo 添加文件到暂存区...
git add .
echo 提交代码到本地仓库...
git commit -m "返回手动初始化并且将触摸按键案列数值不稳定修复。"
:null->项目更新注释
git branch -M main
git push -u origin main
pause

