---ABOUT---
The key files pertaining to dissertation project.

Compile contains compiled Video to Time Series Converter exe and TSAnalysis.py including required files to execute.
Source contains source material of VTSC.exe.
Data contains sample video files and model, as well as a directory to train on.

---EXECUTION---	
VTSC should be executed from a command line in a directory with the supplemental files (net, ConfigFile.params, etc) included.
TSAnalysis depends on numpy, pandas and TSLearn to be in the running environment alongside Python 2.7.

Files passed as arguments should be in the same directory or lower of VTSC.exe
This is easiest achieved by copying the data folder into compile and working from compile in terminal

Correct form videos must include _C_ in the filename
Incorrect form videos must include _I_ in the filename

To run....
Classification:	 VTSC.exe <VideoFile.mp4>   <ModelFile.model>
Training: 	 VTSC.exe <Video Directory> <Model Name>

---FURTHER---
Due to licensing issues prohibiting redistribution, the VNect source files are unable to be included in this archive, 
these can be requested from the researchers at:
http://gvv.mpi-inf.mpg.de/projects/VNect/