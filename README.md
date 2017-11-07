# Arduino Bike Game
[Watch the video demo here](https://youtu.be/r04d039daLk)

My 2015 [High School](http://www.scienceandmathacademy.com/) Capstone Project with mentorship from Christopher Rogers.
<img src="/photos/main_view.jpg?raw=true" alt="main view" width="350"> <img src="/photos/pcb1.jpg?raw=true" alt="main view" width="200"> 

### PURPOSE:
To enable a stationary bike to control computer games so that a biker can race in a game while exercising.

### HOW IT WORKS:
The Arduino Leonardo is plugged into a computer where it emulates a keyboard and constantly measures the bike's speed (RPM) and checks that state of the steering.  In the game, the vehicle’s speed and steering are varied by rapidly modulating the arrow key presses sent to the computer by the Leonardo.
For example: if the bike is above the goal RPM (defined as a constant in the code) then it will effectively press the up arrow continously; and at 50% of the goal speed the up arrow will be pressed 50% of the time on average by the Leonardo.

### RESULTS:
* The system is able to control any racing computer game that can be controlled by arrow keys.
* The faster the biker pedals, the faster the race car will go.
* The sharper the biker turns, the sharper the rac car will turn.
* Read the [project poster](https://github.com/danielengbert/bike-game/blob/master/project_poster.pdf) and watch the [video demo](https://youtu.be/r04d039daLk).

### REQUIREMENTS:
* Arduino Leonardo
* custom printed circuit board (see EAGLE CAD files in the PCB folder)
  * and a few resistors and cables
* hall effect sensor
* neodymium magnet
* 1 IR emmiter and 2 IR receivers (serve as a photointerrupter for the steering)
* lazy susan (for the bike's front tire)
* bike and stationary bike stand

<img src="/photos/speed_sensor.jpg?raw=true" alt="speed sensor" width="200"> <img src="/photos/steering_sensor.jpg?raw=true" alt="steering sensor" width="200"> <img src="/photos/pcb2.jpg?raw=true" alt="main view" height="150"> 

### NOTE:
The next phase of this project would be to add capacitive touch sensor to the bike's handlebar for more game control (like activating turbo) and also to add a fan that blows air on the biker at a strength relative to their bike speed.  Preliminary development of this portion of the project was started and built into the PCB.
