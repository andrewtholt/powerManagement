#!/usr/bin/expect

log_user 0

proc install { cmd pkg } {
  puts "Install $pkg"
  send "which $cmd ; echo $?\r\n"
 
  expect {
    "1" { 
	  send "apt-get install $pkg\r\n"
	}
    "0" { }
    }
  expect "# "
}

proc mkfolder { name } {
  send "mkdir -p $name\r\n"
  
  expect "# "
}

if { $argc != 2 } {
  puts "Need a host name"
  exit 1
}

set hostname [ lindex $::argv 0 ]
set password [ lindex $::argv 1 ]

puts $hostname

spawn ssh $hostname

expect {
  "(yes/no)?" { send "yes\r\n" }
}

expect {
  "word: " { send "$password\r\n" }
}

expect " $"
send "sudo -i\r\n"

expect {
  "word for " { send "$password\r\n" }
}

expect "# "

install "tsp" "task-spooler"
install "ruby" "ruby"
install "wget" "wget"

set data "/var/manage/Data"
set scripts "/var/manage/Scripts"

mkfolder "$data"
mkfolder "$scripts"

send "cd $data\r\n"
expect "# "

log_user 1
set cmd [ format "wget http://192.168.0.15:8080/manage/Data/%s/whatAmI.ini -O ./whatAmI.ini\r\n" $hostname ]
# send "wget http://192.168.0.15:8080/manage/Data/whatAmI.ini -O ./whatAmI.ini\r\n"
send $cmd

expect "# "

send "cd $scripts\r\n"
expect "#"


send "wget http://192.168.0.15:8080/manage/Scripts/node.rb -O ./node.rb\r\n"
expect "# "

send "chmod +x ./node.rb\r\n"
expect "# "

log_user 1

send "gem install inifile\r\n"
expect "# "

send "./node.rb `hostname`\r\n"
expect "# "

interact