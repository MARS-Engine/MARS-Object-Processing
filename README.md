# MARS Object Processing
This repository shows a simple example of how MARS object Processing Works

Unlike MARS Engine this code is single threaded to prioritize the optimization of how layers are processed

## Base time vs Tick time vs Reallocate Tick time
- Base Tick - The base tick represents what is the minimum time required to perform the tasks
- Tick time - The tick time is the time it takes the engine to perform the tasks + the overhead of the engine itself
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
            <td>1</td>
            <td>4.80ms - 5.00ms</td>
            <td>0.73ms - 0.75ms</td>
            <td>0.56ms</td>
            <td>0.43ms</td>
            <td></td>
            <td></td>
            <td>AMD Ryzen 73700x 4.050GHz</td>
            <td><a href="https://github.com/MARS-Engine/MARS-Object-Processing/commit/178488ffa588caf3d206f33ede2feebcf802d057">178488f</a></td>
        </tr>
        <tr>
            <td>1.1</td>
            <td>2.00ms-2.30ms</td>
            <td>0.60ms-0.66ms</td>
            <td>0.56ms</td>
            <td>0.43ms</td>
            <td></td>
            <td></td>
            <td>AMD Ryzen 73700x 4.050GHz</td>
            <td><a href="https://github.com/MARS-Engine/MARS-Object-Processing/commit/f6cb5755a2e8ab5c6d421aa9a7df60a61217bc17">f6cb575</a></td>
        </tr>
        <tr>
            <td>2</td>
            <td>2.25ms-2.52ms</td>
            <td>0.58ms-0.60ms</td>
            <td>0.57ms</td>
            <td>0.43ms</td>
            <td>11.5ms-12.5ms</td>
            <td>3.20ms-3.57ms</td>
            <td>AMD Ryzen 73700x 4.050GHz</td>
            <td><a href="https://github.com/MARS-Engine/MARS-Object-Processing/commit/3dd5cb59966a54c47bc807928c1491b02271b176">3dd5cb5</a></td>
        </tr>
        <tr>
            <td>2.1</td>
            <td>1.93ms-1.98ms</td>
            <td>0.54ms-0.54ms</td>
            <td>0.57ms</td>
            <td>0.43ms</td>
            <td>3.74ms-3.94ms</td>
            <td>0.75ms-0.76ms</td>
            <td>AMD Ryzen 73700x 4.050GHz</td>
            <td><a href="https://github.com/MARS-Engine/MARS-Object-Processing/commit/fb03762a844cc24c4740499edfaf11d7ad745c6e">fb03762</a></td>
        </tr>
    </tbody>
</table>