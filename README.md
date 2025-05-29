# SimEASY

## Table of Contents
* [Introduction](#introduction)
* [Technologies](#technologies)
* [Setup](#setup)
* [Running the code](#running-the-code)

## Introduction
Simulation of the EASY experiment setup and nuclear reaction.

## Technologies
Using Geant4 version 11.3 and ROOT version 6.34

## Setup
git clone the project into a directory of choice
In the main directory (taken here as SimEASY):
```
$ mkdir build && cd build
$ cmake -DGeant4_DIR=/path/to/geant4_v11_3_install_dir/lib/Geant4-11.3.0 /path/to/SimEASY
$ make (-j4)
```

## Running the code
Once built, the simulation can be run in visualisation mode from the build directory as:
` $ ./simEASY nomac "levels_file" #threads `
or in batch mode as:
` $ ./simEASY "run_macro" "levels_file" #threads `
