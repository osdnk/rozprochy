require "socket"
require "ipaddr"
mutlicast_addr = "239.0.0.1"
mutlicast_port = 3000

rcv_socket = UDPSocket.new
membership = IPAddr.new(mutlicast_addr).hton + IPAddr.new("0.0.0.0").hton

rcv_socket.setsockopt(:IPPROTO_IP, :IP_ADD_MEMBERSHIP, membership)
rcv_socket.setsockopt(:SOL_SOCKET, :SO_REUSEPORT, 1)
rcv_socket.bind("0.0.0.0", mutlicast_port)

puts "Started logger."
while true do
  message , _ = rcv_socket.recvfrom(255)
  puts message
end
