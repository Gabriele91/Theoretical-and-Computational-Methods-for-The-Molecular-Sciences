#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals
from multiprocessing import Process, Array, Queue
from time import sleep
import os
import subprocess
import ConfigParser
import sys
BIN_DIR = os.path.join(os.getenv("HOME"), 'bin')
CONFIG = ConfigParser.ConfigParser()
CONFIG.read(os.path.join(BIN_DIR, 'sub.cfg'))
SOURCES = CONFIG.get('sub', 'source_dir')


class Colors(object):
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def call_command(command):
    proc = subprocess.Popen(
        command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    out, err = proc.communicate()
    return [command, proc.returncode, out, err]

class Compile(Process):

    def __init__(self,result,flag):
        super(Compile, self).__init__()
        self.result = result
        self.flag = flag

    def run(self):
        self.result.put(call_command(os.path.join(SOURCES,"intel_build.sh")+" "+self.flag))

def print_output(ret):
    if ret[1] != 0:
        print(Colors.FAIL + "------- FAIL TO COMPILE -------" + Colors.ENDC )
    if len(ret[3]) != 0:
        print(Colors.WARNING + "------- OUPUT ERROR -------" + Colors.ENDC )
        print(ret[2])
        print(ret[3])
    if len(ret[2]) != 0:
        print(Colors.OKGREEN + "------- SUCCESS TO COMPILER -------" + Colors.ENDC )

def compile(flag):
    os.system('clear')
    bar = ["▌", "▀", "▐", "▄"]
    proc_pass = 0
    #result
    ret_q = Queue()
    #parallel compile
    process = Compile(ret_q,flag)
    process.start()
    #wait...
    while process.is_alive():
        new_string = bar[proc_pass]
        proc_pass += 1
        proc_pass %= 4
        print(Colors.OKBLUE + new_string + Colors.WARNING +" -- COMPILING" + Colors.ENDC, end="\r")
        sleep(0.002)
    process.join()
    #get first element
    return ret_q.get()

def main():
    COMPILE_FLAGS       = 0
    COMPILE_FLOAT       = 1
    COMPILE_DOUBLE      = 2
    COMPILE_LONG_DOUBLE = 4
    COMPILE_CLEAN       = 8
    argv = sys.argv[2:]
    if len(argv):    
        for arg in argv:
            if arg.upper() == "DOUBLE":
                COMPILE_FLAGS |= COMPILE_DOUBLE
            elif arg.upper() == "FLOAT":
                COMPILE_FLAGS |= COMPILE_FLOAT
            elif arg.upper() == "LONG_DOUBLE" or arg.upper() == "LONG-DOUBLE":
                COMPILE_FLAGS |= COMPILE_LONG_DOUBLE
            elif arg.upper() == "CLEAN":
                COMPILE_FLAGS |= COMPILE_CLEAN
            elif arg.upper() == "ALL":
                COMPILE_FLAGS |= COMPILE_FLOAT | COMPILE_DOUBLE | COMPILE_LONG_DOUBLE 
            else:
                print("Unknow command: "+arg)
                exit(-1)
    else:
        COMPILE_FLAGS = COMPILE_FLOAT | COMPILE_DOUBLE | COMPILE_LONG_DOUBLE | COMPILE_CLEAN
    #save all return
    returns = {}
    #compile
    if COMPILE_FLAGS & COMPILE_FLOAT:
        returns[COMPILE_FLOAT] = compile("FLOAT")
    if COMPILE_FLAGS & COMPILE_DOUBLE:
        returns[COMPILE_DOUBLE] = compile("DOUBLE")
    if COMPILE_FLAGS & COMPILE_LONG_DOUBLE:
        returns[COMPILE_LONG_DOUBLE] = compile("LONG_DOUBLE")
    
    for key in returns:
        if key & COMPILE_FLOAT: print(Colors.OKBLUE+ "------- FLOAT COMPILING OUTPUT -------" +Colors.ENDC)
        if key & COMPILE_DOUBLE: print(Colors.OKBLUE+ "------- DOUBLE COMPILING OUTPUT -------" +Colors.ENDC)
        if key & COMPILE_LONG_DOUBLE: print(Colors.OKBLUE+ "------- LONG DOUBLE COMPILING OUTPUT -------" +Colors.ENDC)
        print_output(returns[key])

    ret = call_command("cd "+SOURCES+ (" && make clean" if COMPILE_FLAGS & COMPILE_CLEAN else ""))


if __name__ == '__main__':
    main()
