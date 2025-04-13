#!/usr/bin/env python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.log import setLogLevel
from mininet.cli import CLI
from mininet.node import OVSSwitch, Controller, RemoteController

class NetworkTopo(Topo):
    def __init__(self):
        Topo.__init__(self)

        # Add switches
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')

        # Add hosts
        h1 = self.addHost('h1', ip='10.1.1.2/24')
        h2 = self.addHost('h2', ip='10.1.1.3/24')
        h3 = self.addHost('h3', ip='10.0.0.4/24')
        h4 = self.addHost('h4', ip='10.0.0.5/24')
        h5 = self.addHost('h5', ip='10.0.0.6/24')
        h6 = self.addHost('h6', ip='10.0.0.7/24')
        h7 = self.addHost('h7', ip='10.0.0.8/24')
        h8 = self.addHost('h8', ip='10.0.0.9/24')
        h9 = self.addHost('h9', ip='172.16.10.10/24')  # NAT gateway

        # Switch-switch links
        self.addLink(s1, s2, delay='7ms')
        self.addLink(s2, s3, delay='7ms')
        self.addLink(s3, s4, delay='7ms')
        self.addLink(s4, s1, delay='7ms')
        self.addLink(s1, s3, delay='7ms')

        # Hosts to switches
        self.addLink(h3, s2, delay='5ms')
        self.addLink(h4, s2, delay='5ms')
        self.addLink(h5, s3, delay='5ms')
        self.addLink(h6, s3, delay='5ms')
        self.addLink(h7, s4, delay='5ms')
        self.addLink(h8, s4, delay='5ms')
        self.addLink(h9, s1, delay='5ms')           # H9 connected to S1
        self.addLink(h1, h9, delay='5ms')           # H1 connected to H9
        self.addLink(h2, h9, delay='5ms')           # H2 connected to H9

def run():
    topo = NetworkTopo()
    net = Mininet(topo=topo, controller=None)
    net.addController('c0', controller=RemoteController, ip='127.0.0.1', port=6633)
    net.start()
    CLI(net)
    net.stop()

if __name__ == '__main__':
