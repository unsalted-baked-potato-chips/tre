# TRE
A text editor designed to be good.

## Design goals
* Extensible using unix sockets
* Minimal defaults

## Every text editor sucks

They all have too many commands for one person to reasonable know them all

Vim plugins require you to install entire languages for the simplest of plugins

## How Extensions work

Extensions will be their own process ( can be a simple as a one time curl call)

An extension equivalent to NERDTREE could be implemented in its own process running alongside TRE in a terminal multiplexer.

You could build out an entire IDE using TMUX with TRE, GDB, a file manager and a shell

Or you can just edit a simple text file by just running ``tre file``

Simple extensions like adding a command can be done by calling on the socket at startup

## BUGS

Cannot load non nl-terminated files

Cannot exit without saving

Cannot delete lines

## TODO

Add Command system

Build out sockets system

Build out tooltip system

Fix bugs

## License

I dunno yet. Probably something BSD-compatible.
