#main.py
from pathlib import Path
from flask import Flask
from flask import request
from flask_cors import CORS
from py3dbp import Packer, Bin, Item
import json
import uuid
import configparser
from flask import send_file
import os
#===========================================================
#Gobal variable
#===========================================================
#RESULT_JSON_INFOS=[]
PALLET_WEIGHT_LIMMIT=450  #kg
MAX_CONTAINER_HEIGHT=225 #cm
#============================================================
#Gobal config
#============================================================
app=Flask(__name__)
#cors problem
CORS(app, resources={r"/api/*": {"origins": "*"}})
CORS(app, resources={r"/get_resource/*": {"origins": "*"}})
#upload setting
config=configparser.ConfigParser()
config.read("cgi_config.ini")
app.config['UPLOAD_FOLDER']=config['DEFAULT']['UPLOAD_FOLDER']
app.config['MAX_CONTENT_LENGTH']=config['DEFAULT']['MAX_CONTENT_LENGTH']
ALLOWED_EXTENSIONS=set(['xlsx','xls'])
#==================================``===========================
#Function Name: Processsing3DBP
#Description: convert box_info and container info 
#EXAMPLE: HOW STRUCTURE LOOKS LIKE
'''
{
status: 0, 1, 2 //0:success, 1:fail, 2: partial success 3: critical error 
containers:{
	container1:{
	...informations
		fit_box:{	
		}
		unfit_box:{
		}
	},
	container2:{
		fit_box{
			box_infos
		}
		unfit_box:{
		}
	}
}
}
'''
#=============================================================
def Processing3DBP(container_infos, box_infos):
    packer = Packer()
    #0:success, 1:fail, 2:partial sucess 3:critical error

    flag_success=True
    flag_partial_success=False
    

    total_container_types=len(container_infos)
    total_box_types=len(box_infos)


    #pack box_info
    for index, box_info in enumerate(box_infos):
        #if the sameType of box only is one, do nothing and pass it to algorithm
        packer.add_item(Item(box_info['ID'], box_info['name_with_index'], box_info['X'], box_info['Y'], box_info['Z'], box_info['Weight'],box_info['TypeIndex']))
    

    #processing container_info
    for index, container_info in enumerate(container_infos):
        packer.add_bin(Bin(container_info['ID'],
        container_info['name_with_index'],
        container_info['X'],
        container_info['Y'],
        container_info['Z'],
        container_info['Weight_limmit'],
        container_info['TypeIndex']     
             ))
   
    #calculate
    packer.pack()

    containers_array=[]
    statusNumber=-1

    for b in packer.bins:
        #if there if unfitted_item
        if len(b.unfitted_items)!=0:
            flag_success=False
        if len(b.items)!=0:
            flag_partial_success=True
        #result_json_info=json.dumps(b.getResultDictionary(), indent=4)
        containers_array.append(b.getResultDictionary())

    #add statusNumber
    statusNumber=None
    if(flag_success==True):
        #allsuccess
        statusNumber=1
    elif(flag_success==False and flag_partial_success==True):
        #partial success
        statusNumber=3
    else:
        #fail
        statusNumber=2

    #add status of containers
    final_info_dictionary={
        "status":statusNumber,
        "containers":containers_array,
        "total_container_types":total_container_types,
        "total_box_types":total_box_types
        }

    result_json_info=json.dumps(final_info_dictionary, indent=4)
 

        
        #print(b.getResultDictionary())
    print(result_json_info)
    return result_json_info





