#
# This is only a simple script.
# It runs the following commands:
#	"cd threads"
#	"./nachos " + "xxx" ("xxx" are main args)
#	"cd .."
#

import os
import sys

def help_menu():
	print ("Please follow the rules:")
	print ("\tpython onekeyrun.py [-m or -n] {your nachos arguments}")
	print ("\t-m: to first make and then run nachos;")
	print ("\t-n: to run nachos without make")
	print ()
	print ("Example: python onekeyrun.py -m -q 1")
	print ("\tThe script would then make and run na")

if __name__ == "__main__":

	cmd = "./nachos"
	for arg in sys.argv[2:]:
		cmd += (" " + arg)

	if len(sys.argv) < 3:
		help_menu()
		exit()

	if sys.argv[1] == "-m":
		print ("********************")
		print ("Make Mode")
		print ("********************")
		os.system("make;cd threads;"+cmd+";cd ..")
	elif sys.argv[1] == "-n":
		print ("********************")
		print ("Without-Make Mode")
		print ("********************")
		os.system("cd threads;"+cmd+";cd ..")
	else:
		help_menu()