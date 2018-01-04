# wechat jump game tool
- This project based on Qt, opencv and window api.
## Introduction: ##

This game is very hot and I try my best to make a tool with the help of other's idea, but it's still hard for me to do this.      
Now I finished basic funciton such as fix parameters and manual distance measurement function. The relation between distance and key press time is complex, the image resolution may have some influence on it. I will optimize it when I'm free.

这个东西最近好火，跟着别人的思路用C++试了试，感觉好难。       
目前只完成了调系数和手动测距功能，距离和鼠标按下时间的关系可能没那么简单，和手机分辨率也有关系。有时间试下能不能自动化。      
## Screenshot: ##
![](https://github.com/WangHongshuo/wechat_jump_game-Qt-opencv/blob/master/README/readme.png)
## Change Log: ##

- 2018.1.4:        
   
Prepare to rebuild. The way to get HWND is too stupid.     

准备推倒重做，获取目标窗口的方式太煞笔。     

- 2018.1.3:      

Finished basic function.(Just add distance measurement function in ImageWidget class)

完成基本功能，只是把ImageWidget进行了修改，加入了测距功能。