import os, re  
from argparse import ArgumentParser
 
# execute command, and return the output  
def execCmd(cmd):  
    r = os.popen(cmd)  
    text = r.read()  
    r.close()  
    return text  
 
# write "data" to file-filename  
def writeFile(filename, data):  
    f = open(filename, "w")  
    f.write(data)  
    f.close()  
 
#
if __name__ == '__main__':  
    parser = ArgumentParser()
    parser.add_argument('-t','--target', metavar="Path", help='The Path of Target', default='')
    arg = parser.parse_args()
    pass1 = "opt -load ../pass1/libPass1.so -newcg "
    pass2 = "opt -load ../pass2/libPass2.so -newcg "
    pass3 = "opt -load ../pass3/libPass3.so -newcg "
    pass4 = "opt -load ../pass4/libPass4.so -newcg "
    # target = "/home/tada/Desktop/Sased/vmlinux.bc"
    # target = "/home/tada/Desktop/kernels/linux-5.4/drivers/net/wireless/realtek/rtlwifi/.ps.o.bc"
    target = ""
    if arg.target:
        target = arg.target

    if target == "":
        print("You need choose a target.bc")
        exit()
    errHandlerConfirmList = "errHandlerConfirmList: "
    result = execCmd(pass1+target)
    print("suspicious filter:")
    print(result)
    result = execCmd(pass2+target)
    print("recursive confirm:")
    print(result)
    last_confirm_num = 0
    # print(type(result))
    # print(re.search(errHandlerConfirmList,result).span())
    # print(type(re.search(errHandlerConfirmList,result).span()))
    # print(re.search(errHandlerConfirmList,result).span()[0])
    confirm_num =  result[re.search(errHandlerConfirmList,result).span()[1]:].split()[0]
    while(confirm_num != last_confirm_num):
        last_confirm_num = confirm_num
        result = execCmd(pass3+target)
        print("ancestor-child extend:")
        print(result)
        result = execCmd(pass2+target)
        print("recursive confirm:")
        print(result)
        result = execCmd(pass4+target)
        print("slice extend:")
        print(result)
        result = execCmd(pass1+target)
        print("suspicious filter:")
        print(result)
        result = execCmd(pass2+target)
        print("recursive confirm:")
        print(result)
        confirm_num = result[re.search(errHandlerConfirmList,result).span()[1]:].split()[0]        
    print("complete")


    #cmd = "ipconfig /all"  
    #result = execCmd(cmd)  
    #pat1 = "Physical Address[\. ]+: ([\w-]+)"  
    #pat2 = "IP Address[\. ]+: ([\.\d]+)"  
    #MAC = re.findall(pat1, result)[0]       #  
    #IP = re.findall(pat2, result)[0]        #
    #print("MAC=%s, IP=%s" %(MAC, IP)) 
