# IDP-8th-sem

### server.py
1. All clients are handled by separate threads.
2. Communication format is JSON.
3. If ip address is not specified in JSON, the server echo backs the data. 
4. Else it forwards the data to the specified ip address, also it changes the ip address to the source ip(from initial dest ip)  which can be used by destination to send back data again.

### android 
1. All fields are formatted to JSON and sent/received.


### esp8266
1. Forwards received data from/to launchpad to/from the server.
2. It sends data in size of 100 bytes to the launchpad.
3. It can receive variable length data from launchpad.

### launchpad
1. Receives data from the esp8266.
2. Responds with the appropritate response.
