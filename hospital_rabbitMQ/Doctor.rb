require 'bunny'
require 'securerandom'
class Doctor

  private

  def initialize(name = nil)
    set_name name
    connect_bunny
  end

  def set_name(name)
    @name = name if name.class == String
    @name = @name || SecureRandom.hex
  end

  def connect_bunny
    @connection = Bunny.new
    @connection.start
    @channel = @connection.create_channel
    @queue = channel.queue('somequeue')
  end

  public

  def to_s
    "Doctor: #{@name}"
  end


  def add_patient(name: nil, illness: nil)
    if name.nil? || illness.nil? || name.empty? || illness.empty?
      raise ArgumentError.new("Patient with blank name is not acceptable")
    end
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
  doctor.add_patient(:name =>  name, :illness => illness)
rescue Interrupt => _e
  puts "Closing the connection with rabbitMQ, goodbye."
  doctor.close
end