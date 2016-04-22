#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import print_function, unicode_literals
from multiprocessing import Process
import os
import sys
import subprocess
import argparse
import time
import shutil
from math import ceil

TYPE_APPS = ["serial","dlb","slb"]
DLB_FACTORS = [ 0.25, 0.5, 0.75, 1.0 ]
PRECISION_VARS  = ["float","double","long-double"]
KERNELS   = ["newton","schroeder","schroeder4","hally","hally4"]
TEST_TEMPLATE ='-n %d -p %d -o %s bin/%s-%s --polynomial "%s" --kernel %s --qxp %d %d --mxn %d %d --factor %f --iteration %d --zoom %s'
TEST_QUEUE =[ 
                { 
                  "size" : (2,2),
                  "image-size" : (2500,2500),
                  "polynomial" : "(6+2i)z^0+(3+9i)z^1+(3+78i)z^2+(4+9i)z^3+(8+9i)z^4+z^5+z^8",
                  "zoom" : 0.003,
                  "iteration" : 20
                },
                { 
                  "size" : (4,4),
                  "image-size" : (1000,1000),
                  "polynomial" : "z^3+1",
                  "zoom" : 0.01,
                  "iteration" : 10
                }
            ]

def build_test_cmp(type_app,precision, factor, kernel,test):
    return "git sub " + "-t \"%s-%s-%s-%s\" " % (
                            type_app,
                            precision,
                            factor,
                            kernel
                            ) + TEST_TEMPLATE % ( ceil( test["size"][0]*test["size"][1] / 2. ),
                             2, 
                             "output/"+precision, 
                             type_app, precision,
                             test["polynomial"],
                             kernel,
                             test["size"][0],       test["size"][1],
                             test["image-size"][0], test["image-size"][1],
                             factor,
                             test["iteration"],
                             test["zoom"] )

def main():
    os.system("cd .. && mkdir -p output")
    os.system("cd .. && mkdir -p output/float")
    os.system("cd .. && mkdir -p output/double")
    os.system("cd .. && mkdir -p output/long-double")

    test_to_do = []

    for test in TEST_QUEUE:
        for type_app in TYPE_APPS:
            for precision in PRECISION_VARS:
                if type_app == 'dlb':
                    for factor in DLB_FACTORS:
                        test_to_do.append(build_test_cmp(type_app,precision,factor,'newton', test))
                else:
                    test_to_do.append(build_test_cmp(type_app,precision,1.0,'newton', test))
    for value in test_to_do:
        if os.system("cd .. && "+value) == 2:
          exit(2)

if __name__ == '__main__':
    main()