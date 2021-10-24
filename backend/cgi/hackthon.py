import random
import json
import math
import sys
import os

class Node:
    def __init__(self,name,w,h,d,kg):
        self.name = name
        self.width = w
        self.height = h
        self.depth = d
        self.weight = kg
        self.vol = w*h*d
class Edge:
    def __init__(self,node1,node2):
        self.name = node1.name + "_" + node2.name
        self.node1 = node1
        self.node2 = node2
        self.weight = self.cal_weight()
    def cal_weight():
        weight = 0
        node1_e = [node1.width,node1.deight,node1.depth]
        node2_e = [node2.width,node2.deight,node2.depth]
        node1_e.sort()
        node2_e.sort()
        for e1, e2 in node1_e, node2_e:
            weight += 1 if e1==e2 else 0

class Partition:
    def __init__(self):
        self.node = {}
        self.edge = {}
    def run(self):
        print("run partition")

class Box:
    def __init__(self, id, name, t_idx, w, h, d, kg, x, y, z, r):
        self.ID = id
        self.TypeName = name
        self.TypeIndex = t_idx
        self.X = w
        self.Y = h
        self.Z = d
        self.weight = kg
        self.position_x = x
        self.position_y = y
        self.position_z = z
        self.RotationType = r
    def getDict(self):
        data = {
            "ID": self.ID,
            "TypeName": self.TypeName,
            "TypeIndex": self.TypeIndex,
            "X": float(self.X),
            "Y": float(self.Y),
            "Z": float(self.Z),
            "Weight": float(self.weight),
            "position_x": float(self.position_x),
            "position_y": float(self.position_y),
            "position_z": float(self.position_z),
            "RotationType": self.RotationType,
        }
        return data
class Pallete:
    def __init__(self, id, name, t_idx, w, h, d, kg, x, y, z, r):
        self.ID = id
        self.TypeName = name
        self.TypeIndex = t_idx
        self.X = w
        self.Y = h
        self.Z = d
        self.weight = kg
        self.position_x = x
        self.position_y = y
        self.position_z = z
        self.RotationType = r
        self.Fitted_items = []
    def getDict(self):
        data = {
            "ID": self.ID,
            "TypeName": self.TypeName,
            "TypeIndex": self.TypeIndex,
            "X": float(self.X),
            "Y": float(self.Y),
            "Z": float(self.Z),
            "Weight": float(self.weight),
            "position_x": float(self.position_x),
            "position_y": float(self.position_y),
            "position_z": float(self.position_z),
            "RotationType": self.RotationType,
            "Fitted_items": [b.getDict() for b in self.Fitted_items],
        }
        return data

class Container:
    def __init__(self, id, name, t_idx, w, h, d, kg):
        self.ID = id
        self.TypeName = name
        self.TypeIndex = t_idx
        self.X = w
        self.Y = h
        self.Z = d
        self.Weight_limmit = kg
        self.Fitted_items = []
    def getDict(self):
        data = {
            "ID": self.ID,
            "TypeName": self.TypeName,
            "TypeIndex": self.TypeIndex,
            "X": float(self.X),
            "Y": float(self.Y),
            "Z": float(self.X),
            "Weight_limmit": float(self.Weight_limmit),
            "Fitted_items": [p.getDict() for p in self.Fitted_items],
        }
        return data
    
class DMgr:
    def __init__(self):
        self.data = {}
        self.result_data = {}
        #self.part_cpp = cpp_exefile
        #self.parter = Partition()
        self.containers = []
        self.total_container_types = 0
        self.total_pallet_types = 0
        self.total_box_types = 0

    def init(self, data_dict):
        print("data.init()")
        #for obj in data_dict:
            #print("")
    def getContainerList(self):
        return [cont.getDict() for cont in self.containers]
    def load_testcase(self, jsonfile):
        input_data = {}
        with open(jsonfile, newline='') as inFile:
            input_data = json.load(inFile)
        self.data = input_data
    def load_box_result(self, box_result_txt):
        self.data = {}
        containter = Container("container1", "container1", 0, 580, 225, 227, 20000)
        f = open(box_result_txt)
        lines = f.readlines()
        count_box = 0 # necessary
        for line in lines:
            line = line.split()
            if line[0] == "pallete": # pallete <w> <h> <d> <nBox>
                name = line[1]+"X"+line[3]+"_"+str(len(containter.Fitted_items))
                pallete = Pallete(name, name, len(containter.Fitted_items)+1, int(line[1]), int(line[2]), int(line[3]), 23, 0, 0, 0, 0)
                containter.Fitted_items.append(pallete)
                self.total_pallet_types += 1
            else: # <w> <h> <d> <x> <y> <z> <r>
                name = "box" + str(count_box) + "_1"
                box = Box(name, name, count_box, int(line[0]), int(line[1]), int(line[2]), 10, int(line[3]), int(line[4]), int(line[5]), int(line[6]))
                containter.Fitted_items[-1].Fitted_items.append(box)
                self.total_box_types += 1
                count_box += 1
        self.containers.append(containter)
        self.total_container_types += 1
    # def dump_txt(self, filename):
    #     f = open(filename,'w')
    #     for pallet_type in self.data["pallets"]:
    #         for pallet in range(pallet_type["Numbers"]):
    #             # pallet <w> <h> <d> 
    #             f.write("pallet %s %s %s" % (pallet["X"],pallet["Y"],pallet["Z"]))

    #     count_type = 0
    #     for box_type in self.data["boxes"]:
    #         for box in range(box_type["Numbers"]):
    #             # box <type_id> <w> <h> <d> <weight>
    #             f.write("box %s %s %s" % (box["X"],box["Y"],box["Z"]))

    def dump_json(self,filename):
        result_data = {
            "status" : 1,
            "total_container_types" : self.total_container_types,
            "total_pallet_types" : self.total_pallet_types,
            "total_box_types" : self.total_box_types,
            "containers" : self.getContainerList(),
        }
        with open(filename, 'w') as j_out:
            json.dump(result_data, j_out)
        print("%s has been generated successfully!" % (filename))

    def run(self):
        print("run data manager.")
    def run_SA(self):
        print("run SA.")
        #self.dump_txt("testcase.txt") # dump txt info for sa
        os.system("./bin/sa_3dbp") # run sa
        self.load_box_result("SA_box_result.txt") # load txt result of sa
        self.dump_json("SA_box_result.json") # dump json result of sa

def main():
    if len(sys.argv) < 2:
        print("Error: Please input input.json")
        exit(1)
    dmgr = DMgr()
    dmgr.load_testcase(sys.argv[1])
    #dmgr.run_partition()
    dmgr.run_SA()
        

if __name__ == "__main__":
    main()