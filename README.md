<p align="center">
  <img width="300" src="./assets/logo.png">
</p>

<p align="center">
    <a href="LICENSE" alt="License">
        <img src="https://img.shields.io/badge/License-GPLv3-brightgreen.svg" /></a>
   <a href="https://twitter.com/swingcake" alt="Twitter">
        <img src="https://img.shields.io/twitter/url/https/twitter.com/swingcake.svg?style=social&label=Follow%20%40swingcake" /></a>
</p>

# Flareoff - Firewall as Loadable Kernel Module

Flareoff is a stateful software firewall implemented as a Loadable Kernel Module (LKM). Flareoff provides network security by filtering incoming and outgoing network traffic based on a set of user-defined rules.  

Loadable Kernel Modules are kernel extensions that can be loaded into operating system kernel dynamically.  

## Working

`main.c` resides in the kernel and hooks itself with the existing netfilter hooks, specifically the `PRE_ROUTING` hook. In the topology that is created, gateway is then placed at a position such that all the traffic to and from outside the LAN flows through this gateway.

The 'main' module inspects every packet and based on the rules provided, makes the decision of whether to accept the packet or to drop it.

Following are the rules 'main' module implements:

* Block all unsolicited ICMP packets coming in from outside except the ones going to the web-server. However, the local hosts should be able to ping outside.
* Block all SSH attempts from outside.
* Block port 80 (HTTP) access from outside except for the web-server and test that an internal website on a local host is only accessible from inside.

## Setup

Provided is `main.c` and a _Makefile_.  

Pre-requisites for successful build:  
`sudo apt-get install linux-headers-$(uname -r) build-essential`

1. Run the _make_ command inside the directory you have kept these files  
`make`
2. Successful build will generate `main.ko` which is the kernel module
3. Now, load this module directly in the kernel  
`sudo insmod ./main.ko`
4. To see which packets pass the filter and which are dropped, use  
`sudo tail -f /var/log/kern.log`
5. To unload the kernel module,  
`sudo rmmod main`

## Licensing and Contribution

Flareoff is an open-source software and is licensed under the GNU General Public License v3.0. Anyone interested is free to contribute to the project.
