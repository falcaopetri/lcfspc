import subprocess
import sys
import json

with open("example.cfg") as f:
    config = json.load(f)

exe = "./a.out"
args = "{} {} {} {} {} {} {}"
cmd = "{} " + args

print_interval = config["print_interval"]

processes = []

for process_cfg in config["processes"]:
    data = process_cfg["data"]
    types = process_cfg["types"]
    id = len(processes)

    process = subprocess.Popen(cmd.format(exe, id, print_interval, data, *types),
                                    shell=True, stdout=subprocess.PIPE)
    processes.append(process)

labels = [ "Process ID", "Tempo decorrido", "User time", "System time",
                "Porcentagem de uso da cpu", "Porcentagem de uso da cpu por thread" ]

n_active_processes = len(processes)

# Reference: http://www.cyberciti.biz/faq/python-run-external-command-and-get-output/
while n_active_processes != 0:
    for process in processes:
        output = process.stdout.readline()

        if output == '' and process.poll() is not None:
            n_active_processes -= 1
            processes.remove(process)
            continue

        if output:
            values = output.strip().split(" ")

            for i in range(len(labels)):
                print "{}: {}".format(labels[i], values[i])
