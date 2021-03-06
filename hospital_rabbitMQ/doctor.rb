require './employee'
class Doctor < Employee

  private

  def initialize
    super()

    @info_queue = @channel.queue('', exclusive: true)
    @info_queue.bind(@info_exchange)
    @info_queue.subscribe(block:false) do |_delivery_info, _properties, body|
      puts "#{body}"
    end


    @response_queue = @channel.queue('', exclusive: true)
    @response_queue.bind(@response_exchange, routing_key: "#{@name}")
    @response_queue.subscribe(block: false) do |_delivery_info, _properties, body|
      puts "Received response: #{body}"
      @log_exchange.publish("#{@name} received: #{body}")
    end
  end


  public

  def to_s
    "Doctor: #{@name}"
  end


  def send(patient_name, illness)
    payload = "#{@name}-#{patient_name}-#{illness}"
    @send_exchange.publish(payload, routing_key: illness)
    @log_exchange.publish("#{@name}: #{payload}, with key: #{illness}")
    puts "Sent a patient to technician to fix #{patient_name}'s #{illness}"
  end
end


begin
doctor = Doctor.new
puts doctor.to_s
loop do
  puts "PATIENT ID"
  name = STDIN.gets.chomp
  puts "PATIENT ILLNESS"
  illness = STDIN.gets.chomp
  doctor.send(name, illness)
  end
rescue Interrupt => _e
  doctor.close
end