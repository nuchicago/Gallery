#!/usr/bin/env python

import sqlite3
import argparse
import sys
import os
import samweb_cli
import threading
import time
import logging

def getDataGivenFileList(flist,r):
    #query SAM for each file in file list and gets run and subrun processed from meta data
    #puts that into dictionary rslist[run]=list_of_subruns
    dbdir="/uboone/data/uboonebeam/beamdb/"
    con=sqlite3.connect("%s/run.db"%dbdir)
    con.row_factory=sqlite3.Row
    cur=con.cursor()
    cur.execute("ATTACH DATABASE '%s/bnb.db' AS bnb"%dbdir)
    cur.execute("ATTACH DATABASE '%s/numi.db' AS numi"%dbdir)

    bnbwarn=False
    numiwarn=False

    samweb = samweb_cli.SAMWebClient(experiment='uboone')
    try:
        meta=samweb.getMetadataIterator(flist)
    except Exception as e:
        print "Failed to get metadata from SAM."
        print "Make sure to setup sam_web_client v2_1 or higher."
        print e
        sys.exit(0)

    missbnb={}
    missnumi={}
    mcount=0
    for m in meta:
        mcount+=1
        for rs in m['runs']:
            query="%s WHERE r.run=%i AND r.subrun=%i"%(dbquerybase, int(rs[0]),int(rs[1]))
            cur.execute(query)
            row=cur.fetchone()
            for k in r:
                if row[k] is not None:
                    r[k]+=row[k]
                elif k in bnbcols:
                    if rs[0] not in missbnb:
                        missbnb[rs[0]]=[rs[1]]
                    elif rs[1] not in missbnb[rs[0]]:
                        missbnb[rs[0]].append(rs[1])
                    bnbwarn=True
                elif k in numicols:
                    if rs[0] not in missnumi:
                        missnumi[rs[0]]=[rs[1]]
                    elif rs[1] not in missnumi[rs[0]]:
                        missnumi[rs[0]].append(rs[1])
                    numiwarn=True
    r['bnbwarn']=bnbwarn
    r['numiwarn']=numiwarn
    r['missbnb']=missbnb
    r['missnumi']=missnumi
    if mcount != len(flist):
        print "Warning! Did not get metadata for all files. Looped through %i files, but only got metadata for %i. Check list for repeats or bad file names."%(len(flist),mcount)
        logging.debug("Warning! Did not get metadata for all files.")

    con.close()
    return 

def getDataGivenRSList(rslist,r):
    dbdir="/uboone/data/uboonebeam/beamdb/"
    con=sqlite3.connect("%s/run.db"%dbdir)
    con.row_factory=sqlite3.Row
    cur=con.cursor()
    cur.execute("ATTACH DATABASE '%s/bnb.db' AS bnb"%dbdir)
    cur.execute("ATTACH DATABASE '%s/numi.db' AS numi"%dbdir)

    bnbwarn=False
    numiwarn=False

    missbnb={}
    missnumi={}
    mcount=0
    for rsrow in rslist:
        rs=rsrow.split(" ")
        query="%s WHERE r.run=%i AND r.subrun=%i"%(dbquerybase, int(rs[0]),int(rs[1]))
        cur.execute(query)
        row=cur.fetchone()
        for k in r:
            if row[k] is not None:
                r[k]+=row[k]
            elif k in bnbcols:
                if rs[0] not in missbnb:
                    missbnb[rs[0]]=[rs[1]]
                elif rs[1] not in missbnb[rs[0]]:
                    missbnb[rs[0]].append(rs[1])
                bnbwarn=True
            elif k in numicols:
                if rs[0] not in missnumi:
                    missnumi[rs[0]]=[rs[1]]
                elif rs[1] not in missnumi[rs[0]]:
                    missnumi[rs[0]].append(rs[1])
                numiwarn=True

    r['bnbwarn']=bnbwarn
    r['numiwarn']=numiwarn
    r['missbnb']=missbnb
    r['missnumi']=missnumi

    con.close()
    return 

def getDataGivenRunSubrun(run,subrun,r):
    logging.debug("getDataGivenRunSubrun called.")
    dbdir="/uboone/data/uboonebeam/beamdb/"
    con=sqlite3.connect("%s/run.db"%dbdir)
    con.row_factory=sqlite3.Row
    cur=con.cursor()
    cur.execute("ATTACH DATABASE '%s/bnb.db' AS bnb"%dbdir)
    cur.execute("ATTACH DATABASE '%s/numi.db' AS numi"%dbdir)
    query="%s WHERE r.run=%i AND r.subrun=%i"%(dbquerybase,run,subrun)
    cur.execute(query)
    row=cur.fetchone()
    con.close()
    bnbwarn=False
    numiwarn=False
    for k in r:
        if row[k] is not None:
            r[k]+=row[k]
        elif k in bnbcols:
            bnbwarn=True
        elif k in numicols:
            numiwarn=True

    r['bnbwarn']=bnbwarn
    r['numiwarn']=numiwarn
    return

