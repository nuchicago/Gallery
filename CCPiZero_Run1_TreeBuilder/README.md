# Actual Analysis Code:

## To Setup: 

` source setup_gallery`


## To Build:

` make`


## To Run (wild cards allowed): 

` ./make_tree /path/to/artroot_files.root`

## To submit to grid (WARNING: With great power comes great responsibility)

`setup jobsub_client`

`submitJobs.py -f 1 -i $(A) -l $(B) -o $(C)`

A = the file list you want to run over

B = the length of that file list (1 file per job), if you can make this larger

C = path to your output, *SHOULD BE ON Scratch*


![alt text](http://cultofthepartyparrot.com/parrots/partyparrot.gif)