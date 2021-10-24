import random
import json
import math
import sys

# width(X)
# height(Y) --> the height
# depth(Z)

MAX_CONTAINER_EACH_TYPE = 1
MAX_PALLET_EACH_TYPE = 3
MAX_BOX_EACH_TYPE = 50

MIN_BOX_WIDTH = 3


class Box:
	def __init__(self, name, width, height, depth, weight, num):
		self.name = name
		self.width = width
		self.height = height
		self.depth = depth
		self.weight = weight
		self.num = num
	def get_volume(self):
		return self.width * self.height * self.depth
	def string(self):
		return "%s(%sx%sx%s, weight: %s) vol(%s), num=%s" % (
			self.name, self.width, self.height, self.depth, self.weight,
			self.get_volume(), self.num
		)
class Pallete:
	def __init__(self, name, width, height, depth, weight, num):
		self.name = name
		self.width = width
		self.height = height
		self.depth = depth
		self.weight = weight
		self.num = num
	def string(self):
		return "%s(%sx%sx%s, weight:%s) vol(%s), num=%s" % (
			self.name, self.width, self.height, self.depth, self.weight,
			self.get_volume(), self.num
		)
	def get_volume(self):
		return self.width * self.height * self.depth
class Container:
	def __init__(self, name, width, height, depth, weight_limit, num):
		self.name = name
		self.width = width
		self.depth = height
		self.height = depth
		self.num = num
		self.weight_limit = weight_limit
	def string(self):
		return "%s(%sx%sx%s)" % (
			self.name, self.width, self.height, self.depth
		)
	

