require 'bunny'
require 'securerandom'

class Employee
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
  end

  public

  def close
    @connection.close
    exit(0)
  end

end