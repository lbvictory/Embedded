## Git 快速上手（嵌入式专用）

你不需要精通Git，只需要记住 **6 个命令** 就够用。

---

## 一、最常用的 6 个命令

```bash
# 1. 克隆项目（第一次下载代码）
git clone <仓库地址>

# 2. 查看状态（改了什么文件）
git status

# 3. 添加文件（暂存）
git add .                    # 添加所有修改
git add main.c               # 添加单个文件

# 4. 提交（生成一个版本）
git commit -m "写了什么改动"

# 5. 推送（上传到远程仓库）
git push

# 6. 拉取（下载最新代码）
git pull
```

**你 90% 的时间只需要这 6 个命令。**

---

## 二、你的日常工作流

### 场景1：每天开始工作

```bash
git pull        # 先拉取队友的更新（自己一个人可省略）
```

### 场景2：写了一段代码，想保存

```bash
git status                # 看看改了哪些文件
git add .                 # 全部添加
git commit -m "完成了PWM驱动"   # 打个标记
```

### 场景3：下班前/完成一个重要功能

```bash
git add .
git commit -m "第一周完成：LED + 串口 + SysTick"
git push                  # 上传到仓库备份
```

---

## 三、三个概念（理解就行）

```
工作区 → 暂存区 → 仓库
(你的文件夹)  (git add)  (git commit)

你改代码 → git add → git commit → git push
           暂存      本地版本    远程备份
```

**不需要记术语，你只需要知道：**

- `git add` = 告诉Git"我要保存这些文件"
- `git commit` = 拍一张快照，打上标签
- `git push` = 把快照上传到云端备份

---

## 四、你当前项目的用法

```bash
# 进入你的项目目录
cd D:/code/STM32_2/TimeLedUart

# 初始化仓库（第一次需要）
git init

# 添加所有代码
git add .

# 提交第一版
git commit -m "第一周完成：LED闪烁+串口打印Alive+软件定时器"

# 如果想备份到GitHub（可选）
# 先在GitHub创建仓库，然后执行：
git remote add origin https://github.com/你的账号/TimeLedUart.git
git push -u origin master
```

---

## 五、常见问题解决

### 问题1：忘记 add 直接 commit

```bash
git add .
git commit -m "补上遗漏的文件"
```

### 问题2：commit 后想改注释

```bash
git commit --amend -m "新的注释"
```

### 问题3：不想提交某个文件

创建 `.gitignore` 文件：

```
*.o
*.elf
*.hex
build/
.DS_Store
```

### 问题4：查看历史版本

```bash
git log --oneline    # 简洁查看
git log              # 详细信息
```

### 问题5：改坏了想回退

```bash
git checkout -- main.c    # 撤销对main.c的修改
```

---

## 六、你的最小习惯

每天结束时执行：

```bash
git add .
git commit -m "今天完成：xxx"
```

**就这么简单。** 不需要分支、不需要回滚、不需要rebase。

有了本地commit，你就可以：

- 随时看自己写了什么
- 改坏了能找回
- 以后去面试能拿出GitHub记录

---

## 七、现在你可以做的事

```bash
cd D:/code/STM32_2/TimeLedUart

# 查看当前状态
git status

# 如果有红色文件，说明还没保存过
git add .
git commit -m "第一周工程化重构+软件定时器框架"
```


