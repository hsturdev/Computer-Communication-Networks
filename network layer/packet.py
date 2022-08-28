# This is one of the project for ECE463 at Purdue University
# It's for academic and educational purpose only
# Please read only, do not share, post, or copy this code
# Especially if you are a student who is taking this course  

# Student name: Tingzhang Li

from copy import deepcopy

class Packet:
    """Packet class defines packets that clients and routers
       send in the simulated network"""

    # Access these constants from other files
    # as Packet.DATA or Packet.CONTROL
    DATA = 1
    CONTROL = 2

    def __init__(self, kind, srcAddr, dstAddr, content=None):
        """create a new packet"""
        self.kind = kind        # either DATA or CONTROL
        self.srcAddr = srcAddr  # address of the source of the packet
        self.dstAddr = dstAddr  # address of the destination of the packet
        self.content = content  # content of the packet (must be a string)
        self.route = [srcAddr]  # DO NOT access from DSrouter or LSrouter


    def copy(self):
        """Create a deepcopy of the packet.  This gets called automatically
           when the packet is sent to avoid aliasing issues"""
        p = Packet(self.kind, self.srcAddr, self.dstAddr, content=deepcopy(self.content))
        p.route = list(self.route)
        return p


    def isData(self):
        """Returns True if the packet is a DATA packet"""
        return self.kind == Packet.DATA


    def isControl(self):
        """Returns True if the packet is a CONTROL packet"""
        return self.kind == Packet.CONTROL


    def addToRoute(self, addr):
        '''DO NOT CALL from DVrouter or LSrouter'''
        self.route.append(addr)


    def getRoute(self):
        '''DO NOT CALL from DVRouter or LSrouter'''
        return self.route
