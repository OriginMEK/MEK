# MEK git 常用命令

#git安装
sudo apt-get install git
git --version

#git配置
git config --global user.name "Your Name"
git config --global user.email "email@example.com"
git config --global alias.st status 
git config --global alias.last 'log -1'

#创建仓库
git init

#添加,提交
git add .
git commit -m "commit message"

#查看状态
git status
git diff 
git log 
git log -p <file>
git diff HEAD -- readme.txt //可以查看工作区和版本库里面最新版本的区别

#撤销更改
git checkout -- file  //丢弃工作区的修改：
git reset HEAD file   //把暂存区的修改撤销掉（unstage），重新放回工作区
//场景1：当你改乱了工作区某个文件的内容，想直接丢弃工作区的修改时，用命令git checkout -- file。
//场景2：当你不但改乱了工作区某个文件的内容，还添加到了暂存区时，想丢弃修改，分两步，第一步用命令git reset HEAD file，就回到了场景1，第二步按场景1操作。
//场景3：已经提交了不合适的修改到版本库时，想要撤销本次提交，参考版本回退一节，不过前提是没有推送到远程库。

#版本回退
git reset --hard HEAD^  // HEAD表示当前版本,上一个版本就是HEAD^，上上一个版本就是HEAD^^
git log //查看提交历史
git reflog //查看命令历史
git reset --hard c919ff //根据历史记录回退

#远程提交
git branch --set-upstream develop origin/develop //指定本地dev分支与远程origin/dev分支的链接
git pull
git pull origin develop
git push origin develop //推送本地develop分支到远端的origin
git push origin master  //推送本地master分支到远端的origin
git remote 
git remote -v 
#合并分支(将issue-001分支合并到develop分支：先切换分支，再合并，最后删除)
git checkout develop
git merge --no-ff -m "merge info" issue-001
git branch -d issue-001
#合并develop 分支到master主分支
git checkout master
git merge --no-ff -m "merge info" develop

git checkout -b dev //创建并切换分支
git checkout -b dev origin/dev //创建远程的分支到本地
#暂存
git stash 
git stash list //查看
git stash pop

#标签
git tag v1.0 //Mark the current commit with a tag
git tag
git tag v0.9 6224937 //给历史打标签
git tag -a v0.1 -m "version 0.1 released" 3628164
git show <tagname> //可以看到说明文字

#others
git rm file
git mv file1 file2
git pull --rebase origin master
git push -u origin master
git remote //查看远程库的信息
git remote -v
git commit --amend //改变最后一次提交 git rm  git add 后用
git blame <file> //Who changed what and when in <file>
rebase