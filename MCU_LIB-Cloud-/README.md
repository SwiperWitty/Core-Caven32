# MCU_LIB-Cloud-

如果你看到这里，那么你大概率是开始看不懂代码了...

![image-20220705092503090](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220705092503090.png)

### LIB（Cloud）

这是 [开放版本](https://github.com/SwiperWitty/API_Cloud) ，随便用，不要钱。

![image-20220714193058659](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220714193058659.png)

_____

**文件目录**

#### -API（应用层）

#### -Base（接口层+底层）

#### -Mode（应用层、驱动层）

![image-20220714192130719](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220714192130719.png)

Mode目录内的文件就是功能单元，它们都属于应用层。

当然API也是应用层的文件（应用层调另一个应用层）。

#### Magic-API (隐藏副本)



这个不对外开放(⊙o⊙)！

____

### 重要说明

这个仓库一般都作为子模块出现在其他项目工程中，所以使用本仓库要对 git submodule 操作十分熟悉。

例如：

##### 仓库没有子模块，需要new一个

~~~
git submodule add -b main git@github.com:SwiperWitty/API_Magic.git	
# git@github.com:SwiperWitty/API_Cloud.git （这是示例）
# -b 参数，使得母项目追踪子项目的指定branch（否则默认不追踪），不然会建立 HEAD detached at
#原型 git submodule add -b <branch> <repository> [<submodule-path>]
~~~

##### 仓库已经有子模块的链接了，但是没有数据内容

~~~
#子文件目录下
git submodule init
git submodule update --remote

#直接这样也可以：
git submodule update --init --remote
二选一
git submodule update --init --recursive		#子模块操作（递归更新）
~~~

这还没完

##### 每次获取之后，记得切换到主分支

~~~
#查看分支
git branch
#如果不是主分支，那么就：
git checkout main
~~~

##### 如果你修改了子模块内容，然后同时希望add到这个库

~~~
#在子模块目录下 
git add .
git commit -m "up"
git push
#再去上一层模块下 重复上述操作
~~~

____

##### 如果你想删除子模块

1. 首先删除子模块文件

   然后，找到根目录（就是工程名的那个目录）

2.  `.gitmodules`【文件】中相关子模块信息，打开它（以vscode打开）

~~~
删除	[submodule "xxx"] 到 https://github.com/xxx
子模块名称相关描述
~~~

3. `.git/config` 【文件】中相关子模块信息（.git文件平时是隐藏文件）

~~~
删除	[submodule "xxx"] 到 https://github.com/xxx
子模块名称相关描述
~~~

4. `.git/modules` 【文件夹】中的子模块文件夹，需要删除

对以上内容有疑问可以去看：[子模块git submodule基础知识及易踩的坑](https://blog.csdn.net/CcA_Lin/article/details/106684710?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2~default~CTRLIST~default-1-106684710-blog-89702932.pc_relevant_aa&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2~default~CTRLIST~default-1-106684710-blog-89702932.pc_relevant_aa&utm_relevant_index=1) 



蟹鸣：[test_word ](https://github.com/SwiperWitty/test_word) 

GitHub： [Magic](https://github.com/SwiperWitty/API_Magic) 

一般作用在：[Caven-32](https://github.com/SwiperWitty/Core-Caven32)

