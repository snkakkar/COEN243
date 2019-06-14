# AgroTile

AgroTile is a smart IoT greenhouse monitoring system creating by Kevin Velcich, Matthew Findlay, Esai Morales, and Schachi Kakkar.
Within this folder, you will see the following files/directory:
  - `code/`
  - `demo.txt`
  - `sample.html`
  - `paper.pdf`
  - `slides.pdf`

## code/

The code directory contains all of our source code for the project. In order to run, you'll need to install "AgroTile" as a library on Arduino as well as a few additional libraries including: painlessMesh, ArduinoJson, TaskScheduler, and AsyncTCP. There was a slight modification done to the libraries and default partitioning. Check the comments of the source code for more information.

Within the code directory, there is an `applications/` directory and `src/` directory. `applications/` contains the source code for the actual device programs (the server and client nodes), whereas `src/` contains utility functions and variables used between both applications.

## demo.txt

The `demo.txt` file includes a link to a YouTube video showing a demonstration of our system in action. [Alternatively, use this link!](https://www.youtube.com/watch?v=J6kQxO07WdI)

## sample.html

This is a sample HTML document that we saved directly from the browser after using our system. View this web page to get a quick idea of how the user-facing visualizations would behave.

## paper.pdf

A formal write up of our project including our motivations, design and implementation, results, and conclusions.

## slides.pdf

A PDF of our slides from our presentation on the project.
