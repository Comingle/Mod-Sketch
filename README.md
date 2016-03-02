Mod Software v0.2.5
=====

This is the sketch that runs whenever you turn your Mod on. Feel free to use it as a starting point for you own, or simply to restore your toy back to its original state.

This sketch sets up a serial console for you to interact with your toy without having to upload a new sketch. Once you connect to your toy over a serial connection (within the Arduino IDE, Hyperterminal, zterm, etc), it will accept several commands. Separate commands from their argument by a single comma:

* `l,x` to set the LED

To set the LED to a power of 200:
```
l,200
```

To turn the LED off:
```
l,0
```

* `0,x`,`1,x`,`2,x`,`-1,x`,... to set a particular output (motor)

To turn all outputs on to 150:
```
-1,150
```

To turn output 2 to 220:
```
2,220
```

* `c` to cycle through the patterns. This will return the current pattern number.
* `g` to get the current pattern number.
* `s` to stop all patterns and turn all outputs off and sets Powerlevel to 1.0 and Time to 1.0
* `r,x` to run a particular pattern.

If the command begins with `{`, you can control all motors in a single command:
```
{25, 50, 100}
```
Will set motor 0 to 25, motor 1 to 50, and motor 2 to 100. Note that there is no time component to this command; it will run until something interrupts it, such as a `s`top command, or another `{...` command.

Run the first pattern:
```
r,0
```

You can adjust power and time settings with `p`,`P`,`t`, and `T`. All power and time adjustment commands will return the current power or time scale, with 1.0 being normal. A power scale of 1.5 means all patterns will run at 50% higher power than normal. A time scale of 0.75 means that all patterns will run 25% faster than normal.

* `p` to decrease power.
* `P` to increase power.
* `t` to decrease pattern step time (run faster).
* `T` to increase pattern step time.

Read an input:
```
i,0
```

The `i` command reads a given input channel on your Mod and prints that value to the console. Mods have two inputs, so `i,0` and `i,1` are the two options for now.

New from v0.2.2:
* `G` get Powerlevel, returns Powerlevel without changing it first
* `v` get Version Information, returns for example "Mod-Sketch v0.2.2"

New from v0.2.4:
* `U ...` Play the User transmitted pattern, for example:
U 3,255 0 0 500, 0 0 255 500, 0 255 0 500
3 Entries
First Motor 255, Second 0, Third 0, for 500ms
First Motor 0, Second 0, Third 255, for 500ms
First Motor 0, Second 255, Third 0, for 500ms

allStop Pattern added

New from v0.2.5:
* `u` Returns current Userpattern
