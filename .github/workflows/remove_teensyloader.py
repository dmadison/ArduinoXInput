#!/usr/bin/python

import sys

filepath = sys.argv[1]
lines = []

with open(filepath, 'r') as f:
	lines = f.readlines()

with open(filepath, 'w') as f:
	for line in lines:
		if not line.startswith("recipe.hooks.postbuild.3.pattern=\"{compiler.path}teensy_post_compile\""):  # remove post-compile trigger
			f.write(line)
		else:
			print("Removing line '{}'".format(line.strip('\n')))
