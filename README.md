# TCP Client-Server Tunneling 

---

## Server Side Tun Interface Configuration
In the Server run following commands to setup a TUN interface called asa0,

sudo ip tuntap add dev asa0 mode tun <br>
sudo ip addr add 10.0.1.1/24 dev asa0 <br>
sudo ip link set dev asa0 up<br>
sudo ip addr show

---

## Client Side Tun Interface Configuration
In the Client run following commands to setup a TUN interface called asa1,

sudo ip tuntap add dev asa1 mode tun <br>
sudo ip addr add 10.0.1.2/24 dev asa1 <br>
sudo ip link set dev asa1 up<br>
sudo ip addr show

<strong> Note </strong> : Here as we are running client and server on the same machine so we are using two different names(asa1 and asa0) for tunnel interface.

---

## Compile and Run
    Server
        gcc tun-server.c -o tun-server
        ./tun-serve

    Client 
        gcc tun-client.c -o tun-client
        ./tun-client

---

## Test the Tunnel between Server and Client

Ping from host 1 to host 2 where our ping packets will be delivered through the TCP client and TCP server in the TCP socket connection (connection x),

    ping -I 10.0.1.1 10.0.1.2
