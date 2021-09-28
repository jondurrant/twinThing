# twinThing
Twin State and message handling between Raspberry PI PICO and host.

Itention is to allow Raspberry PICO to act as IOT device and sync it's state 
with a digital twin. PICO code is C, and host twin is in Python.

# Dependencies

## C Dependencies

Communication is all in JSON format and use JSON libraries from rafagafe.

+[json-maker](https://github.com/rafagafe/json-maker)

+[tiny-json](https://github.com/rafagafe/tiny-json)


## Python Dependencies

Standard Python3

# Design 
The real device, Raspberry PICO, holds a state (Class **State**) item which is updated with the 
state of sensors or state desired for switches, etc. This state is then mirrored
accross to it's twin on the host. Changes can be initiated from the thing or 
from it's digital twin. State is limitted to 7 eightems at the root level. Though
these could be structures in there own right.

Comms between the thing and the host are managed via the Twin classes. Class **Twin**
is abstract and missing the code to actually talk across the comms approach. Methods
*outputJson* and *readLine* should defined in subclass. ***TwinStdio** does this
over stdio and I used to communicate over usb while developing.

Messages can also be sent in either direction. Message are sent for a particular
topic with a JSON or String data payload. **Topic** provide handlers on both sides
to read and process the messages.

There are two default *Topics* "get" (see **TopicGet**) and "ping" (see **TopicPing**. 
"get" is valid from digital twin
to thing and will make the digital twin publish it's full state. "ping" is valid
in both directions and causes the other side to return "pong" message. Data payload
for all these messages is ignored.

There is one final special **Topic** as when the Thing finds an error, such as a
bugger overflow it will write an error out on topic "error". 


## Thing (PICO) Design
The thing state design is fairly simple as I was trying to be careful with limited
resources on a Raspberry  PICO microcontroller. It only holds the current state.
It does keep an eye on when it last heard from the host and initiate a ping if
it has not heard form the host in some time. I don't do anything witht this but
it could be useful for detecting connection issues. Timeout was set to 10seconds
by default which may not be suitable for all communication approachs.

The Thing is considered master of it's identity. So it does not have to accept
any update requested by the digital twin. It holds a **trn** transaction identity
which it must update and return, this allows the digital twin to understand what
has and has not been updated.

Dynamic memory allocation is limitted to observer patturn for listening to 
changes in **Twin**, see **StateObserver** and **Topic** map within **Twin**. 
Intention is that these should be fairly static and not present any memory leaks.

To handle all the JSON parsing and JSON authoring four memory buffers need to be
provided to **Twin** subclass constructor (e.g. **TwinStdio**). There are:

1 readBuffer - read buffer used for reading JSON, needs to be long enough to 
handle the full state structure in JSON or any long TOPIC messahes
2 writeBuffer - write buffer used for writting JSON, same length issues apply. 
These are seperate from readBuffer as code allows for concurrency of sending during
a read operation
3 msgBuffer  - msg buffer used for writting msg structures. This is needed to 
allow a msg to seperate write its data packet outside of the full msg structure
4 jsonBuffer - json Buffer for parsing the json and used by **tiny-json**

## Digital Twin (host) Design
The digital twin design is more complex as it is not fully master of the state 
of the thing. It borrows from AWS IOT framework in concept of having a *desired*
, *reported* and *declined* state (see **Twin.py**). There is also metadata on 
when any given item in the state structure was updated. 

Changes to the state are processed by accpeting a new **dict** object. Firstly the
*desired* state will be updated and then this will be send to the Thing. The thing
must then respond with the update state, at which point the reported state is
updated. Based on the reported state the *desired* state is checked and any item
not processed by the thing is added to the *declined* state.

The python is not package format and I've been updating the *PYTHONPATH* to 
pick up the modules.

# Example
An example project using the library is ...






