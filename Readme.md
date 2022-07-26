# TRE
A text editor designed to be good.

## Design goals
* Extensible using unix sockets
* Minimal defaults
* Minimal dependencies. (Will probably be just curses. Which is fine because its required by POSIX).
* Portable. No terrible GLibc exensions. If your system is strictly POSIX-compliant it should run.

## Every text editor sucks

They all have too many commands for one person to reasonable know them all

Vim plugins require you to install entire languages for the simplest of plugins

## How Extensions work

Extensions will be their own process ( can be a simple as a one time curl call)

An extension equivalent to NERDTREE could be implemented in its own process running alongside TRE in a terminal multiplexer.

You could build out an entire IDE using TMUX with TRE, GDB, a file manager and a shell

Or you can just edit a simple text file by just running ``tre file``

Simple extensions like adding a command can be done by calling curl on the socket at startup

## What TRE Isn't

### Scriptable

There are plenty of scriptable text editors out there. The extensions system is designed to be rigid but useful. There are plenty of scriptable editors out there. If you really want to edit the source.

### Compatible with windows

I mean yeah.

### Ever going to be completed

I hope this isn't true. But we'll see.

## BUGS
Cannot exit without saving

Cannot scroll

## TODO

Add Command system

Build out sockets system

Build out tooltip system

Figure out how text selection should be handled

Fix bugs

## License

I dunno yet. Probably something BSD-compatible.
