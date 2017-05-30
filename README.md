# Artemis
IoT malware that I'm writing for fun. 

## General About

**Warning:** This project isn't meant to be portable. It's meant to compile on my computer but maybe not yours. I may or may not make it portable later but well have to wait and see about that.

The name comes from the Athena malware which was a HTTP bot for windows.
Athena was the first botnet I ever "set up" and got me into malware and botnets.

It started when I decided I hated trying to understand and use other peoples bots so I fired up an IDE and started writing a bot for IRC.  
Then I decided I hated using the IDE and that IRC was annoying.  
Next I opened sublime text and started writing a HTTP bot.

Really though it takes quite a bit of time considering I'm no coding god.
You might also notice some really messy code and maybe some commits where I clean it up.
This is cause I didn't work on this for a bit and then I came back with a lot more skill than before.
Essentially you'll probably see my coding improve as I do it more.

## Server

### About

Uses a webserver since the bot is controlled via HTTP.
The server side is written in PHP using PDO connection to a mySQL database.

### Features

* **Bot Page** - A bot page with search engine.
* **Command page** - A command page that you can add command with and view currently unfinished commands.
* **User monitoring** - Monitor users last login, last IP, privilege, username, and password.

### Planned features

See the [Projects](https://github.com/Advnet/Artemis/projects) for this Repository.

## Client

### About
The client side is IoT because C is the language I know best although I'm attempting to learn C++ now.
Client side is inspired from both [BashLite](https://en.wikipedia.org/wiki/BASHLITE) and [Mirai](https://en.wikipedia.org/wiki/Mirai_(malware)).

### Features

* **Uhhh...** - The bot isn't really complete so it's pretty bare feature wise right now...

### Planned features

See the [Projects](https://github.com/Advnet/Artemis/projects) for this Repository.

## Credits

Okay so I'm 100% sure I'm missing a bunch because I wasn't documenting this until I put this on GitHub but I'm trying my best to give credit where it's due.

[GitHub](https://github.com) - For giving me these private repos.  
[How To Make A Basic Botnet - Code Overview](https://www.youtube.com/watch?v=n2pB-QQrfaE) [LinkCabin(Channel)](https://www.youtube.com/channel/UCv6i6WVf-KeUeXFmp9oy29w) - His HTTP panel was big help for me because I wasn't sure how the architecture of a HTTP botnet worked and how to use databases the best way.  
https://developer.mbed.org/handbook/C-Data-Types - Obvious.  
http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html - How to detach from terminal and signore signals.  
https://en.wikipedia.org/wiki/Unix_signal - Which signal mean and do what.  
http://stackoverflow.com/questions/10046916/is-it-possible-to-ignore-all-signals - How to block signals.  
http://man7.org/linux/man-pages/man7/signal.7.html - More info on signals  
[Hydra](http://www.xup.in/dl,20640997/hydra-2008.1.zip/) (2008) - Concept.  
[Mirai](https://github.com/jgamblin/Mirai-Source-Code) (2016) [Wiki](https://en.wikipedia.org/wiki/Mirai_(malware))- Concept and its scanner.  
[BASHLITE/Gafgyt/Lizkebab/Qbot/Torlus/LizardStresser](https://github.com/gh0std4ncer/lizkebab) (2014?) [Wiki](https://en.wikipedia.org/wiki/BASHLITE)- Concept and its scanner. 
http://www.binarytides.com/tcp-syn-portscan-in-c-with-linux-sockets/ - TCP portscanning in C.  
https://l3net.wordpress.com/2012/12/09/a-simple-telnet-client/ - Negotiating with telnet.  
https://telnetscan.shadowserver.org/ - Info on what to scan ect.  

