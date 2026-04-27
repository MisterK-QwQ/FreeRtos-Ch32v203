chcp 65001 > nul 2>&1
setlocal enabledelayedexpansion
echo 添加文件到暂存区...
git add .
echo 提交代码到本地仓库...
git commit -m "新增通用屏幕绘制库 和脏数据刷新 目前案列ST7789/135*240"
:null->项目更新注释
git branch -M main
git push -u origin main
pause

