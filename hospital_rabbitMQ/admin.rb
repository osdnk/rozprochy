require 'bunny'
require './employee'

class Admin < Employee

  private
  def initialize
    super()
    @logging_queue = @channel.queue('', exclusive: true)
    @logging_queue.bind(@log_exchange)
    @logging_queue.subscribe(block: false) do |_delivery_info, _properties, body|
      puts "LOG: #{body}"
    end
  end

  public

  def to_s
    "Admin: #{@name}"
  end

  def send_info(info)
      @info_exchange.publish("ADMIN: #{@name}: #{info}")
  end

  def close
    @connection.close
    exit(0)
  end


end

begin
admin = Admin.new
puts admin.to_s

loop do
  puts "MESSAGE:"
  message = STDIN.gets.chomp
  admin.send_info(message)
end

rescue Interrupt => _e
  admin.close
end
