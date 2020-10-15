# import warnings filter
from warnings import simplefilter
# ignore all future warnings
simplefilter(action='ignore', category=FutureWarning)

##system and compile driven imports
import sys
import os

##ml driven imports
import pandas as pd
import numpy as np
from tslearn.neighbors import KNeighborsTimeSeriesClassifier
from tslearn.utils import to_time_series_dataset

def predict(inputTimeSeries,trainedModel):
    print("Predicting...")
    clf = KNeighborsTimeSeriesClassifier(n_neighbors=1, metric='dtw')
    tmodel = clf.from_pickle(trainedModel)
    inputTS = pd.read_csv(inputTimeSeries)
    X = to_time_series_dataset([inputTS])

    prediction = tmodel.predict(X)
    predictionTXT = "CORRECT" if prediction==1 else "INCORRECT"
    print(predictionTXT)
    return predictionTXT

def train(directory, modelName):
    print("Training...")
    TSList = dirToDataFrames(directory)
    if (len(TSList) == 0): 
        print("No training data found, possibly check arguments")
        return
    clf = KNeighborsTimeSeriesClassifier(n_neighbors=1, metric='dtw')
    xs = []
    ys = [] 
    for i in TSList:
        xs.append(i[0])
        ys.append(i[1])
    X = to_time_series_dataset(xs)
    clf = clf.fit(X, ys)
    clf.to_pickle(modelName+".model")
    print("Model saved to "+ modelName+".model")
    return

def dirToDataFrames(directory):
    tsCount = 0
    TSList = []
    if(not os.path.isdir(directory)): return []
    for subDir in os.listdir(directory):
        subDir = os.path.normpath(directory+"\\"+subDir)
        if(os.path.isdir(subDir)):
            csv = getCSV(subDir)
            print(csv)
            if csv is not None:
                ts=pd.read_csv(csv) 
                TSList.append([ts, 0 if "_I_" in subDir else 1, tsCount])
                tsCount = tsCount + 1
    return TSList

def getCSV(directory):
    if (os.path.exists(directory+"/jointAngles.csv")):
        return directory+"/jointAngles.csv"

def isModel(path):
    return os.path.isfile(path) and path.endswith(".model")   

def dataframesToCSV(dfs,csvName):
    outputList = []
    for df in dfs:
        df[0]['Correct'] = np.full(df[0].shape[0], df[1])
        df[0]['Count'] = np.full(df[0].shape[0], df[2])
        outputList.append(df[0])
    TSFull = pd.concat(outputList)
    TSFull.to_csv(csvName+".csv", index=False)
    return

def inputDataFromCSV(csv):
    md = pd.read_csv(csv)
    ts = []
    for i in range(md["Count"].max(axis=0)+1):
        ts.append(md[md['Count'] == i])
    return ts

def main(argv):
    errorWithArgs = False
    if(not os.path.exists(argv[1])):
        errorWithArgs = True
    csv = getCSV(argv[1])
    modelFile = isModel(argv[2])
    if(os.path.isdir(argv[1]) and not modelFile and not errorWithArgs):
        train(argv[1], argv[2])
    elif(csv and modelFile and not errorWithArgs):
        predict(csv, argv[2])
    else:
        print("Error with arguments " + " ".join(argv[1:]))
        print("If trying to train,   arguments should be <trainFolder> <modelName>")        
        print("If trying to predict, arguments should be <targtFolder> <modelName>")

if __name__ == '__main__':
    main(sys.argv)