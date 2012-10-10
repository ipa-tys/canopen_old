CANopen C++ library
===================
&copy; 2012 Fraunhofer Institute for Manufacturing Engineering and Automation (IPA)

Author: Tobias Sing, email: tobias.sing@ipa.fhg.de

See the file "license.txt" for copying permission.

Description:
------------
A C++ library/API for communication with CANopen devices, easy to
modify and extend for specific purposes.
The library can be used on three different levels of abstraction: 

* Low-level communication via individual CANopen messages (NMT, SDO, PDO, SYNC). SDO calls and device
replies appear to the API user as convenient function calls.
* Higher-level functions that implement functionality that requires a sequence of CANopen messages,
e.g. `canopen::setMotorState()`
* An object-oriented system of classes for devices and device groups which automates much of the 
communication with devices and keeps information about devices up-to-date by processing incoming
PDOs in a dedicated thread.

There is also a wrapper, [ros_canopen](https://github.com/ipa-tys/ros_canopen) available,
which allows using this library to control CANopen devices from within the ROS
(Robot Operating System) framework.

The [manual](https://github.com/ipa-tys/canopen/blob/master/doc/usermanual.pdf?raw=true)
covers both the canopen library as well as the wrapper ros_canopen.

Prerequisites:
--------------
* The only compatible CAN device driver so far is the
[Peak CAN driver](http://www.peak-system.com/fileadmin/media/linux/index.htm).
* A compiler with good support for C++11, e.g. gcc 4.6 or newer (the default compiler
in Ubuntu 11.04 or newer)

Feedback / Roadmap:
-------------------
The library is currently under development, but can already be used.
Don't hesitate to get back to us in case of questions.

To date, the library has only been tested with PRL, PRH, and ERB (Powerball) modules from
[Schunk](http://www.schunk.com). So far, only the interpolated position (IP) mode has been
implemented.

Note to manufacturers interested in a free and open-source API for their hardware:
We would appreciate devices (actors and sensors) for testing at any time.

Bugs and feature tracking:
* [Milestone 1](http://www.care-o-bot-research.org/trac/query?milestone=CanOpen+MS1) (alpha version)
* [Milestone 2](http://www.care-o-bot-research.org/trac/query?milestone=CanOpen+MS2) (beta version)




