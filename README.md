# E-Puck Finite State Machine Controller

This is a library for relaying finite state automata controllers from an e-puck Linux hat found at the Bristol Robotics Laboratory down to the underlying e-puck robot. 

## Table of Contents

1. [Features](#features)
2. [Prerequisites](#prerequisites)
3. [Installation](#installation)
4. [Usage](#usage)
5. [Known Issues](#known-issues)
6. [License](#license)
7. [Acknowledgments](#acknowledgments)

## Features

The library provides the following features:

- A simple and easy-to-use library for controlling e-pucks with a simple C code for finite state machines.
- An e-puck struct for object-oriented style programming.
- Various assets from the Bristol Robotics Lab for GPS tracking from a Vicon system, as well as an SPI comms system to get data from a Linux hat to the e-puck microcontroller.

## Prerequisites

The hardware required includes a Bristol Robotics Lab e-puck Linux hat and an e-puck robot from GCtronic.

## Installation

The project includes a Makefile. To compile the project, you simply need to run the target `rwalk`:

```bash
make rwalk
```

## Usage

To run the program on an e-puck Linux hat:

```bash
./rwalk
```

## Known Issues

This project uses a lot of resources on the e-puck side.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

This project uses various assets from the Bristol Robotics Lab.

## Notice

This software will not be maintained or improved and is only left here for posterity.

