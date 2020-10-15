import os
import sys
import time
import subprocess

## MUST run from conda activated terminal
##
## Run in the same folder as VTSC.exe and TSAnalysis.py
## Switch unconverted true/false if the files to test are mp4 or folder format

givenDir = sys.argv[1]
givenModel = sys.argv[2]
if(not os.path.isdir(givenDir)):
    print("exit - not folder")
    sys.exit()
correct = 0
wrong   = 0
total   = 0
unconverted = False

outputFile = open("testingOutput.txt", "w")
fileHeader = "FILE \t\t\t| SYS PREDICTION \t| SYS CORRECT"
print(fileHeader)
outputFile.write(fileHeader+"\n")
start = time.time()
for subItem in os.listdir(givenDir):
    subItem = givenDir+"/"+subItem

    if(unconverted):
        if(os.path.isdir(subItem)): continue
        process = subprocess.run(['VTSC.exe',subItem,givenModel], stdout=subprocess.PIPE)
        output = process.stdout.decode('utf-8').splitlines()
        result = output[len(output)-2]
    else:
        if(not os.path.isdir(subItem)): continue
        process = subprocess.run(['python','TSAnalysis.py',subItem,givenModel], stdout=subprocess.PIPE)
        output = process.stdout.decode('utf-8').splitlines()
        result = output[len(output)-1]

    if(result=="CORRECT" and ("_C_" in subItem)):
        correct += 1
        output = "CORRECT"
    elif(result=="INCORRECT" and ("_I_" in subItem)):
        correct += 1
        output = "CORRECT"
    else:
        wrong += 1
        output = "INCORRECT"
    total += 1
    
    itemResult = subItem + " \t| " + result + " \t\t| "+ output
    print(itemResult)
    outputFile.write(itemResult+"\n")
end = time.time()
time = (end-start)/total
percentage = (correct/total)*100

testingResults = "\nCorrect: "+str(correct)+"\t Wrong: "+str(wrong)+"\t Total: "+str(total)+"\t Accuracy: "+str(round(percentage, 2))+"%\tTime per classify: " +str(round(time,2))+"s"
print(testingResults)
print("Total time: " + str((end-start)) +"s")
outputFile.write(testingResults)
outputFile.close()