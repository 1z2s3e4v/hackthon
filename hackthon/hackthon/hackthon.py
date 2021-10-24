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
            "Z": float(self.Z),
            "Weight_limmit": float(self.Weight_limmit),
            "Fitted_items": [p.getDict() for p in self.Fitted_items],
        }
        return data
    def set_pallet_xy(self):
        # no overlap
        pallet_start_x = 0
        pallet_start_z = 0
        pallet_total_w = 0
        pallet_total_d = 0
        for i in range(len(self.Fitted_items)):
            pallet = self.Fitted_items[i]
            pallet.position_x += pallet_start_x
            pallet.position_z += pallet_start_z
            # for box in pallet.Fitted_items:
            #     box.position_x += pallet_start_x
            #     box.position_z += pallet_start_z
            if i+1 < len(self.Fitted_items): # update start_xz
                next_pallet = self.Fitted_items[i+1]
                if pallet_start_x + pallet.X + next_pallet.X < int(self.X):
                    pallet_start_x += pallet.X
                else:
                    pallet_start_x = 0
                    pallet_start_z += max(p.Z for p in self.Fitted_items)

        # move to center
        pallet_total_w = max(pallet.position_x+pallet.X for pallet in self.Fitted_items)
        pallet_total_d = max(pallet.position_z+pallet.Z for pallet in self.Fitted_items)
        pallet_bias_x = math.floor(int(self.X)-pallet_total_w)/2
        pallet_bias_z = math.floor(int(self.Z)-pallet_total_d)/2
        for pallet in self.Fitted_items:
            pallet.position_x += pallet_bias_x
            pallet.position_z += pallet_bias_z
            
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
        input_container = self.data["containers"][0]
        containter = Container("container1", "container1", 0, input_container["X"], input_container["Y"], input_container["Z"], input_container["Weight_limmit"])
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
            else: # <type_id> <w> <h> <d> <x> <y> <z> <r>
                name = "box" + str(count_box)
                box = Box(name, name, int(line[0]), int(line[1]), int(line[2]), int(line[3]), 10, int(line[4]), int(line[5]), int(line[6]), int(line[7]))
                containter.Fitted_items[-1].Fitted_items.append(box)
                self.total_box_types += 1
                count_box += 1
        self.containers.append(containter)
        self.total_container_types += 1
    def dump_txt(self, filename):
        f = open(filename,'w')
        for pallet_type in self.data["pallets"]:
            for pallet in range(int(pallet_type["Numbers"])):
                # pallet <w> <h> <d> 
                f.write("pallet %s %s %s\n" % (pallet_type["X"],pallet_type["Y"],pallet_type["Z"]))

        count_type = 0
        for box_type in self.data["boxes"]:
            for box in range(int(box_type["Numbers"])):
                # box <type_id> <w> <h> <d> <weight>
                f.write("box %s %s %s %s %s\n" % (count_type,box_type["X"],box_type["Y"],box_type["Z"],box_type["Weight"]))
            count_type += 1

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

    def set_pallet_xy(self):
        for container in self.containers:
            container.set_pallet_xy()
    def print_case_info(self):
        total_usable_vol = sum(int(p["Numbers"])*int(p["X"])*int(p["Z"])*213 for p in self.data["pallets"])
        used_vol = sum(int(b["Numbers"])*int(b["X"])*int(b["Z"])*int(b["Y"]) for b in self.data["boxes"])
        utility = used_vol / total_usable_vol
        #print("#containers=%s, #pallets=%s, #boxes=%s\ntotal_usable_vol=%s, used_vol=%s, utility=%.2f" % (
        print("#containers=%s, #pallets=%s, #boxes=%s" % (
            1, sum(int(p["Numbers"]) for p in self.data["pallets"]), sum(int(b["Numbers"]) for b in self.data["boxes"]),
            #total_usable_vol, used_vol, utility
        ))
        print("***************************************************")

    def run(self):
        print("run data manager.")
    def run_SA(self):
        print("run SA.")
        self.dump_txt("testcase.txt") # dump txt info for sa
        self.print_case_info()
        #os.system("./bin/sa_3dbp testcase.txt") # run sa
        os.system("./bin/sa_3dbp") # run sa
        self.load_box_result("SA_box_result.txt") # load txt result of sa
        self.set_pallet_xy()
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