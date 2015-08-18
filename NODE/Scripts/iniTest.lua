#!/usr/bin/lua

require 'inifile'

test = inifile.parse("../Data/base.ini")

print("Look for " ..  test['sudo']['test'])
print("After install " ..  test['sudo']['action'])
