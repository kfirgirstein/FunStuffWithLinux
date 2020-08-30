# ttftp - HW3
## 046209 - Operating system structure
Simplified version of tftp server
## Description
In this program, we implemented a simplified, one threaded version of tftp server.
The purpose of this exercise is to learn how to use sockets in linux with sys/socket.h
The program is built according to <a href="http://tools.ietf.org/html/rfc1350" >RFC135</a>

* Support only WRQ operation 
* Sends ACK only
* Support for only single client connection 
* Binary information in octet only

## Usage
***server*** <br/>
`./ttftp [server_port]`<br/>
***client*** <br/>
`./tftpc <server_name> <server_port> <S|R> <file_name> [<dist_file_name>]` <br/>


## Author

| #       |              Name |             email |
|---------|-------------------|------------------ |
|Student 1|  Kfir Girstein | kfirgirstein@campus.technion.ac.il |
|Student 2|  Gad Sheffer | gadsheffer@campus.technion.ac.il |
