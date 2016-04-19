#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals
from multiprocessing import Process, Array, Queue
from time import sleep
import os
import subprocess
import ConfigParser

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

    def __init__(self,result):
        super(Compile, self).__init__()
        self.result = result

    def run(self):
        self.result.put(call_command(os.path.join(SOURCES,"intel_build.sh")))


def main():
    os.system('clear')
    bar = ["▌", "▀", "▐", "▄"]
    proc_pass = 0
    #result
    ret_q = Queue()
    #parallel compile
    process = Compile(ret_q)
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
    ret = ret_q.get()

    if ret[1] != 0:
        print(Colors.FAIL + "------- FAIL TO COMPILE -------" + Colors.ENDC )
    if len(ret[3]) != 0:
        print(Colors.WARNING + "------- OUPUT ERROR -------" + Colors.ENDC )
        print(ret[2])
        print(ret[3])
    if len(ret[2]) != 0:
        print(Colors.OKGREEN + "------- SUCCESS TO COMPILER -------" + Colors.ENDC )

    ret = call_command("cd "+SOURCES+" && make clean")


if __name__ == '__main__':
    main()
