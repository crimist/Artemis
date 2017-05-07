# Artemis
IoT malware that I'm writing for fun. 

## General About
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

* **Command page cleanup** - Make the command page pretty. Ooo shiny...
* **Command log page** - A page that shows commands executed by bots chronologically.
* **Logs** - Log all action that clients do and let admins read it. Features include:
  * Home page accessed with IP address.
  * Failed log in attempts with IP address.
  * Successful log in attempts with IP address.
  * Show commands added.
* **Multi user support** - If admin be able to create new users form user panel.

## Client

### About
The client side is IoT because C is the language I know best although I'm attempting to learn C++ now.
Client side is inspired from both [BashLite](https://en.wikipedia.org/wiki/BASHLITE) and [Mirai](https://en.wikipedia.org/wiki/Mirai_(malware)).

### Features

* **Uhhh...** - The bot isn't really complete so it's pretty bare feature wise right now...

### Planned features

* **Self replication** - Self replicating bot allows the botnet to have an exponential growth in power without any effort.
* **DDoS?** - I might add DDoS features later though I'm not sure if I wanna weaponize the bot.
* **Info gathering** - It already has features that gather info but I might add more later.
* **Update** - Let the bot update itself to the newest version.

## Credits

Okay so I'm 100% sure I'm missing a bunch because I wasn't documenting this until I put this on GitHub but I'm trying my best to give credit where it's due.

GitHub - For giving me these private repos.  
https://www.youtube.com/watch?v=n2pB-QQrfaE (LinkCabin) - His HTTP panel was big help for me because I wasn't sure how the architecture of a HTTP botnet worked and how to use databases the best way.  
https://developer.mbed.org/handbook/C-Data-Types - Obvious.  
http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html - How to detach from terminal and signore signals.  
https://en.wikipedia.org/wiki/Unix_signal - Which signal mean and do what.  
http://stackoverflow.com/questions/10046916/is-it-possible-to-ignore-all-signals - How to block signals.  
http://man7.org/linux/man-pages/man7/signal.7.html - More info on signals  
Hydra (2008) - Concept.  
Mirai (2016) - Concept and its scanner.  
Qbot (?) - Concept and its scanner.  
http://www.binarytides.com/tcp-syn-portscan-in-c-with-linux-sockets/ - TCP portscanning in C.  
https://l3net.wordpress.com/2012/12/09/a-simple-telnet-client/ - Negotiating with telnet.  
https://telnetscan.shadowserver.org/ - Info on what to scan ect.  

