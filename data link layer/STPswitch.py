# This is one of the project for ECE463 at Purdue University
# It's for academic and educational purpose only
# Please read only, do not share, post, or copy this code
# Especially if you are a student who is taking this course  

# Student name: Tingzhang Li

import json
import sys
import time
from switch import Switch
from link import Link
from client import Client
from packet import Packet
#from json import loads, dumps


class STPswitch(Switch):
    """MAC learning and forwarding implementation."""

    def __init__(self, addr, heartbeatTime):
        Switch.__init__(self, addr, heartbeatTime)  # initialize superclass - don't remove
        """TODO: add your own class fields and initialization code here"""
        self.view = {'root': addr, 'cost': 0, 'self': addr, 'nextHop': addr} # proposes itself as root
        self.forwardTable = {} #('destination Addr': port)
        self.neighbor = {} #('addr': port)
        self.cost = {} #(port: cost)
        self.counter = 40

    def handlePacket(self, port, packet):
        """TODO: process incoming packet"""
        # default implementation sends packet back out the port it arrived
        if packet.dstAddr == "X":
            self.MAClearning(port, packet)
            self.broadcast(port, packet)
        elif packet.isData():
            self.MAClearning(port, packet)
            dstAddr = packet.dstAddr
            if(dstAddr in self.forwardTable and self.links[port].status == Link.ACTIVE):
                portSend = self.forwardTable[dstAddr]
                if(portSend == port):
                    return
                else:
                    self.send(portSend, packet)
            else:
                self.broadcast(port, packet)
        else:
            #Control packet, perform STP
            oldview = self.view
            packetview = json.loads(packet.content)
            #case 1
            if(self.view['nextHop'] == packetview['self']):
                if(int(self.view['root']) > int(packetview['root'])):
                    self.view['root'] = packetview['root']
                    self.view['cost'] = self.links[port].l + packetview['cost']
                    self.view['nextHop'] = packetview['self']
            #case 2
            else:
                if(int(self.view['root']) > int(packetview['root'])):
                    self.view['root'] = packetview['root']
                    self.view['cost'] = self.cost[port] + packetview['cost']
                    self.view['nextHop'] = packetview['self']
                elif((int(self.view['root']) == int(packetview['root'])) and (self.cost[port] + packetview['cost']<=self.view['cost'])):

                    self.view['root'] = packetview['root']
                    self.view['cost'] = self.cost[port] + packetview['cost']
                    self.view['nextHop'] = packetview['self']

            if(oldview != self.view):
                self.SendNeighbor()

            #check H every time receive from Y
            if((self.view['nextHop']!= packetview['self']) and (packetview['nextHop']!=self.view['self'])): 
                self.links[port].status = Link.INACTIVE
            nextHop = self.view['nextHop']
            if(nextHop in self.neighbor):
                neigborport = self.neighbor[nextHop]
                if(self.links[neigborport].status == Link.INACTIVE):
                    self.links[neigborport].status = Link.ACTIVE
            


    def handleNewLink(self, port, endpoint, cost):
        """TODO: handle new link"""

        if(port in self.cost):
            oldcost = self.cost[port]
        else:
            oldcost = 0
        self.view['cost'] = self.view['cost'] - oldcost + cost
        self.cost[port] = cost
        self.SendNeighbor()
        pass


    def handleRemoveLink(self, port, endpoint):
        """TODO: handle removed link"""
        self.view['root'] = self.view['self']
        self.view['cost'] = 0
        self.view['nextHop'] = self.view['self']
        self.SendNeighbor()
        pass

    def handlePeriodicOps(self, currTimeInMillisecs):
        """TODO: handle periodic operations. This method is called every heartbeatTime.
        You can change the value of heartbeatTime in the json file."""
        self.SendNeighbor()
        pass

    
    # send packet to all port expect the port receive the packet
    def broadcast(self, port, packet):
        for i in self.links.keys():
            self.cost[i] = self.links[i].l
            addr1 = self.links[i].e1
            addr2 = self.links[i].e2
            if((addr1 not in self.neighbor) or (addr2 not in self.neighbor)):
                if((addr1.isnumeric()) and (addr1!=self.addr)):
                    self.neighbor[addr1] = i
                elif((addr2.isnumeric()) and (addr2!=self.addr)):
                    self.neighbor[addr2] = i
            
            if((port != i) and (self.links[i].status == Link.ACTIVE)):
                self.send(i, packet)
    
    # MAC Learning - add reachable addrress to switch
    def MAClearning(self, port, packet):
        srcAddr = packet.srcAddr
        dstAddr = packet.dstAddr
        if(self.links[port].status == Link.ACTIVE):
            self.forwardTable[srcAddr] = port
        if((srcAddr in self.forwardTable) and (self.links[port].status == Link.INACTIVE)):
            self.forwardTable.pop(srcAddr)
        if((dstAddr in self.forwardTable) and self.view['cost'] >= self.counter):
            self.forwardTable.pop(dstAddr)
            
    # Send control packet to all neighbor switch
    def SendNeighbor(self):
        newview = json.dumps(self.view)
        for i in self.neighbor.keys():
            self.send(self.neighbor[i], Packet(Packet.CONTROL,self.addr, i, newview))
