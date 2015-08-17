#!/usr/bin/ruby

require 'inifile'
require 'pp'
require 'open-uri'

def readConfig rl
  path = "../Data/"
  for name in rl
    fileName = name + '.ini'
    filePath = path + fileName
    puts filePath
    download = open('http://192.168.0.15:8080/manage/Data/' + fileName )
    IO.copy_stream(download, filePath )
  end
end

def main
  roles = [ "base"]
  
  puts ARGV[0]
  
  if ARGV[0] == nil
    puts "Usage node.rb <hostname>"
    exit 
  end
  
  hostName=ARGV[0]
  
  whatAmI = IniFile.load('../Data/whatAmI.ini')
  cfg = whatAmI['config']
  network = whatAmI['network']
  
  r = cfg['roles']
  webs = network['server']
  
  puts webs
  
  addThese = r.split(',')
  roles = roles + addThese
  
  readConfig roles
  
  for file in roles
    fileName='../Data/' + file + '.ini'
    
    if File.file?( fileName )
      file = IniFile.load('../Data/' + file + '.ini')
      
      for section in file
	data = file[section] 
	
	installed=FALSE
	if data["test"]
	  puts "Test " + data["test"]
	  if data['test'] == 'NONE' 
	    installed=FALSE
	  else
	    installed=system("which " + data["test"])
	  end
	end
	
	if installed
	  puts "Done"
	else
	  cmd = "apt-get -y install " + section
	  puts cmd
	  res = system( cmd )
	  if res
	    puts "Success"
	  else
	    puts "Failed"
	  end
	end
	
	if data["action"] != "NONE"
	  puts data["action"]
	end
      end
    else
      puts "File " + fileName + " not found."
      url = 'http://' + webs + '/manage/Data/' + file + '.ini' 
      puts url
      download = open( url )
#      download = open('http://192.168.0.15:8080/manage/Data/' + file + '.ini' )
      IO.copy_stream(download, fileName )
    end
    
  end
end

main

