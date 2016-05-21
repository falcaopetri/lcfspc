import subprocess
import sys

n_processes = 5
exe = "./a.out"
args = "{} {} {} {} {} {} {}"
cmd = "{} " + args

processes = []

for i in range(n_processes):
    process = subprocess.Popen(cmd.format(exe, i, 2, 10000, 5, 0, 0, 0),
                                    shell=True, stdout=subprocess.PIPE)
    processes.append(process)

labels = [ "Process ID", "Tempo decorrido", "User time", "System time",
                "Porcentagem de uso da cpu", "Porcentagem de uso da cpu por thread"]

n_active_processes = n_processes

# Reference: http://www.cyberciti.biz/faq/python-run-external-command-and-get-output/
while n_active_processes != 0:
    print n_active_processes
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