#===============================================================================
#FunctinoName: Processing3DBPwithPallet
#Description :Base on Processing3DBP
#==========================================================================
def Processing3DBPWithPallet(container_infos, box_infos, pallet_infos):

    virtual_container_with_packed_box_array=[]
    #return the containers with packed information
    packer = Packer()
    #0:success, 1:fail, 2:partial sucess 3:critical error
    statusNumber=-1000
    pallet_status_number=-1000
    flag_success_pallet=True
    flag_partial_success_pallet=False
    

    total_container_types=len(container_infos)
    total_box_types=len(box_infos)
    total_pallet_types=len(pallet_infos)


    #dictionary for searching pallet and virtual container pair
    id_to_pallet=None


    #pack box_info
    for index, box_info in enumerate(box_infos):
        #if the sameType of box only is one, do nothing and pass it to algorithm
        packer.add_item(Item(box_info['ID'], box_info['name_with_index'], box_info['X'], box_info['Y'], box_info['Z'], box_info['Weight'],box_info['TypeIndex']))
    

    #sort the pallet_infos base on area, from large to small
    pallet_infos=sorted(pallet_infos, key=lambda pallet_info: pallet_info["X"]* pallet_info["Z"], reverse=True)
    ids=[pallet_info['ID'] for pallet_info in pallet_infos]
    id_to_pallet=dict(zip(ids, pallet_infos))

    virtual_containers=[]

    ids_of_box=[box['ID'] for box in box_infos]
    id_to_box=dict(zip(ids_of_box, box_infos))

    #create the virtual container to pack, virtual container is the space upon
    for index, pallet_info in enumerate(pallet_infos):
        virtual_container=pallet_info.copy()
        virtual_container["Weight_limmit"]=PALLET_WEIGHT_LIMMIT-pallet_info["Weight"]
        virtual_container["Y"]=MAX_CONTAINER_HEIGHT-pallet_info["Y"]
        virtual_container["Total_box_weight"]=0
        #pack all remain box into one container at once
        packer.add_bin(Bin(virtual_container['ID'],
        virtual_container['name_with_index'], 
        virtual_container['X'],
        virtual_container['Y'],
        virtual_container['Z'],
        virtual_container['Weight_limmit'],
        virtual_container['TypeIndex']
        ))
        packer.pack(bigger_first=True)
        for b in packer.bins:
            if len(b.unfitted_items)!=0:
                flag_success_pallet=False
            if len(b.items)!=0:
                total_weight=0
                for i in b.items:
                    total_weight+=i.weight
                virtual_container["Total_box_weight"]=total_weight
                flag_partial_success_pallet=True
            virtual_container_with_packed_box_array.append(b.getResultDictionary())
        virtual_containers.append(virtual_container)
        #all remain box have been packed
        if len(b.unfitted_items)==0:
            break
        else:
            print("!!!!!!!!!!!!!!!!!!!!find unfitted_items")
            packer=Packer()
            for box_info in b.get_unfitted_items_as_dict_array():
                name_with_index= id_to_box[box_info['ID']]['name_with_index']
                packer.add_item(Item(box_info['ID'], name_with_index, box_info['X'], box_info['Y'], box_info['Z'], box_info['Weight'],box_info['TypeIndex']))


    #find virtual container by ID
    id_to_virtualcontainer=dict(zip(ids, virtual_containers))

    if(flag_success_pallet==True):
        #allsuccess
        pallet_status_number=10
    elif(flag_success_pallet==False and flag_partial_success_pallet==True):
        #partial success
        pallet_status_number=30
    else:
        #fail
        pallet_status_number=20

    #pack virtual container into real container
    status_number=-1000

    flag_success=True
    flag_partial_success=False

    packer=Packer(TWO_D_MODE=True)
    remain_container_infos=container_infos
    containers_array=[]
    packed_pallet_infos=[]


    for virtual_container in virtual_container_with_packed_box_array:
        pallet=id_to_pallet[virtual_container['ID']]
        pallet['Fitted_items']=virtual_container['Fitted_items']
        pallet['Weight']=id_to_virtualcontainer[virtual_container['ID']]['Total_box_weight']+pallet['Weight']
        packed_pallet_infos.append(pallet)

    print(packed_pallet_infos)

    for packed_pallet_info in packed_pallet_infos:
        packer.add_item(Item(
        packed_pallet_info['ID'],
        packed_pallet_info['name_with_index'], 
        packed_pallet_info['X'],
        packed_pallet_info['Y'],
        packed_pallet_info['Z'],
        packed_pallet_info['Weight'],
        packed_pallet_info['TypeIndex'],
        packed_pallet_info['Fitted_items']
        ))


    if len(remain_container_infos) >0:
        container_info=remain_container_infos.pop(0)
        packer.add_bin(Bin(container_info['ID'],
        container_info['name_with_index'],
        container_info['X'],
        container_info['Y'],
        container_info['Z'],
        container_info['Weight_limmit'],
        container_info['TypeIndex']
        ))
        packer.pack()
        b=packer.bins[0]


        #there is a bug need to be fix, maybe i didn't modify the algorithm right, there will have unfitted item even
        # the result totally packed, the following is a mitigration way at present 
        b.unfitted_items=[]
        containers_array.append(b.getResultDictionary())
        print(b.getResultDictionary())

    else:
        #error
        statusNumber=300+pallet_status_number


    statusNumber=100+pallet_status_number

    #add status of containers
    final_info_dictionary={
        "status":statusNumber,
        "total_container_types":total_container_types,
        "total_pallet_types":total_pallet_types,
        "total_box_types":total_box_types,
        "containers":containers_array,
        }

    result_json_info=json.dumps(final_info_dictionary, indent=4)
 

        
        #print(b.getResultDictionary())
    print(result_json_info)
    return result_json_info



#==============================================================
#Function Name: CheckValidJsonData
#Descritpion: Check whether the json file is in the format we want
#Return: if the result is valid return true else false
#==============================================================
# TO DO!
def CheckValidJsonData(infoJsonData):
    FirstLayerKeys={'containers':False,'box':False}
    containers_sec_key={'ID':False,'TypeName':False,'X':False,'Y':False,'Z':False,'Weight_limmit':False,'Numbers':False}
    box_sec_key={'ID':False,'TypeName':False,'X':False, 'Y':False, 'Z':False, 'Weight':False, 'Numbers':False}

    firstkyes=infoJsonData.keys()
    #for key in FirstLayerKeys.keys():