class Data:
	def __init__(self):
		self.pallete_num = 0
		self.box_num = 0
		self.container_num = 0
		self.pallets = []
		self.boxes = []
		self.containers = []
		self.p_template = [(140,12,102,25), (122,12,102,22), (122,12,90,20), (110,12,75,12), (138,12,89,18)]
		self.c_template = [(580,225,227,20000),(1180,225,227,40000)]
		self.density = 0
	def add_box(self, box):
		self.box_num += box.num
		return self.boxes.append(box)
	def add_pallete(self, pallete):
		self.pallete_num += pallete.num
		return self.pallets.append(pallete)
	def add_container(self, container):
		self.container_num += container.num
		return self.containers.append(container)
	def rand_gen_pallete(self, type_num):
		max_w = self.containers[0].width
		max_d = self.containers[0].depth
		quota_num = math.floor(max_w / 140) * math.floor(max_d / 102)

		pallete_nums = []
		for i in range(type_num):
			num = min(random.randint(1,MAX_PALLET_EACH_TYPE), quota_num)
			pallete_nums.append(num)
			quota_num -= num
		pallete_nums.sort(reverse = True) # sort (big-->small)
		for i in range(len(pallete_nums)):
			template = self.p_template.copy()
			num = pallete_nums[i]
			if num > 0:
				name = "pallete" + str(i)
				p_type = random.randint(0,len(template)-1) # choose a type of Pallete
				w , h, d, kg = template[p_type]
				new_p = Pallete(name, w, h, d, kg, num)
				self.add_pallete(new_p)
				del template[p_type]
	def rand_gen_box(self, type_num):
		box_nums = []
		for i in range(type_num):
			box_nums.append(random.randint(1,100))
		# generate random box
		for i in range(len(box_nums)):
			num = box_nums[i]
			name = "box" + str(i)
			w , h, d, kg = (random.randint(5,140), random.randint(5,round(self.containers[0].height/2)), random.randint(5,102), random.randint(1,20))
			new_b = Box(name,w,h,d,kg,num)
			self.add_box(new_b)
	def rand_gen_box_with_density(self):
		# get the limit
		quota_vol = math.floor(sum(p.get_volume()*p.num for p in self.pallets) * self.density)
		max_w = max(p.width for p in self.pallets)
		max_h = math.floor((self.containers[0].height - max(p.height for p in self.pallets)) / 2)
		max_d = max(p.depth for p in self.pallets)
		min_w = MIN_BOX_WIDTH
		max_macro_num = self.pallete_num*3

		count_box_type = 0
		count_macro = 0
		is_limit = False
		while quota_vol > (min_w**3):
			# check macro num
			if count_macro > max_macro_num and not is_limit:
				max_w = round(max_w / 3)
				max_d = round(max_d / 3)
				max_h = round(max_h / 3)
				is_limit = True
			is_macro = False
			# rand the volume and box_num to use in this iter
			use_vol = random.randint((min_w**3),quota_vol)
			box_num = min(math.floor(use_vol/random.randint((min_w**3),use_vol)), MAX_BOX_EACH_TYPE)
			# rand the whd,kg of a box
			unit_vol = math.floor(use_vol / box_num)
			w = min(random.randint(min_w,max_w),math.floor(unit_vol/(min_w**2)))
			unit_area = math.floor(unit_vol / w)
			d = min(random.randint(min_w,max_d),math.floor(unit_area/min_w))
			h = min(math.floor(unit_area / d),max_h)
			# limit the macro num
			if w > (max_w / 3) or h > (max_h / 3) or d > (max_d / 3):
				is_macro = True
				box_num = min(box_num, max_macro_num-count_macro)
				if box_num < 1:
					continue
			# set box
			kg = random.randint(1,20)
			name = "Box" + str(count_box_type)
			new_b = Box(name,w,h,d,kg,box_num)
			self.add_box(new_b)
			
			count_box_type += 1
			if is_macro:
				count_macro += box_num
			quota_vol = quota_vol - w*d*h*box_num
	def rand_gen_container(self,type_num):
		c_nums = []
		for i in range(type_num):
			c_nums.append(MAX_CONTAINER_EACH_TYPE)
		# generate random container
		for i in range(len(c_nums)):
			template = self.c_template.copy()
			num = c_nums[i]
			name = "container" + str(i)
			c_type = random.randint(0,len(template)-1) # choose a type of Pallete
			w , h, d, kg_limit = template[c_type]
			new_c = Container(name, w, h, d, kg_limit, num)
			self.add_container(new_c)
			del template[c_type]
	def case_gen(self, density):
		self.density = density
		pallete_type_num = random.randint(1, len(self.p_template))
		box_type_num = random.randint(3,20)
		self.rand_gen_container(1)
		self.rand_gen_pallete(pallete_type_num)
		#self.rand_gen_box(box_type_num)
		self.rand_gen_box_with_density()
	def print_info(self):
		total_usable_vol = sum(p.get_volume()*p.num for p in self.pallets)
		used_vol = sum(b.get_volume()*b.num for b in self.boxes)
		utility = used_vol / total_usable_vol
		print("#containers=%s, #pallets=%s, #boxes=%s\ntotal_usable_vol=%s, used_vol=%s, utility=%.2f" % (
			self.container_num, self.pallete_num, self.box_num,
			total_usable_vol, used_vol, utility
		))
		print("*******************Containers**********************")
		for c in self.containers:
			print("  ", c.string())
		print("*******************pallets************************")
		for p in self.pallets:
			print("  ", p.string())
		print("*********************Boxes*************************")
		for b in self.boxes:
			print("  ", b.string())
		print("***************************************************")
		print("***************************************************")
	def output_json(self):
		data_dict = {}
		# box
		boxes_list = []
		for b in self.boxes:
			box_dict = {
				"ID" : "Box"+str(len(boxes_list)),
				"Numbers" : str(b.num),
				"TypeName" : str(b.name),
				"Weight" : str(b.weight),
				"X" : str(b.width),
				"Y" : str(b.height),
				"Z" : str(b.depth)
			}
			boxes_list.append(box_dict)
		# container
		containers_list = []
		for c in self.containers:
			container_dict = {
				"ID" : "Container"+str(len(containers_list)),
				"Numbers" : str(c.num),
				"TypeName" : str(c.name),
				"Weight_limmit" : str(c.weight_limit),
				"X" : str(c.width),
				"Y" : str(c.height),
				"Z" : str(c.depth)
			}
			containers_list.append(container_dict)
		# pallet
		pallets_list = []
		for p in self.pallets:
			pallet_dict = {
				"ID" : "Pallet"+str(len(pallets_list)),
				"Numbers" : str(p.num),
				"TypeName" : str(p.name),
				"Weight" : str(p.weight),
				"X" : str(p.width),
				"Y" : str(p.height),
				"Z" : str(p.depth)
			}
			pallets_list.append(pallet_dict)
		############# 
		# data
		data_dict = {
			"boxes" : boxes_list,
			"containers" : containers_list,
			"pallet_mode" : 1,
			"pallets" : pallets_list
		}
		filename = "case" + str(int(self.density*100)) + ".json"
		with open(filename, 'w') as j_out:
			json.dump(data_dict, j_out)
		print("%s has been generated successfully!" % (filename))

def main(density):
	data = Data()
	data.case_gen(density)
	data.print_info()
	data.output_json()

if __name__ == "__main__":
	density = 0.6 if len(sys.argv) == 1 else float(sys.argv[1])
	if density > 1:
		density /= 100
	main(density)