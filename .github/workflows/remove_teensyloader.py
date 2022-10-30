#!/usr/bin/python

import sys
import re

filepath = sys.argv[1]  # path of the file to parse
lines = []  # list of lines from the file
removed = False  # flag for if the line was removed from the file

with open(filepath, 'r') as f:
	lines = f.readlines()

with open(filepath, 'w') as f:
	# matching "postbuild" hooks of any number which call the "teensy_post_compile" application
	pattern = re.compile("recipe\.hooks\.postbuild\.[0-9]\.pattern=\"{teensytools\.path}teensy_post_compile\"")
	for line in lines:
		if not pattern.match(line):
			f.write(line)
		else:
			print("Removing line '{}'".format(line.strip('\n')))
			removed = True

if not removed:
	raise RuntimeError("Did not find a matching line to remove in \"{}\"".format(filepath))
