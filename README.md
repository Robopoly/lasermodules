# Laser barrier modules

	Optical robot detectors for use on race tracks and other projects

## What it's about

This code is intended to operate up to *four* lasers and photodetectors which
will function as optical barriers to detect the presence of a robot, when the
beam is interrupted. The beams are modulated at around 1kHz, to allow for some
robustness: it is not possible to fool the module by firing a plain laser
pointer into it, and the hardware can easily be designed to incorporate a
high-pass filter to remove ambient light.

## Input / Output

The module takes one input (`Enable`).
Up to four lasers (or IR LEDs) and four photodetectors can be connected at any
time.
The module outputs four logic signals, corresponding to the state of each beam.
