# MARS Object Processing
This repository shows a simple example of how MARS object Processing Works

Unlike MARS Engine this code is single threaded to prioritize the optimization of how layers are processed

## Base time vs Tick time vs Reallocate Tick time
- Base Tick - The base tick represents what is the minimum time required to perform the tasks
- Tick time - The tick time it takes the engine to perform the tasks + the overhead of the engine itself
- Reallocate Tick time - The reallocate tick time represents the time a tick time takes + the overhead of creating and deleting one object

## Version Speed
<table>
    <thead>
        <tr>
            <th>Version</th>
            <th colspan="2">Tick</th>
            <th colspan="2">Base</th>
            <th colspan="2">Alloc Tick</th>
            <th>CPU</th>
            <th>Link</th>
        </tr>
        <tr>
            <th></th>
            <th>Debug</th>
            <th>Release</th>
            <th>Debug</th>
            <th>Release</th>
            <th>Debug</th>
            <th>Release</th>
            <th></th>
            <th></th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>2</td>
            <td>2.25ms-2.52ms</td>
            <td>0.58ms-0.60ms</td>
            <td>0.57ms</td>
            <td>0.43ms</td>
            <td>11.5ms-12.5ms</td>
            <td>3.2ms-3.57ms</td>
            <td>AMD Ryzen 73700x 4.050GHz</td>
            <td></td>
        </tr>
    </tbody>
</table>


## Legacy Version Speed
| Version | Tick Debug    | Tick Release    | Base Debug | Base release | CPU                       | Link                                                                                                               |
|---------|---------------|-----------------|------------|--------------|---------------------------|--------------------------------------------------------------------------------------------------------------------|
| 1       | 4.8ms - 5.0ms | 0.73ms - 0.75ms | 0.56ms     | 0.43ms       | AMD Ryzen 73700x 4.050GHz | [178488f](https://github.com/MARS-Engine/MARS-Object-Processing/commit/178488ffa588caf3d206f33ede2feebcf802d057)   |
| 1.1     | 2.0ms - 2.3ms | 0.60ms - 0.66ms | 0.56ms     | 0.43ms       | AMD Ryzen 73700x 4.050GHz | [f6cb575](https://github.com/MARS-Engine/MARS-Object-Processing/commit/f6cb5755a2e8ab5c6d421aa9a7df60a61217bc17)   |