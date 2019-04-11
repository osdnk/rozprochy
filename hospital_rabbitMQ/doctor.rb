require './slave'
class Doctor < Slave

  private

  def initialize(name: nil)
    super(name: name)
    subscribe_responses
  end

  def subscribe_responses
    #create exclusive response queue
    @response_queue = @channel.queue('', exclusive: true)
    #We bind the queue with our name's routing key
    @response_queue.bind(@response_exchange, routing_key: "#{@name}")
    #Subscribe on responses (nonblock)
    @response_queue.subscribe(block: false) do |_delivery_info, _properties, body|
      puts "Received response: #{body}"
      #Send out logs to administrator's fanout
      @log_exchange.publish("#{@name} HAS RECEIVED: #{body}")
    end
  end

  public

  def to_s
    "Doctor: #{@name}"
  end


  def dispatch_examination(patient_name: nil, illness: nil)
    if patient_name.nil? || illness.nil? || patient_name.empty? || illness.empty?
      raise ArgumentError.new("Patient/Illness with blank name is not acceptable")
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


begin
doctor = Doctor.new name: ARGV[0]
puts doctor.to_s
loop do
  puts "Please enter the patient's name"
  name = STDIN.gets.chomp
  puts "Please enter the patient's illness"
  illness = STDIN.gets.chomp
  doctor.dispatch_examination(:patient_name => name, :illness => illness)
  end
rescue Interrupt => _e
  puts "Closing the connection with rabbitMQ, goodbye."
  doctor.close
end