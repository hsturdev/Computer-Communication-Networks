# This is one of the project for ECE463 at Purdue University
# It's for academic and educational purpose only
# Please read only, do not share, post, or copy this code
# Especially if you are a student who is taking this course  

# Student name: Tingzhang Li

import sys
from collections import defaultdict
from router import Router
from packet import Packet
from json import dumps, loads


class DVrouter(Router):
    """Distance vector routing and forwarding implementation"""

    def __init__(self, addr, heartbeatTime, infinity):
        Router.__init__(self, addr, heartbeatTime)  # initialize superclass - don't remove
        self.infinity = infinity
        """add your own class fields and initialization code here"""
        self.table = {} # dst: (cost to dst, next hop to dst)
        self.forward = {} # address: port
        # initialize 
        self.table[addr] = (0, addr)
        

    def handlePacket(self, port, packet):
        """process incoming packet"""
        # default implementation sends packet back out the port it arrived
        # you should replace it with your implementation
        if packet.isData(): #data packet
            if (packet.dstAddr in self.table):
                portAddr = self.table[packet.dstAddr][1]
                if(portAddr in self.forward):
                    portSend = self.forward[portAddr]
                else:
                    return
                self.send(portSend, packet)
            else:
                return #drop the packet
        else: # control packet
            if(packet.srcAddr == self.addr):
                return
            recTable = loads(packet.content)
            temp = dumps(self.table)

            delList = []
            for j in self.table.keys():
                if(self.table[j][0]>=self.infinity):
                    delList.append(j)
            for j1 in delList:
                self.table.pop(j1)

            for i in recTable.keys():
                if(recTable[i][1] != self.addr): 
                    if (i not in self.table):                   
                        self.table[i] = (recTable[i][0]+self.links[port].cost, packet.srcAddr)
                    elif(self.table[i][1] == packet.srcAddr):
                        self.table[i] = (recTable[i][0]+self.links[port].cost, packet.srcAddr)
                    elif(recTable[i][0]+self.links[port].cost < self.table[i][0]):
                        self.table[i] = (recTable[i][0]+self.links[port].cost, packet.srcAddr)
            oldtable = loads(temp)
            if(oldtable != self.table):
                self.SendNeighbor()


    def handleNewLink(self, port, endpoint, cost):
        """a new link has been added to switch port and initialized, or an existing
        link cost has been updated. Implement any routing/forwarding action that
        you might want to take under such a scenario"""
        # new link
        self.initialization()
        sendTable = dumps(self.table)
        self.send(port, Packet(kind=Packet.CONTROL,srcAddr=self.addr, dstAddr=endpoint, content=sendTable))

        pass


    def handleRemoveLink(self, port, endpoint):
        """an existing link has been removed from the switch port. Implement any 
        routing/forwarding action that you might want to take under such a scenario"""
        #delList = []
        for i in self.table.keys():
            if(self.table[i][1] == endpoint):
                #delList.append(i)
                self.table[i] = (self.infinity, None)  
        #for j in delList:
        #    self.table.pop(j)           
        if(endpoint in self.forward):
            self.forward.pop(endpoint)
        self.SendNeighbor()
        pass


    def handlePeriodicOps(self):
        """handle periodic operations. This method is called every heartbeatTime.
        You can change the value of heartbeatTime in the json file"""
        self.SendNeighbor()
        pass
    
    #send its routing table to all its neighbor
    def SendNeighbor(self):
        sendTable = dumps(self.table)
        for i in self.forward.keys():
            self.send(self.forward[i], Packet(kind=Packet.CONTROL,srcAddr=self.addr, dstAddr=i, content=sendTable))

    def initialization(self):
        for i in self.links.keys():
            if (self.links[i].e1 == self.addr):
                neighboraddr = self.links[i].e2
            else:
                neighboraddr = self.links[i].e1
            if(neighboraddr not in self.table):
                self.table[neighboraddr] = (self.links[i].cost, neighboraddr)
            if(neighboraddr not in self.forward):
                self.forward[neighboraddr] = i