def getDataGivenRun(run,r):
    dbdir="/uboone/data/uboonebeam/beamdb/"
    con=sqlite3.connect("%s/run.db"%dbdir)
    con.row_factory=sqlite3.Row
    cur=con.cursor()
    cur.execute("ATTACH DATABASE '%s/bnb.db' AS bnb"%dbdir)
    cur.execute("ATTACH DATABASE '%s/numi.db' AS numi"%dbdir)
    query="%s WHERE r.run=%i"%(dbquerybase,run)
    cur.execute(query)
    row=cur.fetchone()
    con.close()
    bnbwarn=False
    numiwarn=False
    for k in r:
        if row[k] is not None:
            r[k]+=row[k]
        elif k in bnbcols:
            bnbwarn=True
        elif k in numicols:
            numiwarn=True

    r['bnbwarn']=bnbwarn
    r['numiwarn']=numiwarn
    return

def getDataGivenWhere(where,r):
    dbdir="/uboone/data/uboonebeam/beamdb/"
    con=sqlite3.connect("%s/run.db"%dbdir)
    con.row_factory=sqlite3.Row
    cur=con.cursor()
    cur.execute("ATTACH DATABASE '%s/bnb.db' AS bnb"%dbdir)
    cur.execute("ATTACH DATABASE '%s/numi.db' AS numi"%dbdir)
    wherec=" "+where
    for kw in [' run',' subrun',' begin_time',' end_time']:
        wherec=wherec.replace(kw," r."+kw.lstrip())
    query="%s WHERE %s"%(dbquerybase,wherec)
    cur.execute(query)
    bnbwarn=False
    numiwarn=False
    row=cur.fetchone()
    con.close()
    for k in r:
        if row[k] is not None:
            r[k]+=row[k]
        elif k in bnbcols:
            bnbwarn=True
        elif k in numicols:
            numiwarn=True

    r['bnbwarn']=bnbwarn
    r['numiwarn']=numiwarn
    return

def getFileListFromDefinition(defname):
    samweb = samweb_cli.SAMWebClient(experiment='uboone')
    flist=[]
    try:
        flist=samweb.listFiles(defname=args.defname)
    except:
        print "Failed to get the list of files in %s"%args.defname
        sys.exit(1)

    flist.sort()
    if (not args.noheader):
        print "Definition %s contains %i files"%(defname,len(flist))

    return flist

def getListFromFile(fname):
    logging.debug("getListFromFile called.")
    flist=[]
    try:
        finp=open(fname)
        for l in finp:
            flist.append(os.path.basename(l.strip("\n")))
    except Exception as e:
        print "Failed to read %s"%fname
        print e
        sys.exit(1)

    if (not args.noheader):
        print "Read %i lines from %s"%(len(flist),fname)

    return flist
        
def getListForThreads(flist, nthreads):
    flist_thread=[]
    while len(flist)>0:
        for ith in range(0,nthreads):
            if (len(flist)==0):
                break
            if len(flist_thread)>ith:
                flist_thread[ith].append(flist.pop())
            else:
                flist_thread.append([flist.pop()])
    return flist_thread

