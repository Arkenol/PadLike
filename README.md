# **PadLike**
A simple texteditor case on QT.
![如图所示][https://github.com/Arkenol/PadLike/blob/master/test/001.png]
### 实现细节功能汇总
- [x] 正常的打开、保存、关闭文件
- [x] 常用的剪切、复制、粘贴、撤销、恢复文本功能
- [x] 菜单栏、工具栏、状态栏的设置，如Clipboard无内容时粘贴是灰的
- [x] 自动换行设置
- [x] 命令行打开程序，可识别file参数，-v -h
- [x] 视图的缩放(zoom in/out)，支持快捷键和滚轮
- [x] 全屏模式
- [x] 文件的拖拽处理
- [x] 显示最近打开的文件，最大显示数目为5
- [x] 退出前确认是否保存
- [x] 输出至PDF
- [x] 打印
- [x] 改动后的文本，窗口显示的文件名应该带*
- [x] 行号、列号、长度的状态显示
- [x] 缩进长度设置
- [x] 编码方式的显示，如window换行为\r\n，mac为\r，Linux为\n
- [x] 重写模式的切换，Insert/Overtype
- [x] 跳转至XXX行
- [x] 右键菜单栏的设置，包括剪切、复制、粘贴、删除、全选、撤销、恢复等基础操作
- [x] 实现查找功能，可以控制大小写，向上或者向下
- [x] 基于查找，实现了替换功能
- [x] 高亮光标所在行
- [x] 快速单行注释/反注释
- [x] 关键字高亮
- [x] 关键字补全
- [x] 显示行号区域
- [x] {}折叠

### 可能会做也可能不会做的
- [ ] 语法分析
- [ ] 生成标准函数模板
- [ ] 自动更新   :分析github仓库xml or json，对比版本号，然后下载，
- [ ] 多文档   ：QTabWidget或者QT MDI
- [ ] 侧边栏、缩略图   ：QListViewModel，QFileSystemModel，QTreeWidget显示文件树，QDockWidget包裹浮动；缩略图，......
- [ ] 编译    :QProcess 处理传递参数
- [ ] 可使用QScintilla库实现更多额外功能
- [ ] 其他乱七八糟没想到的东西
