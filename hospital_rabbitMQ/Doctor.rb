require 'bunny'
require 'securerandom'
class Doctor

  private

  def initialize(name: nil)
    set_name name
    connect_bunny
    subscribe_info
    subscribe_responses
  end

  def set_name(name)
    @name = name if name.class == String
    @name = @name || SecureRandom.hex
  end

  def connect_bunny
    @connection = Bunny.new
    @connection.start
    @channel = @connection.create_channel
    @send_exchange = @channel.direct('senders_exchange')
    @response_exchange = @channel.direct('response_exchange')
    @info_exchange = @channel.fanout('info_exchange')
    @log_exchange = @channel.fanout('log_exchange')
  end

  def subscribe_info
    @info_queue = @channel.queue('', exclusive: true)
    @info_queue.bind(@info_exchange)
    @info_queue.subscribe(block:false) do |_delivery_info, _properties, body|
      puts "#{body}"
    end
  end

  def subscribe_responses
    #create exclusive response queue
    @response_queue = @channel.queue('', exclusive: true)
    #We bind the queue with our name's routing key
    @response_queue.bind(@response_exchange, routing_key: "#{@name}")
    #Subscribe on responses (nonblock)
    @response_queue.subscribe(block: false) do |_delivery_info, _properties, body|
      puts "Received response: #{body}"
      @log_exchange.publish("#{@name} HAS RECEIVED: #{body}")
    end
  end

  public

  def to_s
    "Doctor: #{@name}"
  end


  def add_patient(patient_name: nil, illness: nil)
    if patient_name.nil? || illness.nil? || patient_name.empty? || illness.empty?
      raise ArgumentError.new("Patient with blank name is not acceptable")
    end
    payload = "#{@name}-#{patient_name}-#{illness}"
    @send_exchange.publish(payload, routing_key: illness)
    @log_exchange.publish("#{@name} SENDING REQUEST: #{payload}, TO DIRECT WITH KEY: #{illness}")
    puts "Sent a test to run on #{patient_name}'s #{illness}"

  end

  def close
    @connection.close
    exit(0)
  end

end


doctor = Doctor.new name: ARGV[0]
puts doctor.to_s

loop do
  puts "Please enter the patient's name"
  name = STDIN.gets.chomp
  puts "Please enter the patient's illness"
  illness = STDIN.gets.chomp
  doctor.add_patient(:patient_name => name, :illness => illness)
rescue Interrupt => _e
  puts "Closing the connection with rabbitMQ, goodbye."
  doctor.close
end