def getDBQueryBase(cols):
    dbq="SELECT "
    addBNB=False
    addNuMI=False
    for var in cols:
        var=var.lower()
        if "ext" in var:
            dbq+="SUM(r.EXTTrig) AS EXT,"
        elif "gate1" in var:
            dbq+="SUM(r.Gate1Trig) AS Gate1,"
        elif "gate2" in var:
            dbq+="SUM(r.Gate2Trig) AS Gate2,"
        elif "e1dcnt" in var:
            if "wcut" in var:
                dbq+="SUM(b.E1DCNT) AS E1DCNT_wcut,"
                addBNB=True
            else:
                dbq+="SUM(r.E1DCNT) AS E1DCNT,"
        elif "tor860" in var:
            if "wcut" in var:
                dbq+="SUM(b.tor860) AS tor860_wcut,"
                addBNB=True
            else:
                dbq+="SUM(r.tor860) AS tor860,"
        elif "tor875" in var:
            if "wcut" in var:
                dbq+="SUM(b.tor875) AS tor875_wcut,"
                addBNB=True
            else:
                dbq+="SUM(r.tor875) AS tor875,"
        elif "ea9cnt" in var:
            if "wcut" in var:
                dbq+="SUM(n.EA9CNT) AS EA9CNT_wcut,"
                addNuMI=True
            else:
                dbq+="SUM(r.EA9CNT) AS EA9CNT,"
        elif "tor101" in var:
            if "wcut" in var:
                dbq+="SUM(n.tor101) AS tor101_wcut,"
                addNuMI=True
            else:
                dbq+="SUM(r.tor101) AS tor101,"
        elif "tortgt" in var:
            if "wcut" in var:
                dbq+="SUM(n.tortgt) AS tortgt_wcut,"
                addNuMI=True
            else:
                dbq+="SUM(r.tortgt) AS tortgt,"
    dbq=dbq[0:-1]
    dbq+=" FROM runinfo AS r"
    if addBNB:
        dbq+=" LEFT OUTER JOIN bnb.bnb AS b ON r.run=b.run AND r.subrun=b.subrun"
    if addNuMI:
        dbq+=" LEFT OUTER JOIN numi.numi AS n ON r.run=n.run AND r.subrun=n.subrun"

    return dbq



