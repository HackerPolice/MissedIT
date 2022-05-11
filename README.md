![missedit](Pictures/missedit.gif)

# It looks like we are starting to get detected. Use public forks at your own Risk!

**Want to chat or play with other MissedIT users?**

[![Discord](https://img.shields.io/discord/463752820026376202.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/PDtg8hd)
[![Gitter](https://badges.gitter.im/MissedIT-Fuzion-AimTux/community.svg)](https://gitter.im/MissedIT-Fuzion-AimTux/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

**Want To chat with other fuzion users?**

[![Gitter](https://badges.gitter.im/MissedIT-Fuzion-AimTux/community.svg)](https://gitter.im/Aimtux-Fuzion)

## What is MissedIT?

A fully featured internal hack for *CounterStrike : Global Offensive* written in C++. Which is a Fork
of **[Aimtux-Fuzion](https://github.com/LWSS/Fuzion)**

## Compiling

**Note:** _Do NOT download or compile as the root user._

#### Download the dependencies required to build MissedIT:

If you are having problems compiling make sure you've got the latest version of `g++`.

==================||==================

__Ubuntu-Based / Debian:__

```bash
sudo apt install cmake g++ gdb git libsdl2-dev zlib1g-dev patchelf
```

__Arch:__

```bash
sudo pacman -S base-devel cmake gdb git sdl2 patchelf
```

__Fedora:__

```bash
sudo dnf install cmake gcc-c++ gdb git libstdc++-static mesa-libGL-devel SDL2-devel zlib-devel libX11-devel patchelf
```

===================||===================

## How To use MissedIT

===================||===================

### Step 1 : Download MissedIT

```bash
git clone https://github.com/HackerPolice/MissedIT.git
```

### Step 2 : Navigate to the folder

```bash
cd MissedIT
```

### Step 3 : Compile and load

```bash
./MissedIT -b -l or ./MissedIT --build --load
```

### Step 4 : unLoad Missed

```bash
./MissedIT -ul or ./MissedIT --unload
```

### To Know More Commands

```bash
./MissedIT -h or ./MissedIT --help
```
===================||===================

You might be prompted to enter in your password, this is because the injection script requires root access.

The text printed out during injection is not important.

If the injection was successful you will see a message at the bottom saying `Successfully injected!`, however, if the
message says `Injection failed`, then you've most likely done something wrong.

Now, go back into CS:GO, if you are in the main menu of the game you should see a banner in the top left like so:

![this](Pictures/Legacy/mainmenu-ss.png)

*Note:* if you are getting crashes ( that are unrelated to game updates ) Try disabling shader precaching in your Steam
Client -> Steam -> Settings -> Shader Pre-Caching.

## Using the hack

Now that MissedIT has been injected into the game, press <kbd>Insert</kbd> on your keyboard to open the hack menu (<kbd>
ALT</kbd>+<kbd>I</kbd> if you are using a laptop).

If you want to change skins, create and load configs or open the player list, you can find those buttons at the top of
the screen.

## Unloading the hack

```bash
./MissedIT -ul or ./MissedIT --unload
```

## Update the hack

```bash
./MissedIT -u or ./MissedIT --update
```

If The update is availabel but the cheat is not updateing then try bellow command

```bash
./MissedIT -uf or ./MissedIT --update-force
``` 

## Configs

Configs are stored in a hidden directory in your home folder. Specifically

```
~/.config/MissedIT/CSGO
```

Each `config.json` is stored in a seperately named folder (The name you see in-game, in the config window).

To add a config, create a folder inside of the `~/.config/MissedIT/CSGO` folder with a name of your choice, and paste
the `config.json` inside of that folder.

To see hidden folders inside your home folder, press <kbd>CTRL</kbd>+<kbd>H</kbd> when using a file manager.

On your command line, you can also add the -a flag on `ls` e.g.

```bash
ls -la ~/
```

## Grenade Configs

```
~/.config/MissedIT-GH/CSGO
```

Each `config.json` is stored in the folder named after them map name.

To add a config, copy the folder containing it to `~/.config/MissedIT/CSGO`

## Screenshots

### We Have Two Theme Available

#### 1 Legacy Theme

![LegitBot](Pictures/Legacy/Legitbot.png)
**To Check More Pictures *[Click Here](Pictures/Legacy/Readme.md)***

### 2 AimwareV5 (In Beta)

![RageBot](Pictures/Aimware/RageBot.png)
**To Check More Pictures *[Click Here](Pictures/Aimware/Readme.md)***

## Credits

Special thanks to the Original AimTux
project: [https://github.com/AimTuxOfficial/AimTux](https://github.com/AimTuxOfficial/AimTux).

Special thanks to the Fuzion For Panaroma Update And For Quality
Codes: [https://github.com/LWSS/Fuzion](https://github.com/LWSS/Fuzion).

Special thanks to the Nimbus
project : [https://github.com/NimbusProjects/Nimbus](https://github.com/NimbusProjects/Nimbus)

Special thanks to [@aixxe](http://www.github.com/aixxe/) ([aixxe.net](http://www.aixxe.net)) for the skin changer and
with the initial project, as well as helping this project with source code (Available
on [@aixxe's](http://www.github.com/aixxe/) github page.).

This project was also originally based upon
Atex's [Linux Basehook](http://unknowncheats.me/forum/counterstrike-global-offensive/181878-linux-basehook.html).
