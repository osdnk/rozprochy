require 'bunny'
require 'securerandom'

class NonAdminEmployee
  private

  def initialize
    @name = SecureRandom.hex
    @connection = Bunny.new
    @connection.start
    @channel = @connection.create_channel
    @send_exchange = @channel.direct('senders_exchange')
    @response_exchange = @channel.direct('response_exchange')
    @log_exchange = @channel.fanout('log_exchange')
    @info_exchange = @channel.fanout('info_exchange')
    @info_queue = @channel.queue('', exclusive: true)
    @info_queue.bind(@info_exchange)
    @info_queue.subscribe(block:false) do |_delivery_info, _properties, body|
      puts "#{body}"
    end
  end

  public

  def close
    @connection.close
    exit(0)
  end

end