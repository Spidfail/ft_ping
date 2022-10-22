# ft_ping
Redo the famous network utility `ping` with various flags. Skill : Network and System administration

## Packet

8 bytes header + 56 bytes payload by default.

### ICMP Header

He is meant to handle errors of IP protocol. Each relay (routers or stations) manage ICMP header by default.

The header will always have a size of **8 bytes**. He is encapsulated inside the IP protocol, which means the ICMP header is positionned right after the IP header.

We use the **SOCKET_RAW** type and the **IPPROTO_ICMP** protocol when we create the socket, check the [man page](https://man7.org/linux/man-pages/man2/socket.2.html) for more details.
> A Raw socket is required since it's a protocol with no user interface (see the Linux [raw](https://man7.org/linux/man-pages/man7/raw.7.html) man page.)

Schema :

  ``` bash
_______________________________________________________________
|        _________________________________________  ______    |
|________|____|____|____|____|____|____|____|____|__|____|____|
   |         ^    ^    ^_____^   ^_____^    ^_____^      ^
   |         |    |    |     |   |     |    |     |      |
   |         |    |    |     |   |     |    |     |      |
 (IP)     Type  Code  Checksum    ID     Seq Number   (Data)

  Each column represent an octet : 8 octets
  Preceded by IP header.
  ```

For more informations I recommend the website [FrameIP](https://www.frameip.com/entete-icmp/) for french readers.
  
### Ping and ECHO_REQUEST/REPLY

Ping is basically use to validate or invalidate the presence of a host on a network.
  
`ping` use the `ECHO_REQUEST` **8/0** and the `ECHO_REPLY` **0/0** Type and Code.

- If the host **receive the message**, he will make an `ECHO_REPLY` .
- If the host **has not be found** or **is unreachable**, the last relay will return a ICMP packet to let us know about it.

To identify a packet, we use :
- The **ID** is filled with the _UID_.
- The **Sequence Number** filled with a number incremented at each ECHO_REQUEST sent.
- The **TTL** (in the IP datagram) set as max value (255) and decremented at each relay point.

The Packet is _timed out_ and _returned_ by the relay if the TTL reach 0. It can be manually set with the option `-t`.

### Payload

Payload's data is composed of a **timestamp** (a `stuct timeval`, check [here](https://pubs.opengroup.org/onlinepubs/7908799/xsh/systime.h.html)), then **filled**. The user can give an arbitrary packet size with `-s` or choose a specific timestamp with `-T`.

Note that the timestamp is not mandatory, the data can be filled with anything.


### Output

Ping gives the sum of a send packet each **1scd** by default. If the process is interrupted by a **SIGINT** signal (`Ctrl-C`), `ping` will produce a sum-up of the session.

For the rest like the content of the packets' sum or the session sum-up, ***it depends on the implementation***.

To be consistent here with choose to stick with **Debian 11** (_Bullseye_) output, like this example :

``` bash
~/ft_ping$ ping google.com
PING GOOGLE.com ([HOST_IP]) 56(84) bytes of data.
64 bytes from wr-in-f102.1e100.net ([HOST-IP]): icmp_seq=1 ttl=128 time=18.2 ms
64 bytes from wr-in-f102.1e100.net ([HOST-IP]): icmp_seq=2 ttl=128 time=18.0 ms
64 bytes from wr-in-f102.1e100.net ([HOST-IP]): icmp_seq=3 ttl=128 time=24.2 ms
64 bytes from wr-in-f102.1e100.net ([HOST-IP]): icmp_seq=4 ttl=128 time=24.4 ms
^C
--- GOOGLE.com ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3006ms
rtt min/avg/max/mdev = 17.982/21.184/24.401/3.116 ms
```

Which can be resumed by :

- **Header** : PING `<dest>` (`<host>`) `<ICMP header bytes>`(`<IP header bytes>`) bytes of data.
- **Packet sum** : `<bytes received>` bytes from `<sender>` (`<Host IP address>`): icmp_seq=`<packet number>` ttl=`<ttl remaining>` time=`<time enlapsed>`
- **Statistics** : little header, then
    - `<total nb>` transmitted, `<total received>` received, `<% of loss>` packet loss, time `<total time enlapsed>`
    - rtt min/avg/max/mdev = `<round trip time>`
    
## Options and Flags

`-S <size>` : Set the limit of the socket send buffer in bytes. The minimum value is 2048 bytes and it triggeres errors if this is not correctly set, or if a packet is sent with a size that exceed the limit previously set.

`-t <value>` : Set the TTL value of the IP header. If the option is not provided the TTL is set to the system default, which can vary depending from one distribution to the other (Ex : between Debian or Manjaro / between Debian and Macos).

`-T <timestamp>` : This is an interesting one. Since the time enlapsed printed by ping as default is the one between a ECHO_REQUEST and a ECHO_REPLY (**RTT**), this option will make the IP header timestamp be filled with local time then send the ECHO_REQUEST with it ; and the receiver is asked to do the same in return in his ECHO_REPLY packet.
It means that with this option, the user can calculate the time that enlapsed between a sent ECHO_REQUEST (by local machine) and a received ECHO_REQUEST (by the targeted machine), in other words the time a ECHO_REQUEST makes between two hosts and not only the round trip time.
For a better explanation with a Wireshark example, check [this video](https://youtu.be/4EFEdAyxemk).

For more details on socket options, check [here](https://man7.org/linux/man-pages/man7/socket.7.html).

## Lexique

**I.P.** : _Internet Protocol_.
**I.C.M.P.** : _Internet Message Control Protocol_.
**UID** : _User Identifier_. Retrieved by the C function `getuid()`.
**RTT** : _Round Trip Time_.
**TTL** : _Time To Live_.