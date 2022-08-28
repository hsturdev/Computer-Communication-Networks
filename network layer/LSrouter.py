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
from heapq import heappush, heappop, heapify


class PQEntry:

    def __init__(self, addr, cost, next_hop):
        self.addr = addr
        self.cost = cost
        self.next_hop = next_hop

    def __lt__(self, other):
         return (self.cost < other.cost)

    def __eq__(self, other):
         return (self.cost == other.cost)


class LSrouter(Router):
    """Link state routing and forwarding implementation"""

    def __init__(self, addr, heartbeatTime):
        Router.__init__(self, addr, heartbeatTime)  # initialize superclass - don't remove
        self.graph = {} # A dictionary with KEY = router
                        # VALUE = a list of lists of all its neighbor routers/clients and the cost to each neighbor
                        # {router: [[neighbor_router_or_client, cost]]}
        self.graph[self.addr] = []
        """add your own class fields and initialization code here"""
        self.forward = {} #forward table - dis:(cost,nexthop)
        self.seqNumber = 0 # local sequence number
        self.seqNumberV = {} #sequence number vector - dis:(cost,nexthop)

    def handlePacket(self, port, packet):
        """process incoming packet"""
        # default implementation sends packet back out the port it arrived
        # you should replace it with your implementation
        if packet.isData(): #data packet
            if(packet.dstAddr in self.forward):
                portToSend = None
                for i in self.links.keys():
                    if ((self.links[i].e1 == self.forward[packet.dstAddr][1]) or (self.links[i].e2 == self.forward[packet.dstAddr][1])):
                        portToSend = i
                if(portToSend != None):
                    self.send(portToSend, packet)
        else: #LSA
            recevePack = loads(packet.content)
            receveG = recevePack[0]
            receveSeq = recevePack[1]
            if(packet.srcAddr not in self.seqNumberV):
                self.seqNumberV[packet.srcAddr] = receveSeq
            else:
                if(self.seqNumberV[packet.srcAddr] > receveSeq):
                    return
                else:
                    self.seqNumberV[packet.srcAddr] = receveSeq
            temp = dumps(self.graph)
            if(packet.srcAddr != self.addr):
                self.graph[packet.srcAddr] = receveG
            oldgraph = loads(temp)
            if(oldgraph != self.graph):
                Q = self.dijkstra()
                for i in Q:
                    self.forward[i.addr] = (i.cost, i.next_hop)
            for i in self.links.keys():
                if(i != port):
                    self.send(i,packet)



    def handleNewLink(self, port, endpoint, cost):
        """a new link has been added to switch port and initialized, or an existing
        link cost has been updated. Implement any routing/forwarding action that
        you might want to take under such a scenario"""
        for neighbor in self.graph[self.addr]:
            if neighbor[0] == endpoint:
                self.graph[self.addr].remove(neighbor)
        self.graph[self.addr].append([endpoint,cost])
        self.SendLSA()       



    def handleRemoveLink(self, port, endpoint):
        """an existing link has been removed from the switch port. Implement any 
        routing/forwarding action that you might want to take under such a scenario"""
        for neighbor in self.graph[self.addr]:
            if neighbor[0] == endpoint:
                self.graph[self.addr].remove(neighbor)
        delList = []
        for i in self.forward.keys():
            if(self.forward[i][1] == endpoint):
                delList.append(i)
        for j in delList:
            self.forward.pop(j)
        self.SendLSA() 


    def handlePeriodicOps(self):
        """handle periodic operations. This method is called every heartbeatTime.
        You can change the value of heartbeatTime in the json file"""
        self.SendLSA()
        pass


    def SendLSA(self):
        NewPack = [self.graph[self.addr], self.seqNumber]
        SendPack = dumps(NewPack)
        self.seqNumber += 1
        for port in self.links.keys():
            if (self.links[port].e1 == self.addr):
                neighboraddr = self.links[port].e2
            else:
                neighboraddr = self.links[port].e1
            self.send(port, Packet(kind=Packet.CONTROL,srcAddr=self.addr, dstAddr=neighboraddr, content=SendPack))


    def dijkstra(self):
        """An implementation of Dijkstra's shortest path algorithm.
        Operates on self.graph datastructure and returns the cost and next hop to
        each destination router in the graph as a List (finishedQ) of type PQEntry"""
        priorityQ = []
        finishedQ = [PQEntry(self.addr, 0, self.addr)]
        for neighbor in self.graph[self.addr]:
            heappush(priorityQ, PQEntry(neighbor[0], neighbor[1], neighbor[0]))

        while len(priorityQ) > 0:
            dst = heappop(priorityQ)
            finishedQ.append(dst)
            if not(dst.addr in self.graph.keys()):
                continue
            for neighbor in self.graph[dst.addr]:
                #neighbor already exists in finnishedQ
                found = False
                for e in finishedQ:
                    if e.addr == neighbor[0]:
                        found = True
                        break
                if found:
                    continue
                newCost = dst.cost + neighbor[1]
                #neighbor already exists in priorityQ
                found = False
                for e in priorityQ:
                    if e.addr == neighbor[0]:
                        found = True
                        if newCost < e.cost:
                            del e
                            heapify(priorityQ)
                            heappush(priorityQ, PQEntry(neighbor[0], newCost, dst.next_hop))
                        break
                if not found:
                    heappush(priorityQ, PQEntry(neighbor[0], newCost, dst.next_hop))

        return finishedQ