#=============================================================
#
#
#=============================================================
@app.errorhandler(404)
def page_not_found(error):
   return "404 not found"
    
#=============================================================
#Function:testImg
#
#=============================================================
@app.route('/get_resource/image/<filename>', methods=['GET'])
def get_image(filename):
    filepath="./textures/"+filename
    if os.path.isfile(filepath):
        return send_file(filepath, mimetype='image/gif')
    else:
        return "404 not found"


#==============================================================
#Function Name:Index Page
#Description: used in connnection test
#==============================================================
@app.route('/api/test',methods=['POST'])
def IndexPage():
    path = Path(app.config['UPLOAD_FOLDER'])
    path = path / request.get_data().decode("utf-8")
    
    # process = subprocess.Popen('python hackthon.py', shell=True, stdout=subprocess.PIPE)
    # process.wait()
    # print(""process.returncode)
    '''
    if (path.is_file()):
        with open(path, 'r') as inFile:
            parsed = json.loads(inFile.read())
            print(json.dumps(parsed, indent=4, sort_keys=True))
    '''
    #os.system("python hackthon.py /home/team1/3D-Binpacking-GUI/backend/cgi/3dbp_upload/testcase.json")
    file_name = 'SA_box_result.json'
    # file_name = 'text.json'
    if (os.path.isfile(file_name)):
        with open(file_name, 'r') as inFile:
            parsed = json.loads(inFile.read())
            # print(json.dumps(parsed, indent=4, sort_keys=True))
            return json.dumps(parsed, indent=4, sort_keys=True)
    else:
        return "Error"
    # print(request.get_data().decode("utf-8"))
    return "hello"


#===============================================================
#Function Name: upLoadFile()
#Description: handle the file that need to be upload
# the relevent config setting written in cgi_config_ini
# ===============================================================
@app.route('/api/uploadExcelSettingFile',methods=['POST'])
def upLoadExcelSettingFile():
    file=request.files['file']




#===============================================================
#Funciton Name: reciveJsonFromClient
#Description: recive the post data which content the box_info and container_info
#return: the return value send back to server side will contenet with
#data that going to be render in 3d.
#===============================================================
@app.route('/api/recv/3dbinpack/info',methods=['POST'])
def reciveJsonFromClient():
    info_jsondata=request.get_json(force=True)
    # print(info_jsondata)
    with open("text.json", 'w') as out:
        out.write(json.dumps(info_jsondata, indent=4, sort_keys=True))
    #retrive data from json
    container_infos=info_jsondata['containers']
    box_infos=info_jsondata['boxes']
    pallet_infos=info_jsondata['pallets']


    #preprocess the data
    container_infos=preProcessContainerInfos(container_infos)

    #sorted by volume
    box_infos=sorted(box_infos,  key=lambda box_info: int(box_info["X"])* int(box_info["Z"])*int(box_info['Y']), reverse=True)

    box_infos=preProcessBoxInfos(box_infos)
    #pallet mode
    if info_jsondata['pallet_mode']==1:
        print("Pallet mode on")
        pallet_infos=preProcessBoxInfos(pallet_infos)
        jsonData=Processing3DBPWithPallet(container_infos, box_infos, pallet_infos)
    #none pallet mode
    else:
        #print(box_infos)
        jsonData=Processing3DBP(container_infos, box_infos)

    with open('text.json', 'w') as out:
        out.write(jsonData)
    return jsonData
#==================================================================
#Function: PreProcessContainerInfos
#Description: expand the same type of Object
#example: box1 with numbers=3 => [box1, box1, box1]
#==================================================================
def preProcessContainerInfos(container_infos):
    preProcessedInfo=[]
    for container_type_index, container_info in enumerate(container_infos):
        new_container_info={}
        #if the sameType of container number only is one, do nothing and pass it to algorithm
        if (container_info['Numbers'] ==1):
            container_info['TypeIndex']=1
            container_info['name_with_index']=container_info['TypeName']+"_0"
            preProcessedInfo.append(container_info)
        #else multiple numbers condition, create copy and pass it into algorithm
        else:
            #create the numbers of clone
            for number_index in  range(0, int(container_info['Numbers'])):
                new_container_info=container_info.copy()
                new_container_info['name_with_index']=container_info['TypeName']+"_"+str(number_index)
                new_container_info['TypeIndex']=container_type_index
                #create new uuid expect index 0
                if number_index!=0:
                    new_container_info['ID']=str(uuid.uuid4())
                preProcessedInfo.append(new_container_info)
    #print(preProcessedInfo)
    return preProcessedInfo
#=====================================================================
#Function name
#Description
#=====================================================================
def preProcessBoxInfos(box_infos):
    return preProcessContainerInfos(box_infos)

if __name__=="__main__":
    app.run()
