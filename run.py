import subprocess
import sys
import json
import time
import os
import signal
import atexit
from threading import Timer

with open("example.cfg") as f:
    config = json.load(f)

exe = "./a.out"
args = "{} {} {} {} {} {} {}"
cmd = "{} " + args

print_interval = config["print_interval"]

processes = []

def run_subprocess(process_cfg):
    data = process_cfg["data"]
    types = process_cfg["types"]
    id = process_cfg["id"]

    process = subprocess.Popen(cmd.format(exe, id, print_interval, data, *types),
                                    shell=True,
                                    stdout=subprocess.PIPE)
    process.id = id
    processes.append(process)

labels = [ "Process ID", "Tempo decorrido", "User time", "System time",
                "Porcentagem de uso da cpu", "Porcentagem de uso da cpu por thread" ]
files_labels = [ "run", "user", "system", "percentage" ]

output_directory = "output"
if not os.path.exists(output_directory):
    os.makedirs(output_directory)
output_files = [open(os.path.join(output_directory, fl + ".csv"), "w") for fl in files_labels]

n_files = len(files_labels)
n_processes = len(config["processes"])
# n_active_processes = len(config["processes"])

iteration = 0

timers = []
# Source: http://stackoverflow.com/a/19448255/6278885
def kill_child():
    for process in processes:
        os.kill(process.pid, signal.SIGTERM)

def cancel_timers():
    for timer in timers:
        timer.cancel()

def remove_tmp_files():
    for file in os.listdir("."):
        if file.startswith("tmp"):
            os.remove(file)

atexit.register(kill_child)
atexit.register(remove_tmp_files)
# it won't call atexit until all scheduled timers have finished
atexit.register(cancel_timers)

for i in range(len(config["processes"])):
    process_cfg = config["processes"][i]
    process_cfg["id"] = i

    delay = process_cfg.get("delay", 0)

    t = Timer(delay, run_subprocess, [process_cfg])
    timers.append(t)
    t.start()

# TODO exit after all processes have finished
# Reference: http://www.cyberciti.biz/faq/python-run-external-command-and-get-output/
while True:
    ordered_buff = [["0"] * n_processes] * n_files

    for process in processes:
        output = process.stdout.readline()

        if output == '' and process.poll() is not None:
            # n_active_processes -= 1
            processes.remove(process)
            continue

        if output:
            values = output.strip().split(" ")
            id = process.id

            for i in range(n_files):
                ordered_buff[i][id] = values[i+1]

            # for i in range(len(labels)):
            #     print "{}: {}".format(labels[i], values[i])

    iteration += 1
    for i in range(n_files):
        output_files[i].write(str(iteration))

        for v in ordered_buff[i]:
            output_files[i].write(",")
            output_files[i].write(v)

        output_files[i].write("\n")
        output_files[i].flush()
