# duo-lvgl-fb-demo

## 下载交叉编译工具链
下载工具链：

git clone https://github.com/milkv-duo/host-tools.git

也可以直接使用 Buildroot SDK 中 host-tools 目录，二者是一样的。

进入到工具链目录中 export 工具链到环境变量中：

cd host-tools
export PATH=$PATH:$(pwd)/gcc/riscv64-linux-musl-x86_64/bin

验证工具链是否可用：

riscv64-unknown-linux-musl-gcc -v

能够正常显示交叉编译工具链的版本信息，即工具链可用。

## 仓库
lv_port_linux_frame_buffer from:
```
git clone https://gitcode.com/lvgl/lv_port_linux_frame_buffer.git
cd lv_port_linux_frame_buffer
git submodule update --init --recursive
```

