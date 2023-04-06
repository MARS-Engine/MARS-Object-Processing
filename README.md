# MARS Object Processing
This repository shows a simple example of how MARS object Processing Works

Unlike MARS Engine this code is single threaded to prioritize the optimization of how layers are processed

## Base time vs Tick Time
When running the code the first printed message will be the base time, the base time is the time it takes to execute all tasks without any overhead.
A Base Func time is the same as a Base time but the task happens inside a function instead of a for loop.
A tick time is the time it takes to execute all tasks including the overhead of calling the functions in the MARS engine.

## Version Speed
| Version | Speed Debug   | Speed Release   | Base Debug | Base release | CPU                       | Link                                                                                                             |
|---------|---------------|-----------------|------------|--------------|---------------------------|------------------------------------------------------------------------------------------------------------------|
| 1       | 4.8ms - 5.0ms | 0.73ms - 0.75ms | 0.56ms     | 0.43ms       | AMD Ryzen 73700x 4.050GHz | [178488f](https://github.com/MARS-Engine/MARS-Object-Processing/commit/178488ffa588caf3d206f33ede2feebcf802d057) |
| 1.1     | 2.0ms - 2.3ms | 0.60ms - 0.66ms | 0.56ms     | 0.43ms       | AMD Ryzen 73700x 4.050GHz |                                                                                                                  |