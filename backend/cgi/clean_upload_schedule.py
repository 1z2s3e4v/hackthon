import datetime


#=======================================================
#Clean upload floder every weekend
#=======================================================
if datetime.datetime.now().weekday() > 6:
    print("deleting the folder")
    