parser = argparse.ArgumentParser(description='Run info.',formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("-r", "--run", type=int,
                    help="Specify run.")
parser.add_argument("-s", "--subrun", type=int,
                    help="Specify subrun (if not specified, sum for all subruns is returned.")
parser.add_argument("-w", "--where", type=str,
                    help="SQL query. For exampe 'run>4500 AND run<4550'")
parser.add_argument("-d", "--defname", type=str,
                    help="Use SAM definition to get run/subrun list.")
parser.add_argument("--file-list", type=str,
                    help="File with a list of files.")
parser.add_argument("--run-subrun-list", type=str,
                    help="File with a list of runs and subruns. (each line giving run subrun")
parser.add_argument("-f", "--format", type=str,
                    default="run:subrun:EXT:Gate2:E1DCNT:tor860:tor875:E1DCNT_wcut:tor860_wcut:tor875_wcut",
                    help="Output format.")
parser.add_argument("--format-bnb", action="store_true",
                    help="Output format, include all BNB variables.")
parser.add_argument("--format-numi", action="store_true",                   
                    help="Output format, include all NuMI variables.")
parser.add_argument("--format-all", action="store_true",
                    help="Output format, print all data.")
parser.add_argument("--nthreads", type=int, default=4,
                    help="Run in multiple threads to speed up.")
parser.add_argument("--noheader",action="store_true",
                    help="Don't print table header.")


args = parser.parse_args()

if (args.run is None and args.defname is None and args.where is None and args.file_list is None and args.run_subrun_list is None):
    print "Need to specify run (subrun), or SAM definition, or SQL query, or list of files, or list of runs and subruns."
    parser.print_help()
    sys.exit(0)

dbdir="/uboone/data/uboonebeam/beamdb/"
logging.basicConfig(filename="%s/dbquery.log"%dbdir,level=logging.DEBUG,format='%(asctime)s '+str(os.getpid())+' ['+os.environ['USER']+'] %(message)s', datefmt='%m/%d/%Y %H:%M:%S')
logging.debug(" ".join(sys.argv))

res={}
#cols has to match the dbquerybase (queries in getDataGivenX)
cols=[]
if args.format_bnb:
    cols="run:subrun:Gate2:E1DCNT:tor860:tor875:E1DCNT_wcut:tor860_wcut:tor875_wcut".split(":")
elif args.format_numi:
    cols="run:subrun:Gate1:EA9CNT:tor101:tortgt:EA9CNT_wcut:tor101_wcut:tortgt_wcut".split(":")
elif args.format_all:
    cols="run:subrun:EXT:Gate2:E1DCNT:tor860:tor875:E1DCNT_wcut:tor860_wcut:tor875_wcut:Gate1:EA9CNT:tor101:tortgt:EA9CNT_wcut:tor101_wcut:tortgt_wcut".split(":")
else:
    cols=args.format.split(":")


numicols=['EA9CNT_wcut', 'tor101_wcut', 'tortgt_wcut']
bnbcols =['E1DCNT_wcut', 'tor860_wcut', 'tor875_wcut']
allowedcols=['run','subrun','EXT','Gate1','Gate2','e1dcnt','tor860','tor875','ea9cnt','tor101','tortgt']
allowedcols.extend(bnbcols)
allowedcols.extend(numicols)

if not set(map(str.lower, cols))<=set(map(str.lower,allowedcols)):
    print "Bad format. Allowed columns (case insensitive):"
    print allowedcols
    sys.exit(0)

dbquerybase=getDBQueryBase(cols)
logging.debug(dbquerybase)

for icol in cols:
    if "run" not in icol and "time" not in icol:
        res[icol]=0

if args.defname is not None or args.file_list is not None:
    while "run" in cols: cols.remove('run')
    while "subrun" in cols: cols.remove('subrun')
    flist=[]
    if args.defname is not None:
        flist=getFileListFromDefinition(args.defname)
    else:
        flist=getListFromFile(args.file_list)

    flist_thread=getListForThreads(flist, args.nthreads)

    nthreads=min(len(flist_thread),args.nthreads)
    logging.debug("Running in %i thread(s)"%nthreads)    
    threads=[]
    rthread=[]
    for ith in range(0,nthreads):
        rthread.append(res.copy())
        t=threading.Thread(target=getDataGivenFileList, args=(flist_thread[ith],rthread[ith]))
        threads.append(t)
        t.start()

    res['bnbwarn']=False
    res['numiwarn']=False
    res['missbnb']={}
    res['missnumi']={}

    for ith in range(0,nthreads):
        threads[ith].join()
        for k in res:
            if not isinstance(res[k],dict):
                res[k]+=rthread[ith][k]
            else:
                for rk in rthread[ith][k]:
                    if rk in res[k]:
                        res[k][rk].extend(rthread[ith][k][rk])
                    else:
                        res[k][rk]=rthread[ith][k][rk]

elif args.run_subrun_list:
    while "run" in cols: cols.remove('run')
    while "subrun" in cols: cols.remove('subrun')
    rslist=getListFromFile(args.run_subrun_list)
    rslist_thread=getListForThreads(rslist, args.nthreads)

    nthreads=min(len(rslist_thread),args.nthreads)
    logging.debug("Running in %i thread(s)"%nthreads)    
    threads=[]
    rthread=[]

    for ith in range(0,nthreads):
        rthread.append(res.copy())
        t=threading.Thread(target=getDataGivenRSList, args=(rslist_thread[ith],rthread[ith]))
        threads.append(t)
        t.start()

    res['bnbwarn']=False
    res['numiwarn']=False
    res['missbnb']={}
    res['missnumi']={}

    for ith in range(0,nthreads):
        threads[ith].join()
        for k in res:
            if not isinstance(res[k],dict):
                res[k]+=rthread[ith][k]
            else:
                for rk in rthread[ith][k]:
                    if rk in res[k]:
                        res[k][rk].extend(rthread[ith][k][rk])
                    else:
                        res[k][rk]=rthread[ith][k][rk]

elif args.where is not None:
    while "run" in cols: cols.remove('run')
    while "subrun" in cols: cols.remove('subrun')
    getDataGivenWhere(args.where,res)
else:
    if args.subrun is not None:
        getDataGivenRunSubrun(args.run,args.subrun,res)
    else:
        while "subrun" in cols: cols.remove('subrun')
        getDataGivenRun(args.run,res)


header=""
output=""
for var in cols:
    header+="%14s"%var
    if var=="run":
        output+="%14i"%args.run
    elif var=="subrun":
        output+="%14i"%args.subrun
    elif "tor" in var:
        output+="%14.4g"%(res[var]*1e12)
    else:
        output+="%14i"%res[var]

if not args.noheader:
    print header
print output

logging.debug(output)

mess=None
if res['bnbwarn'] and res['numiwarn']:
    mess="Warning!! NuMI and BNB data for some of the requested runs/subruns is not in the database.\n"
elif res['bnbwarn']:
    mess="Warning!! BNB data for some of the requested runs/subruns is not in the database.\n"
elif res['numiwarn']:
    mess="Warning!! NuMI data for some of the requsted runs/subruns is not in the database.\n"


if res['bnbwarn'] and 'missbnb' in res:
    mess+="Runs missing BNB data (number of subruns missing the data): "
    for k in res['missbnb']:
        mess+="%i (%i),"%(int(k),len(res['missbnb'][k]))
    mess.rstrip(",")
    mess+="\n"
if res['numiwarn'] and 'missnumi' in res:
    mess+="Runs missing NuMI data (number of subruns missing the data): "
    for k in res['missnumi']:
        mess+="%i (%i),"%(int(k),len(res['missnumi'][k]))
    mess.rstrip(",")
    mess+="\n"


if mess:
    print mess 
    logging.warning(mess)

