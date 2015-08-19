#!/usr/bin/expect

log_user 1

proc install { cmd pkg } {
  puts "Install $pkg"
  send "which $cmd ; echo $?\r\n"
 
 set timeout -1
 expect {
    "1" { 
	  send "apt-get install $pkg\r\n"
	}
    "0" 
    }
  set timeout 60
  expect "# "
}

proc mkfolder { name } {
  send "mkdir -p $name\r\n"
  
  expect "# "
}

if { $argc != 1 } {
  puts "Need a host name"
  exit 1
}

set hostname [ lindex $::argv 0 ]

puts $hostname

spawn ssh $hostname

puts "One"
expect {
  "(yes/no)?" { send "yes\r\n" }
  " $" 
}
puts "Two"

expect {
  "word: " { send "$password\r\n" }
  "*" 
}
puts "Three"

send "sudo -i\r\n"

expect {
  "word for " { send "$password\r\n" }
  "*"
}


install "tsp" "task-spooler"
install "lua" "lua5.2"
#
# set timeout 120
# send "apt-get -y install lua5.2\r\n"
# expect "# "

set timeout 10

send "cd /var/manage/Scripts\r\n"
expect "# "

send "wget -N http://192.168.0.15:8080/manage/Scripts/node.lua\r\n"
expect "# "

send "chmod +x node.lua\r\n"
expect "# "

send "wget -N http://192.168.0.15:8080/manage/Scripts/inifile.lua\r\n"
expect "# "

send "cd /var/manage/Data\r\n"
expect "# "

send "mkdir -p $hostname\r\n"
expect "# "

send "cd $hostname\r\n"
expect "# "

send "wget -N http://192.168.0.15:8080/manage/Data/whatAmI.ini\r\n"
expect "# "

send "cd /var/manage/Scripts\r\n"
expect "# "

set timeout -1
send "./node.lua\r\n"
expect "++"
set timeout 10
send "tsp\r\n"
expect "# "

# interact
