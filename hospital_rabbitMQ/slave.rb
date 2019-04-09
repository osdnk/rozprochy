require 'bunny'
require 'securerandom'

class Slave
  private

  def initialize(name: nil)

    set_name(name)
    connect_bunny
    subscribe_info

  end

  def set_name(name)
    @name = name if name.class == String
    @name = name || SecureRandom.hex
  end

  def connect_bunny
    @connection = Bunny.new
    @connection.start
    @channel = @connection.create_channel
    @send_exchange = @channel.direct('senders_exchange')
    @response_exchange = @channel.direct('response_exchange')
    @log_exchange = @channel.fanout('log_exchange')
    @info_exchange = @channel.fanout('info_exchange')
  end

  def subscribe_info
    @info_queue = @channel.queue('', exclusive: true)
    @info_queue.bind(@info_exchange)
    @info_queue.subscribe(block:false) do |_delivery_info, _properties, body|
      puts "#{body}"
    end
  end

end