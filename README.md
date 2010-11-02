# Motor Music project

	Just because I'm a bit bored

## What it's about

The goal of this project is to play music on a pair of DC motors, using
hardware features of the standard AVR Mega microcontrollers.
This is mostly just for fun (and probably not something to try on expensive
motors :) ), but I'll try to make it an exercise in clean and modular
implementation.

## Structure

The available note values will be loaded from a table in Flash memory. The
tempo will be set by a single constant/variable. Melodies will be stored in a
table in the format [note, duration] where `note` corresponds to an entry in
the note table, and duration is a multiple of the tempo. Tempo thus corresponds
to the length of the shortest note.

If possible, note frequency will be determined by the TOP value for the given
channel's PWM module. Potential problems : prescaler and timer source are
shared between the two channels, which will reduce the resolution. If this is
too limiting, at least one of the channels will have to use a timer-driven
software PWM implementation. I'd rather keep the default hardware setup so that
it's easily transferred to existing hardware, rather than use the third PWM
channel (to have two different prescaler settings)

Note changes will be driven by a timer overflow. Timer overflow (or compare
match) is set to occur at the given tempo. On every match, compare number of
elapsed 'ticks' to note length in table. If it's time, load next note and
duration from table. Rinse, repeat.


## Future developments

If I don't get bored with this project before the end, I plan to add variable
speed control by varying the PWM duty cycle. Not sure how well this will work,
but it would definitely be more fun to have a robot draw figures on the floor
at the same time that the motors are playing music.

Another thing to test, which is almost certainly worse for the motors : toggle
the direction pin rather than the power pin, so the motor will remain (almost)
stationary, and probably sound louder.

