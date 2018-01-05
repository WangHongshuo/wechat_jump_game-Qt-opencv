# wechat jump game tool
- This project based on Qt, opencv and window api.
## Introduction: ##

This game is very hot and I try my best to make a tool with the help of other's idea, but it's still hard for me to do this.      
Now I finished basic funciton such as fix parameters and manual distance measurement function. The relation between distance and key press time is complex, the image resolution may have some influence on it. I will optimize it when I'm free.

这个东西最近好火，跟着别人的思路用C++试了试，感觉好难。       
目前只完成了调系数和手动测距功能，距离和鼠标按下时间的关系可能没那么简单，和手机分辨率也有关系。有时间试下能不能自动化。

Click Get Window and move your mouse to point the target window, press S to stop finding window handle when get target window handle.     

点击Get Window将鼠标指向目标窗口，获取目标窗口的图像和句柄后按S键停止寻找鼠标所指窗口句柄。           
## Screenshot: ##
![](https://github.com/WangHongshuo/wechat_jump_game-Qt-opencv/blob/master/README/readme.png)
## Change Log: ##

- 2018.1.5 #2:     

Used Adb to control the phone, the screenshot was obtained by the third-party software, because the screenshot from Adb is original image (5.8Mb). Fixed bugs when get the window title. 

改为用Adb去控制手机，屏幕图像还是用第三方软件获取，因为Adb获取的截图是没有压缩的，一张截图5M。修复了获取窗口标题的错误。

- 2018.1.5 #1:

It's not a good idea to control and get screen image by the third-party software, I should use Adb to do that. Rewriting...

用第三方软件获取手机屏幕图像和操控手机不如自己调用Adb，推倒重写。

- 2018.1.4 #2:     

Rewrote get window handle funciton and now you can get the window handle and name by moving your mouse to point the window.     

重写了获取窗口句柄部分，改为获取鼠标所指窗口的句柄和名称。     

- 2018.1.4 #1:        
   
Prepare to rebuild. The way to get HWND is too stupid.     

准备推倒重做，获取目标窗口的方式太煞笔。     

- 2018.1.3:      

Finished basic function.(Just add distance measurement function in ImageWidget class)

完成基本功能，只是把ImageWidget进行了修改，加入了测距功能。