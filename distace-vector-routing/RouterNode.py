#!/usr/bin/env python
from typing import List
from RouterSimulator import RouterSimulator
import GuiTextArea
from RouterPacket import RouterPacket
from copy import deepcopy

INFTY: int = 0
N: int = 0


class RouterNode:
    ID: int = -1
    myGUI = None
    sim = None

    costs: List[int] = list()
    routesList: List[int] = list()
    D: List[List[int]] = list()

    def __init__(self, ID: int, sim: RouterSimulator, costs: List[int]):
        self.ID = ID
        self.sim = sim
        self.myGUI = GuiTextArea.GuiTextArea(
            "  Output window for Router #" + str(ID) + "  "
        )

        # Setup constants
        global N, INFTY
        N = sim.NUM_NODES
        INFTY = sim.INFINITY

        self.costs = deepcopy(costs)
        self.routesList = [i if costs[i] != INFTY else -1 for i in range(N)]
        # for i in range(N):
        #     if self.costs[i] != INFTY:
        #         self.routesList[i] = i
        #     else:
        #         self.routesList[i] = -1

        self.D = [[INFTY for j in range(N)] for i in range(N)]
        self.D[ID] = deepcopy(costs)

        self.sendUpdate()
        self.printDistanceTable()

    def update(self) -> bool:
        changed = False
        for i in range(N):
            if i == self.ID:
                continue

            newRouteCost = INFTY

            oldRoute = self.routesList[i]
            oldRouteCost = self.D[self.ID][i]

            for j in range(N):
                if j == self.ID:
                    continue

                if (
                    self.costs[j] != INFTY
                    and newRouteCost > self.D[j][i] + self.costs[j]
                    and j != self.ID
                ):
                    self.D[self.ID][i] = self.D[j][i] + self.costs[j]
                    newRouteCost = self.D[self.ID][i]
                    self.routesList[i] = j

            if oldRoute != self.routesList[i] or oldRouteCost != self.D[self.ID][i]:
                changed = True

        return changed

    def recvUpdate(self, pkt):
        self.myGUI.println(
            f"[Router {self.ID}] Received distance vector from Router {pkt.sourceid}"
        )
        self.D[pkt.sourceid] = pkt.mincost
        if self.update():
            self.sendUpdate()

    def sendUpdate(self):
        for i in range(N):
            sendedCosts = deepcopy(self.D[self.ID])

            if i == self.ID or self.costs[i] == INFTY:
                continue

            if self.sim.POISONREVERSE:
                for j in range(N):
                    if self.routesList[j] == i and j != i:
                        sendedCosts[j] = INFTY

            pkt = RouterPacket(self.ID, i, sendedCosts)
            self.sim.toLayer2(pkt)

    def updateLinkCost(self, dest, newcost):
        self.myGUI.println(
            f"[Router {self.ID}] Received link cost update to destination {dest}: old({self.costs[dest]}), new({newcost})"
        )

        self.costs[dest] = newcost
        if self.update():
            self.sendUpdate()

    def printDistanceTable(self):
        self.myGUI.println(f"Routing table for {self.ID} at {self.sim.getClocktime()}")
        # self.myGUI.println("--------------------")
        # for n in range(N):
        #     self.myGUI.print(f"{n}:  ")
        #     for m in range(N):
        #         self.myGUI.print(f"{self.D[n][m]} | ")
        #     self.myGUI.println("")
        self.myGUI.println(f"D_{self.ID}: {self.D[self.ID]}")
        self.myGUI.println(f"R_{self.ID}: {self.routesList}")
        self.myGUI.println("---")
