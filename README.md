# autoVersion
===============

vc 的rc文件自动修改工具 用来让 vs编译时自动生成版本号

# 版本号格式
 **[VER1.VER2.VER3.VER4] **  
 
	 **VER1 **保持原始值不变;  
	 **VER2 **调整为年份;  
	 **VER3 **调整为第几周;  
	 **VER4 **调整为该周的第几次编译  
# 使用方法
1. 把autoversion.exe复制到解决方案根目录
2. 在需要自动配置版本号的项目属性里设置 <生成前事件>命令行
```$(SolutionDir)autoVersion.exe  "$(ProjectDir)$(ProjectName).rc"```
3. 每次编译自动调整版本号
