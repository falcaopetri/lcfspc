import subprocess
import sys
import json
import time
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
                                    shell=True, stdout=subprocess.PIPE)
    process.id = id
    processes.append(process)

for i in range(len(config["processes"])):
    process_cfg = config["processes"][i]
    process_cfg["id"] = i

    delay = process_cfg.get("delay", 0)

    Timer(delay, run_subprocess, [process_cfg]).start()

labels = [ "Process ID", "Tempo decorrido", "User time", "System time",
                "Porcentagem de uso da cpu", "Porcentagem de uso da cpu por thread" ]

n_processes = len(config["processes"])
# n_active_processes = len(config["processes"])

output_file = open("output.csv", "w")

iteration = 0

# TODO exit after all processes have finished
# Reference: http://www.cyberciti.biz/faq/python-run-external-command-and-get-output/
while True:
    ordered_buff = ["0"] * n_processes

    for process in processes:
        output = process.stdout.readline()

        if output == '' and process.poll() is not None:
            # n_active_processes -= 1
            processes.remove(process)
            continue

        if output:
            values = output.strip().split(" ")
            id = process.id
            ordered_buff[id] = values[5]

            for i in range(len(labels)):
                print "{}: {}".format(labels[i], values[i])

    output_file.write(str(iteration))
    iteration += 1

    for v in ordered_buff:
        output_file.write(",")
        output_file.write(v)

    output_file.write("\n")
    output_file.flush()
