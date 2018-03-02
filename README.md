# Wechat Jump Game Tool
- This project based on Qt and opencv.
- This project only tested on Xiaomi4 phone and the screenshot size is 1920*1080.
## Introduction: ##

This game is very hot and I try my best to make a tool with the help of other's idea, but it's still hard for me to do this.      
Now I finished basic funciton such as fix parameters and manual distance measurement function. The relation between distance and key press time is complex, the image resolution may have some influence on it. I will optimize it when I'm free.

这个东西最近好火，跟着别人的思路用C++试了试，感觉好难。       
目前只完成了调系数和手动测距功能，距离和鼠标按下时间的关系可能没那么简单，和手机分辨率也有关系。有时间试下能不能自动化。

## Auto Jump Mode High Score: ##
- 2018.02.17: 884 [Image](https://github.com/WangHongshuo/Readme_Images_Repository/blob/master/wechat_jump_game-Qt-opencv/HighScoreImage.png)
- 2018.02.12: 511 
- 2018.02.08: 439 
- 2018.02.07: 307 

## How to use: ##

1. Turn on USB debugging and allow simulating input via USB debugging.
1. Connect to PC, open this program and make sure the Adb status is Find: XXXX(Your device name).
1. Open the Jump Game, click Get Screenshot (shortcut key G) and mark the point in screenshot, fix jump parameter, click Jump (shortcut key J).
1. Get the screenshot manually when the game screen is stable, mark the point and jump.       
>          

1. 打开手机USB调试并允许通过USB调试修改权限或模拟点击。           
2. 连接电脑，打开程序并确定Adb状态为Find: XXXX(设备名称）。     
3. 打开游戏界面，手动Get Screenshot（快捷键G）和在图上标点，调整弹跳系数，点击Jump(快捷键J)。      
4. 稳定后再手动获取截图标点弹跳。      
                
## Screenshot: ##
![](https://github.com/WangHongshuo/Readme_Images_Repository/blob/master/wechat_jump_game-Qt-opencv/readme.png)      
![](https://github.com/WangHongshuo/Readme_Images_Repository/blob/master/wechat_jump_game-Qt-opencv/readme.gif)    
## Change Log: ##
- 2018.03.02:

Added cheat mode and added a flexible parameter to fix the jump parameter.

添加作弊模式躲避作弊检测，添加修正参数去修正跳跃参数。 

- 2018.03.01:

Separated adb part to a controller and fixed a bug in JumpJump class.

将Adb部分独立来成为一个控制器，修复了JumpJump类中的一个错误。

- 2018.02.12:

Improved locating the block corners and the block center function. This method is more radical and more accurate
than the last version, but may lead to bigger error in some extreme case. (

改进了检测角点和确定中心点的函数，改进后的方法更为激进，对圆台类块中心定位更加准确，但对其他个别情况会造成较大误差。 (

- 2018.02.09:

updated JumpJump::getBlockCenterPos and fixed crash bug when click Start(S) or Stop(S) pushbutton too frequent.

更新了寻找方块中心的方法，修复了频繁点击开始（停止）按钮导致程序崩溃的问题。

- 2018.02.08:

Fixed bug in locating block when the distance between man and block is too close.

修复了当小人和方块距离太近导致方块定位不准的问题。

- 2018.02.05:

Finished Auto Jump function, but it performed bad. The ui was not designed. 

完成自动跳功能，未优化，有很大概率会跳死，ui未设计。

- 2018.02.04:

Added detect target block corners function and fixed the problem in locating the man in the last version.

添加检测目标方块角点的功能,并修复了上一版本小人定位不准的问题。

- 2018.01.13:

Finished rewriting this. And now it can locate the position of man, but the position is not always right. This version is more accurate than the last one at least.      
  
完成了重写，可以定位小人，有时会不准，但比上一版本要精确。            

- 2018.01.09:     
 
Set `ImageWidget` and `mat_qimage_convert` to submodule.        
 
将 `ImageWidget` 和 `mat_qimage_convert` 设置为子模块。       

- 2018.01.07:     

Optimized get screenshot function. Now it only needs one adb command instead of two adb commands to get screenshot. It uses `adb shell screencap -p`  to get PNG image stdout and then catch the stdout, convert the binary data to QImage or Mat. It will not create image files in the phone or the PC disk (maybe create image files in the phone, but I can't find it), reduce I/O operation. And now it will takes 900ms to get screenshot in the PC memory, the last version will take 2000ms.

优化了获取手机截图的方式，由原来的执行两条Adb命令（截图到手机和从手机获取图片到本地磁盘）,改为只执行一条Adb命令，获取adb shell screencap -p 命令的stdout输出，转化为QImage或Mat，不再本地磁盘生成图片文件，减少I/O。获取同样的截屏，由原来的2000ms左右减少到900ms左右。

- 2018.01.05 #2:     

Used Adb to control the phone, the screenshot was obtained by the third-party software, because the screenshot from Adb is original image (5.8Mb). Fixed bugs when get the window title. 

改为用Adb去控制手机，屏幕图像还是用第三方软件获取，因为Adb获取的截图是没有压缩的，一张截图5M。修复了获取窗口标题的错误。

- 2018.01.05 #1:

It's not a good idea to control and get screen image by the third-party software, I should use Adb to do that. Rewriting...

用第三方软件获取手机屏幕图像和操控手机不如自己调用Adb，推倒重写。

- 2018.01.04 #2:     

Rewrote get window handle funciton and now you can get the window handle and name by moving your mouse to point the window.     

重写了获取窗口句柄部分，改为获取鼠标所指窗口的句柄和名称。     

- 2018.01.04 #1:        
   
Prepare to rebuild. The way to get HWND is too stupid.     

准备推倒重做，获取目标窗口的方式太煞笔。     

- 2018.01.03:      

Finished basic function.(Just add distance measurement function in ImageWidget class)

完成基本功能，只是把ImageWidget进行了修改，加入了测距功能。