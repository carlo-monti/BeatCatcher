# BeatCatcher
Beat tracker for Arduino compatible microcontrollers (i.e. Teensy / ESP32).

## About
This project is the porting of the **B-Keeper** beat-tracking algorithm to an Arduino compatible device. It basically takes two input signals from a drum-kit (kick and snare) and outputs a MIDI Clock synchronized with the groove played by the drummer. Once started, the algorithm does its best to stay in time with the drummer, given that there are no big tempo-changes and the time signature is 4/4.

## Input
The input signal came from two piezo sensors placed on kick and snare. The signal pass through a smoothing filter and a limiter (to 3.3V) and then go to the Analog In pin. <img width="839" alt="circuito" src="https://user-images.githubusercontent.com/79301372/181357966-652a7ec0-a377-424d-9434-f0e07ecfd4c4.png">

## Code structure
Basically there are five objects, whose work is coordinated by the main ''BeatCatcher.ino'' file. The **Click** object calculate and outputs the MIDI Clock signal. The **Onset** object samples the two inputs and detect a hit. The **Sync** object evaluates and mantain the sync and keeps track of the current layer (see the article). The **Tempo** object checks if there are significant tempo changes. The whose

## How to


## References
The original algorithm has been described in: Robertson, Andrew, and Mark D. Plumbley. “Synchronizing Sequencing Software to a Live Drummer.” Computer Music Journal, vol. 37, no. 2, 2013, pp. 46–60. (JSTOR, http://www.jstor.org/stable/24265466. Accessed 27 Jul. 2022.). 
