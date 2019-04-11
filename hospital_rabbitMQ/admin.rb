require 'bunny'
class Admin

  private
  def initialize(name: nil)
    set_name(name)
    connect_bunny
    subscribe_logs
  end

  def set_name(name)
    @name = name if name.class == String
    @name = name || SecureRandom.hex
  end

  def connect_bunny
    @connection = Bunny.new
    @connection.start
    @channel = @connection.create_channel
    @log_exchange = @channel.fanout('log_exchange')
    @info_exchange = @channel.fanout('info_exchange')
  end

  def subscribe_logs
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
      @info_exchange.publish("ALERT: ADMIN_ID: #{@name} says: #{info}")
  end

  def close
    @connection.close
    exit(0)
  end


end

begin
admin = Admin.new name: ARGV[0]
puts admin.to_s

loop do
  puts "Please enter info to send:"
  message = STDIN.gets.chomp
  admin.send_info(message)
end

rescue Interrupt => _e
  puts "Closing the connection with rabbitMQ, goodbye."
  admin.close
end
