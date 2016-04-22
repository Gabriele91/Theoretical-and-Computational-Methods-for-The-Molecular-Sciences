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


def pretty_return(command, ret_code, stdout, stderr):
    string = "|" + Colors.HEADER + "------- CMD ------" + Colors.ENDC + "\n"
    string += "| " + Colors.HEADER + \
        str(command) + Colors.ENDC + "\n| -> returned: " + \
        Colors.HEADER + str(ret_code) + Colors.ENDC + "\n"
    string += "|" + Colors.OKGREEN + "----- STDOUT -----" + Colors.ENDC + "\n"
    for line in stdout.strip().split("\n"):
        string += str("| " + line + "\n")
    string += "|" + Colors.FAIL + "----- STDERR -----" + Colors.ENDC + "\n"
    for line in stderr.strip().split("\n"):
        string += str("| " + line + "\n")
    string += "|------------------\n"
    print(string)
    return string


def call_command(command):
    proc = subprocess.Popen(
        command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    out, err = proc.communicate()
    return command, proc.returncode, out, err


class Submit(Process):

    def __init__(self, title, exec_name, num_nodes, num_processes, output_dir, input_args):
        super(Submit, self).__init__()
        self.exec_name = exec_name
        self.job_name = self.exec_name.replace("/", "_")
        self.proc_id = None
        self.num_nodes = num_nodes
        self.num_processes = num_processes
        self.output_dir = output_dir
        self.input_args = input_args
        self.title = title
        self.cpu_time_used = ''

    def run(self):
        print(Colors.OKGREEN + "----- Start submit -> %s -----" % self.title + Colors.ENDC)
        # ----- MAKE SUBMIT -----
        print(
            Colors.WARNING + "- Make submit command" + Colors.ENDC, end=" -> ")
        sys.stdout.flush()
        try:
            with open(os.path.join(BIN_DIR, "sub_template.sh"), "r") as template:
                with open("cur_sub.sh", "w") as cur_sub:
                    cur_sub.write(
                        template.read()
                        .replace("%EXE_PATH%", os.path.join(SOURCES, self.exec_name))
                        .replace("%JOB_NAME%", self.job_name)
                        .replace("%NODES%", str(self.num_nodes))
                        .replace("%PROCESSES%", str(self.num_processes))
                        .replace("%INPUTARGS%", "\""+"\" \"".join(self.input_args)+"\"")
                        .replace("%OUTDIR%", os.path.join(SOURCES, self.output_dir)))
        except IOError:
            print(Colors.FAIL + "FAIL" + Colors.ENDC)
            return
        print(Colors.OKGREEN + "OK" + Colors.ENDC)

        # ----- SUBMIT JOB -----
        print(Colors.WARNING + "- Submit job on {0} node{1} with {2} process{3}".format(
            self.num_nodes, 's' if self.num_nodes > 1 else '',
            self.num_processes, 'es' if self.num_processes > 1 else '') + Colors.ENDC, end=" -> ")
        sys.stdout.flush()
        command, ret_code, stdout, stderr = call_command(
            "qsub ./cur_sub.sh")
        if ret_code != 0:
            print(Colors.FAIL + "FAIL" + Colors.ENDC)
            pretty_return(command, ret_code, stdout, stderr)
            return
        else:
            print(Colors.OKGREEN + "OK" + Colors.ENDC)

        process_id = stdout.strip()
        try:
            # ----- WATCH JOB -----
            job_state = {
                'C': "Job is completed  ",
                'E': "Job is exiting    ",
                'H': "Job is held       ",
                'Q': "job is queued     ",
                'R': "job is running    ",
                'T': "job is being moved",
                'W': "job is waiting    ",
                'S': "job is suspend    "
            }
            print(
                Colors.WARNING + "- Processing job -> {0}".format(process_id) + Colors.ENDC)
            command, ret_code, stdout, stderr = call_command("qstat")
            last_string = ""
            proc_pass = 0
            ended = False
            bar = ["▌", "▀", "▐", "▄"]
            while stdout.strip() != "" and ret_code == 0 and not ended:
                command, ret_code, stdout, stderr = call_command("qstat")
                cur_status = ""
                for line in stdout.strip().split("\n"):
                    data = [char for char in line.split(" ") if char]
                    if line.find(process_id) != -1:
                        cur_status = job_state.get(
                            data[-2], "undefined")
                        if cur_status == job_state['E']:
                            self.cpu_time_used = data[-3]
                            ended = True
                if cur_status == "":
                    ended = True
                new_string = Colors.WARNING + \
                    "  - status: {0}".format(cur_status) + Colors.ENDC
                if new_string != last_string:
                    last_string = new_string
                else:
                    new_string += bar[proc_pass]
                    proc_pass += 1
                    proc_pass %= 4
                print(new_string, end="\r")
                sys.stdout.flush()
                time.sleep(0.15)
            print(Colors.WARNING + "\n  - done" + Colors.ENDC)
            if ret_code != 0:
                print(Colors.FAIL + "FAIL" + Colors.ENDC)
                pretty_return(command, ret_code, stdout, stderr)
                return
        except KeyboardInterrupt:
            print("\r" +
                  Colors.HEADER + "!!! REQUESTED INTERRUPT !!!" + Colors.ENDC)
            command, ret_code, stdout, stderr = call_command(
                "qdel " + process_id.split(".")[0])
            if ret_code != 0:
                print(Colors.FAIL + "FAIL" + Colors.ENDC)
                pretty_return(command, ret_code, stdout, stderr)
                return
            else:
                print(
                    Colors.HEADER + "!!! JOB successful aborted !!!" + Colors.ENDC)
                return

        print(Colors.OKGREEN +
              "-> CPU USED: {0} <-".format(self.cpu_time_used) + Colors.ENDC)
        print(Colors.OKGREEN + "------ End submit ------" +
              Colors.ENDC, end="\n\n")
        f_stdout, f_stderr = [
            self.job_name + "." + type_ + process_id.split(".")[0] for type_ in ["o", "e"]]
        try:
            os.mkdir("log")
        except OSError as err:
            if err.errno == 17:
                pass
            else:
                raise err
        while not os.path.isfile(f_stdout) and not os.path.isfile(f_stderr):
            pass
        time.sleep(1.5)
        shutil.move(f_stdout, "log")
        shutil.move(f_stderr, "log")
        print(
            Colors.OKGREEN + "!!! Output files moved in log folder !!!" + Colors.ENDC)
        print(Colors.OKGREEN + "@ Results: " + Colors.ENDC, end="\n\n")
        print(Colors.OKGREEN + "----- STDOUT -----" + Colors.ENDC)
        with open(str("log/" + f_stdout), "r") as cur_file:
            print(cur_file.read())
        print(Colors.OKGREEN + "----- END STDOUT -----" + Colors.ENDC)
        print(Colors.FAIL + "----- STDERR -----" + Colors.ENDC)
        with open(str("log/" + f_stderr), "r") as cur_file:
            print(cur_file.read())
        print(Colors.FAIL + "----- END STDERR -----" + Colors.ENDC)


def main():
    # test
    # command, ret_code, out, err = call_command("cd /home/user")
    # print(pretty_return(command, ret_code, out, err))

    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description="""Launch an MPI project

    NOTE: if you want to chage the source directory or
          you have to set it use:

        $ git sub setSource /path/of/your/sources
""")

    parser.add_argument('execName', metavar='execName', type=str,
                        help='Name of the bin to launch')
    parser.add_argument(
        'input_args', nargs=argparse.REMAINDER, metavar='args', default='',
        help='input arguments for the project executable')
    parser.add_argument('-n', '--nodes', metavar='N', type=int,
                        help='number of nodes needed', nargs='?',
                        default=4)
    parser.add_argument('-p', '--processes', metavar='P', type=int,
                        help='number of processes per node', nargs='?',
                        default=1)
    parser.add_argument('-o', '--output', metavar='outDir', type=str,
                        help='output directory', default="output/")
    parser.add_argument('-t', '--title', metavar='title', type=str,
                        help='title submit', default="")

    args = parser.parse_args()

    proc = Submit(args.title, args.execName, args.nodes, args.processes, args.output, args.input_args)
    os.system('clear')
    proc.run()
    if proc.is_alive():
        proc.join()
    sys.exit(0)

if __name__ == '__main__':
    main()