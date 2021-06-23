# Final Project Introduction
  
    由於openMV損壞，因此題目更改為主要使用PING辨識特殊幾何形狀物件，並結合Hw4的part1去完成一個完整的題目。
  
# BBCar Task

    透過BBCar原地擺頭使PING測量距離變化去偵測物體。
    根據每個辨識出的物體，各自將指派BBCar不同的Task。
    Task1: 將指示BBCar向左轉。
    Task2: 將指示BBCar向右轉。
    Task3: 將指示BBCar完成parking任務。
    做完上述Task，即達成此次目標。
  
# Process

    首先，按下"up"按鍵即可透過XBee傳送RPC command開始進行任務。
    起點：在一開始時，BBCar設定為不斷向前行的狀態。
    途中：每當前方遇到物體，就會停下偵測該物體，並完成其指示的Task。
    終點：當抵達parking lot，即任務完成。
    
# Scene Design
![image](https://user-images.githubusercontent.com/59250738/123070848-b5edec80-d446-11eb-9c80-e70245d25711.png)

# Demo Video

https://drive.google.com/file/d/1pLCnNxVP25iRcYhwb7vX62m3u5BB6FYx/view?usp=sharing


  
  
  
