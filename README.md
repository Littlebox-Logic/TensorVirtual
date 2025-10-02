# Tensor Virtual
### Build-0.1.0.0 (Pre-alpha)
---
*Tensor Virtual* 是一个**支持断点调试**的简易 Intel 8086 架构虚拟机程序.

这是一个**学生实验项目**，~~受部分静态库限制，目前仅可在 Linux 发行版上编译运行~~. 目前该项目还在基础开发阶段，欢迎各位学习交流.

源代码 (Makefile 除外) 在 `src` 目录;
编译中间文件在 `obj` 目录;
可执行文件在 `bin` 目录.
`boot.img` 是一个引导识别测试文件，由 `src/boot.asm` 编译得到，无其他作用.

编译时需要保证已安装 readline-devel 库（GNU Readline）并确定已安装支持 GNU C 23 标准的 gcc 编译器，如果无法满足这一点，请在保证可使用 GNU C 的情况下自行修改 `Makefile`.

试运行请在项目根目录使用命令：

```bin/tensor -```

并键入：
    `boot`: 启动 MBR 执行
    `reg` : 获取寄存器数据
    `clear`: 清空屏幕
    `exit`: 退出程序
    `c`: 从断点处继续运行
    `instr`: 获取当前指令的地址和内容

版本和帮助信息请使用命令：

```bin/tensor --version```
```bin/tensor --help```

如需运行时关闭日志输出，请使用命令：

```bin/tensor --quiet```

EOF
