require 'securerandom'
require 'bunny'
class Technician

  private

  def initialize(skills: nil, name: nil)
    set_skills(skills)
    set_name(name)
    connect_bunny
    subscribe_info
    subscribe_requests
  end

  def set_skills(skills)
    if skills.nil?
      raise ArgumentError("No skills specified for a technician")
    elsif skills.length <= 2
      @skills = []
      skills.each do |skill|
        @skills << skill unless (!skill.class == String)
      end
      @skills.freeze
    else
      raise ArgumentError.new("Cannot have a technician with more than 2 skills!")
    end
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

  def subscribe_requests
    request_queues = []
    @skills.each do |skill|
      skill_queue = @channel.queue("#{skill}")
      skill_queue.bind(@send_exchange, routing_key: "#{skill}")
      request_queues << skill_queue
    end

    request_queues.each do |req_queue|
      req_queue.subscribe(block: request_queues[-1].equal?(req_queue)) do |delivery_info, _, body|
        #Log the received request
        @log_exchange.publish("#{@name} HAS RECEIVED: #{body}")
        #Parse request
        payload_tbl = body.split("-")
        requesting_doctor = payload_tbl[0]
        job_hash = SecureRandom.hex[0..5]
        response_msg = "#{payload_tbl[1]}  #{delivery_info.routing_key} - done (by #{@name}, hash: #{job_hash}) "
        #Simulate doing some work
        sleep(5)
        @response_exchange.publish(response_msg, routing_key: requesting_doctor)
        #Log the response
        @log_exchange.publish("#{@name} SENDING RESPONSE: #{response_msg}")
      end
    end

  end

  def subscribe_info
    @info_queue = @channel.queue('', exclusive: true)
    @info_queue.bind(@info_exchange)
    @info_queue.subscribe(block:false) do |_delivery_info, _properties, body|
      puts "#{body}"
    end
  end

  public

  def to_s
    "Technician: #{@name} , skills: #{@skills}"
  end


end


#Start running , Usage: First arg is name , and second/third are skills
Technician.new(skills: [ARGV[1], ARGV[2]], name: ARGV[0])

